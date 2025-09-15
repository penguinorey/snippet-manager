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

    std::string escapeJson(const std::string& s) {
        std::ostringstream o;
        for (auto c = s.cbegin(); c != s.cend(); c++) {
            if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f')) {
                o << "\\" << *c;
            } else {
                o << *c;
            }
        }
        return o.str();
    }

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
            file << "    \"snippet\":\"" << escapeJson(name) << "\"," << std::endl;
            file << "    \"command\":\"" << escapeJson(command) << "\"" << std::endl;
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

    bool updateSnippet(const std::string& name, const std::string& newCommand) {
        if (!loadSnippets()) return false;

        if (snippets.find(name) == snippets.end()) {
            std::cerr << "Error: Snippet '" << name << "' no encontrado." << std::endl;
            return false;
        }

        snippets[name] = newCommand;
        std::cout << "Snippet '" << name << "' actualizado correctamente." << std::endl;
        return saveSnippets();
    }

    bool deleteSnippet(const std::string& name) {
        if (!loadSnippets()) return false;

        auto it = snippets.find(name);
        if (it == snippets.end()) {
            std::cerr << "Error: Snippet '" << name << "' no encontrado." << std::endl;
            return false;
        }

        std::cout << "¿Estás seguro de que quieres eliminar el snippet '" << name << "'? (s/N): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "s" && response != "S") {
            std::cout << "Eliminación cancelada." << std::endl;
            return false;
        }

        snippets.erase(it);
        std::cout << "Snippet '" << name << "' eliminado correctamente." << std::endl;
        return saveSnippets();
    }

    std::string escapeArgument(const std::string& arg) {
        if (arg.find(' ') != std::string::npos) {
            return "\"" + arg + "\"";
        }
        return arg;
    }

    bool executeSnippet(const std::string& name, const std::vector<std::string>& args = {}) {
        if (!loadSnippets()) return false;

        auto it = snippets.find(name);
        if (it == snippets.end()) {
            std::cerr << "Error: Snippet '" << name << "' no encontrado." << std::endl;
            return false;
        }

        std::string command = it->second;
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string placeholder = "${" + std::to_string(i + 1) + "}";
            size_t pos = command.find(placeholder);
            while (pos != std::string::npos) {
                std::string escapedArg = escapeArgument(args[i]);
                command.replace(pos, placeholder.length(), escapedArg);
                pos = command.find(placeholder, pos + escapedArg.length());
            }
        }
        
        if (!args.empty()) {
            std::string placeholder = "${nombre}";
            size_t pos = command.find(placeholder);
            while (pos != std::string::npos) {
                std::string escapedArg = escapeArgument(args[0]);
                command.replace(pos, placeholder.length(), escapedArg);
                pos = command.find(placeholder, pos + escapedArg.length());
            }
            
            placeholder = "${archivo}";
            pos = command.find(placeholder);
            while (pos != std::string::npos) {
                std::string escapedArg = escapeArgument(args[0]);
                command.replace(pos, placeholder.length(), escapedArg);
                pos = command.find(placeholder, pos + escapedArg.length());
            }
            
            if (args.size() > 1) {
                placeholder = "${extension}";
                pos = command.find(placeholder);
                while (pos != std::string::npos) {
                    std::string escapedArg = escapeArgument(args[1]);
                    command.replace(pos, placeholder.length(), escapedArg);
                    pos = command.find(placeholder, pos + escapedArg.length());
                }
            }
            
            if (args.size() > 2) {
                placeholder = "${ruta}";
                pos = command.find(placeholder);
                while (pos != std::string::npos) {
                    std::string escapedArg = escapeArgument(args[2]);
                    command.replace(pos, placeholder.length(), escapedArg);
                    pos = command.find(placeholder, pos + escapedArg.length());
                }
            }
        }

        std::cout << "Ejecutando: " << command << std::endl;
        return system(command.c_str()) == 0;
    }

    bool fileExists() {
        return fs::exists(configPath);
    }

    bool snippetExists(const std::string& name) {
        if (!loadSnippets()) return false;
        return snippets.find(name) != snippets.end();
    }
};

void showHelp() {
    std::cout << "Uso: sn [comando] [argumentos...]" << std::endl;
    std::cout << "Comandos:" << std::endl;
    std::cout << "  init                 - Inicializa el archivo de snippets" << std::endl;
    std::cout << "  list                 - Lista todos los snippets disponibles" << std::endl;
    std::cout << "  add <nombre> \"<comando>\" - Agrega un nuevo snippet" << std::endl;
    std::cout << "  update <nombre> \"<comando>\" - Actualiza un snippet existente" << std::endl;
    std::cout << "  delete <nombre>      - Elimina un snippet" << std::endl;
    std::cout << "  <nombre> [args...]   - Ejecuta un snippet con argumentos" << std::endl;
    std::cout << "  help                 - Muestra esta ayuda" << std::endl;
    std::cout << std::endl;
    std::cout << "Placeholders disponibles en los comandos:" << std::endl;
    std::cout << "  ${1}, ${2}, ${3}, ... - Argumentos por posicion" << std::endl;
    std::cout << "  ${nombre}             - Primer argumento" << std::endl;
    std::cout << "  ${archivo}            - Primer argumento" << std::endl;
    std::cout << "  ${extension}          - Segundo argumento" << std::endl;
    std::cout << "  ${ruta}               - Tercer argumento" << std::endl;
    std::cout << std::endl;
    std::cout << "NOTA: Usa \\${1} en lugar de ${1} para evitar que el shell expanda la variable" << std::endl;
    std::cout << "Ejemplo: sn add push \"git add . && git commit -m \\${1} && git push origin\"" << std::endl;
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
            std::cerr << "Ejemplo: sn add push \"git add . && git commit -m \\${1} && git push origin\"" << std::endl;
            return 1;
        }
        if (!manager.fileExists()) {
            std::cerr << "Error: Primero ejecuta 'sn init' para inicializar los snippets" << std::endl;
            return 1;
        }
        manager.addSnippet(argv[2], argv[3]);
    }
    else if (command == "update") {
        if (argc < 4) {
            std::cerr << "Uso: sn update <nombre> \"<nuevo_comando>\"" << std::endl;
            std::cerr << "Ejemplo: sn update push \"git add . && git commit -m \\${1} && git push origin master\"" << std::endl;
            return 1;
        }
        if (!manager.fileExists()) {
            std::cerr << "Error: Primero ejecuta 'sn init' para inicializar los snippets" << std::endl;
            return 1;
        }
        manager.updateSnippet(argv[2], argv[3]);
    }
    else if (command == "delete") {
        if (argc < 3) {
            std::cerr << "Uso: sn delete <nombre>" << std::endl;
            std::cerr << "Ejemplo: sn delete push" << std::endl;
            return 1;
        }
        if (!manager.fileExists()) {
            std::cerr << "Error: Primero ejecuta 'sn init' para inicializar los snippets" << std::endl;
            return 1;
        }
        manager.deleteSnippet(argv[2]);
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
