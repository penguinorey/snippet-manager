#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <cstdlib>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

class SnippetManager {
private:
    std::string configPath;
    std::map<std::string, std::string> snippets;

public:
    SnippetManager() : configPath(std::string(getenv("HOME")) + "/.snippets.json") {}

    bool init() {
        std::ofstream file(configPath);
        if (!file) {
            std::cerr << "Error: No se pudo crear el archivo " << configPath << std::endl;
            return false;
        }
        file << "[]" << std::endl;
        file.close();
        std::cout << "Archivo de snippets inicializado: " << configPath << std::endl;
        return true;
    }

    bool loadSnippets() {
        std::ifstream file(configPath);
        if (!file) {
            std::cerr << "Error: No se pudo abrir el archivo " << configPath << std::endl;
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        file.close();

        snippets.clear();
        size_t pos = 0;
        
        while ((pos = content.find("\"snippet\"", pos)) != std::string::npos) {
            size_t nameStart = content.find('\"', pos + 9) + 1;
            size_t nameEnd = content.find('\"', nameStart);
            std::string name = content.substr(nameStart, nameEnd - nameStart);

            size_t cmdPos = content.find("\"command\"", nameEnd);
            size_t cmdStart = content.find('\"', cmdPos + 9) + 1;
            size_t cmdEnd = content.find('\"', cmdStart);
            std::string command = content.substr(cmdStart, cmdEnd - cmdStart);

            snippets[name] = command;
            pos = cmdEnd;
        }

        return true;
    }

    bool saveSnippets() {
        std::ofstream file(configPath);
        if (!file) {
            std::cerr << "Error: No se pudo abrir el archivo " << configPath << std::endl;
            return false;
        }

        file << "[" << std::endl;
        bool first = true;
        for (const auto& [name, command] : snippets) {
            if (!first) file << "," << std::endl;
            file << "  {" << std::endl;
            file << "    \"snippet\":\"" << name << "\"," << std::endl;
            file << "    \"command\":\"" << command << "\"" << std::endl;
            file << "  }";
            first = false;
        }
        file << std::endl << "]" << std::endl;
        file.close();
        return true;
    }

    void listSnippets() {
        if (!loadSnippets()) return;

        if (snippets.empty()) {
            std::cout << "No hay snippets disponibles." << std::endl;
            return;
        }

        for (const auto& [name, command] : snippets) {
            std::cout << name << ": \"" << command << "\"" << std::endl;
        }
    }

    bool addSnippet(const std::string& name, const std::string& command) {
        if (!loadSnippets()) return false;

        if (snippets.find(name) != snippets.end()) {
            std::cout << "El snippet '" << name << "' ya existe. ¿Sobrescribir? (s/N): ";
            std::string response;
            std::getline(std::cin, response);
            if (response != "s" && response != "S") {
                return false;
            }
        }

        snippets[name] = command;
        return saveSnippets();
    }

    bool executeSnippet(const std::string& name, const std::vector<std::string>& args = {}) {
        if (!loadSnippets()) return false;

        auto it = snippets.find(name);
        if (it == snippets.end()) {
            std::cerr << "Error: Snippet '" << name << "' no encontrado." << std::endl;
            return false;
        }

        std::string command = it->second;
        
        // Reemplazar argumentos numéricos (${1}, ${2}, ${3}, ...)
        for (size_t i = 0; i < args.size(); ++i) {
            std::string placeholder = "${" + std::to_string(i + 1) + "}";
            size_t pos = command.find(placeholder);
            while (pos != std::string::npos) {
                command.replace(pos, placeholder.length(), args[i]);
                pos = command.find(placeholder, pos + args[i].length());
            }
        }
        
        // Reemplazar argumentos con nombres específicos
        if (!args.empty()) {
            // ${nombre} -> primer argumento
            std::string placeholder = "${nombre}";
            size_t pos = command.find(placeholder);
            while (pos != std::string::npos) {
                command.replace(pos, placeholder.length(), args[0]);
                pos = command.find(placeholder, pos + args[0].length());
            }
            
            // ${archivo} -> primer argumento
            placeholder = "${archivo}";
            pos = command.find(placeholder);
            while (pos != std::string::npos) {
                command.replace(pos, placeholder.length(), args[0]);
                pos = command.find(placeholder, pos + args[0].length());
            }
            
            // ${extension} -> segundo argumento (si existe)
            if (args.size() > 1) {
                placeholder = "${extension}";
                pos = command.find(placeholder);
                while (pos != std::string::npos) {
                    command.replace(pos, placeholder.length(), args[1]);
                    pos = command.find(placeholder, pos + args[1].length());
                }
            }
            
            // ${ruta} -> tercer argumento (si existe)
            if (args.size() > 2) {
                placeholder = "${ruta}";
                pos = command.find(placeholder);
                while (pos != std::string::npos) {
                    command.replace(pos, placeholder.length(), args[2]);
                    pos = command.find(placeholder, pos + args[2].length());
                }
            }
        }

        std::cout << "Ejecutando: " << command << std::endl;
        return system(command.c_str()) == 0;
    }

    bool fileExists() {
        return fs::exists(configPath);
    }
};

void showHelp() {
    std::cout << "Uso: sn [comando] [argumentos...]" << std::endl;
    std::cout << "Comandos:" << std::endl;
    std::cout << "  init          - Inicializa el archivo de snippets" << std::endl;
    std::cout << "  list          - Lista todos los snippets disponibles" << std::endl;
    std::cout << "  add <nombre> \"<comando>\" - Agrega un nuevo snippet" << std::endl;
    std::cout << "  <nombre> [args...]     - Ejecuta un snippet con argumentos" << std::endl;
    std::cout << std::endl;
    std::cout << "Placeholders disponibles en los comandos:" << std::endl;
    std::cout << "  ${1}, ${2}, ${3}, ... - Argumentos por posición" << std::endl;
    std::cout << "  ${nombre}             - Primer argumento" << std::endl;
    std::cout << "  ${archivo}            - Primer argumento" << std::endl;
    std::cout << "  ${extension}          - Segundo argumento" << std::endl;
    std::cout << "  ${ruta}               - Tercer argumento" << std::endl;
    std::cout << std::endl;
    std::cout << "Ejemplos:" << std::endl;
    std::cout << "  sn add busca \"find . -name \\${1} -type f\"" << std::endl;
    std::cout << "  sn busca \"*.txt\"" << std::endl;
    std::cout << "  sn add copia \"cp \\${1} \\${2}/\"" << std::endl;
    std::cout << "  sn copia archivo.txt /backup/" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        showHelp();
        return 1;
    }

    SnippetManager manager;

    std::string command = argv[1];

    if (command == "init") {
        if (!manager.init()) {
            return 1;
        }
    } 
    else if (command == "list") {
        manager.listSnippets();
    }
    else if (command == "add") {
        if (argc < 4) {
            std::cerr << "Uso: sn add <nombre> \"<comando>\"" << std::endl;
            std::cerr << "Ejemplo: sn add busca \"find . -name \\${1}\"" << std::endl;
            return 1;
        }
        if (!manager.fileExists()) {
            std::cerr << "Error: Primero ejecuta 'sn init' para inicializar los snippets" << std::endl;
            return 1;
        }
        manager.addSnippet(argv[2], argv[3]);
    }
    else if (command == "help") {
        showHelp();
    }
    else {
        if (!manager.fileExists()) {
            std::cerr << "Error: Primero ejecuta 'sn init' para inicializar los snippets" << std::endl;
            return 1;
        }

        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        if (!manager.executeSnippet(command, args)) {
            return 1;
        }
    }

    return 0;
}
