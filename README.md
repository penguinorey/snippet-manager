# Snippet Manager (sn)

Una herramienta CLI escrita en C++ para gestionar y ejecutar snippets de comandos personalizados.

## Características

- Crear y gestionar snippets de comandos
- Ejecutar snippets con argumentos
- Soporte para múltiples placeholders
- Interfaz de línea de comandos intuitiva
- Almacenamiento en JSON (~/.snippets.json)

## Instalación

### Compilación desde código fuente

```bash
# Clonar el repositorio
git clone <tu-repositorio>
cd snippet-manager

# Compilar
make

# Instalar (opcional)
sudo make install
```

### Instalación manual

```bash
g++ -std=c++17 -o sn sn.cpp
sudo cp sn /usr/local/bin/
```

## Uso básico

```bash
# Inicializar el archivo de snippets
sn init

# Listar todos los snippets
sn list

# Agregar un nuevo snippet
sn add <nombre> "<comando>"

# Ejecutar un snippet
sn <nombre> [argumentos...]

# Mostrar ayuda
sn help
```

## Ejemplos de uso

### Snippets simples

```bash
# Agregar snippet para listar archivos Word
sn add listar-word "ls -l | grep *.docx"

# Agregar snippet para instalar paquetes
sn add instala "sudo apt install -y"

# Ejecutar snippets
sn listar-word
sn instala vim
```

### Snippets con argumentos

```bash
# Búsqueda de archivos
sn add busca "find . -name \\${1} -type f"
sn busca "*.txt"

# Copia de archivos con múltiples argumentos
sn add copia "cp \${1} \${2}"
sn copia documento.pdf /backup/

# Comandos complejos con placeholders nombrados
sn add comprimir "tar -czf \${nombre}.tar.gz \${archivo} --directory=\${ruta}"
sn comprimir mi_proyecto /home/user/projects /backup
```

Saber más [manual de sn con argumentos](Manual-Argumentos.md)

## Placeholders disponibles

### Por posición
- `\${1}` - Primer argumento
- `\${2}` - Segundo argumento  
- `\${3}` - Tercer argumento
- `\${n}` - Enésimo argumento

### Con nombres descriptivos
- `\${nombre}` - Primer argumento
- `\${archivo}` - Primer argumento
- `\${extension}` - Segundo argumento
- `\${ruta}` - Tercer argumento

## Estructura del archivo de snippets

Los snippets se almacenan en `~/.snippets.json`:

```json
[
  {
    "snippet": "listar-word",
    "command": "ls -l | grep *.docx"
  },
  {
    "snippet": "instala", 
    "command": "sudo apt install -y"
  },
  {
    "snippet": "busca-pdf",
    "command": "find . -name \${nombre}.pdf"
  }
]
```

## Comandos disponibles

| Comando | Descripción |
|---------|-------------|
| `sn init` | Inicializa el archivo de snippets |
| `sn list` | Lista todos los snippets disponibles |
| `sn add <nombre> "<comando>"` | Agrega un nuevo snippet |
| `sn <nombre> [args...]` | Ejecuta un snippet con argumentos |
| `sn help` | Muestra la ayuda |

## Ejemplos avanzados

```bash
# Reemplazo de texto en archivos
sn add reemplazar "sed 's/\${1}/\${2}/g' \${3} > \${3}.modificado"
sn reemplazar viejo nuevo archivo.txt

# Descarga y procesamiento
sn add descargar "wget \${1} && tar -xzf \${archivo} && cd \${nombre} && make"
sn descargar http://ejemplo.com/package.tar.gz

# Backup con timestamp
sn add backup "tar -czf backup_\$(date +%Y%m%d).tar.gz \${1} && mv backup_*.tar.gz \${2}/"
sn backup /home/user/documents /mnt/backup
```

## Troubleshooting

**Error: "No se pudo abrir el archivo ~/.snippets.json"**
```bash
sn init
```

**Error: "Snippet no encontrado"**
```bash
sn list  # Verifica que el snippet exista
```

**Los argumentos no se reemplazan**
- Asegúrate de usar comillas dobles alrededor del comando
- Usa los placeholders correctos (\${1}, \${2}, etc.)


## Contribuir

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama para tu feature
3. Commit tus cambios
4. Push a la rama
5. Abre un Pull Request

## Soporte

Si encuentras algún problema o tienes sugerencias:
- Abre un issue en GitHub
- Contacta al maintainer


