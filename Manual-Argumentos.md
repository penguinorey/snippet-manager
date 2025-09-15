# Guía completa de uso de argumentos en Snippet Manager

## Conceptos básicos

Los argumentos te permiten hacer tus snippets más flexibles y reutilizables. Se pasan al ejecutar el snippet y se reemplazan automáticamente en el comando.

## Sintaxis básica

```bash
# Agregar snippet con placeholders
sn add <nombre> "<comando con ${placeholders}>"

# Ejecutar con argumentos
sn <nombre> argumento1 argumento2 argumento3 ...
```

## Tipos de placeholders

### 1. Placeholders por posición (${1}, ${2}, ${3}, ...)

```bash
# Snippet con múltiples argumentos
sn add copiar "cp ${1} ${2}"

# Uso - el orden importa!
sn copiar archivo.txt /backup/
# Se convierte en: cp archivo.txt /backup/
```

### 2. Placeholders con nombres (${nombre}, ${archivo}, ${extension}, ${ruta})

```bash
# Snippet con placeholders nombrados
sn add comprimir "tar -czf ${nombre}.tar.gz ${archivo}"

# Uso - más legible
sn comprimir proyecto /home/user/project
# Se convierte en: tar -czf proyecto.tar.gz /home/user/project
```

## Ejemplos prácticos

### Ejemplo 1: Búsqueda de archivos

```bash
# Crear snippet
sn add buscar "find . -name \"${1}\" -type f"

# Usar con diferentes argumentos
sn buscar "*.txt"          # Busca todos los .txt
sn buscar "*.pdf"          # Busca todos los .pdf  
sn buscar "documento*"     # Busca archivos que empiecen con "documento"
```

### Ejemplo 2: Manipulación de archivos

```bash
# Crear snippet para renombrar
sn add renombrar "mv ${1} ${2}"

# Usar
sn renombrar viejo.txt nuevo.txt
# Se convierte en: mv viejo.txt nuevo.txt
```

### Ejemplo 3: Comandos con múltiples argumentos

```bash
# Snippet complejo con 3 argumentos
sn add procesar "convert ${1} -resize ${2} -quality ${3} output.jpg"

# Usar
sn procesar imagen.png 800x600 90
# Se convierte en: convert imagen.png -resize 800x600 -quality 90 output.jpg
```

### Ejemplo 4: Placeholders con nombres

```bash
# Snippet más legible con nombres
sn add backup "tar -czf ${nombre}_backup.tar.gz ${archivo} --directory=${ruta}"

# Usar
sn backup proyecto /home/user/project /backups
# Se convierte en: tar -czf proyecto_backup.tar.gz /home/user/project --directory=/backups
```

## Equivalencia entre placeholders

| Placeholder | Equivalente | Descripción |
|------------|-------------|-------------|
| `${nombre}` | `${1}` | Primer argumento |
| `${archivo}` | `${1}` | Primer argumento |
| `${extension}` | `${2}` | Segundo argumento |
| `${ruta}` | `${3}` | Tercer argumento |

## Consejos de uso

### 1. Usar comillas para argumentos con espacios

```bash
# Argumentos con espacios necesitan comillas
sn buscar "archivo con espacios.txt"
```

### 2. Escapar caracteres especiales

```bash
# Si tu comando tiene $, necesitas escaparlo
sn add precio "echo El precio es \\$${1}"
sn precio 100
# Output: El precio es $100
```

### 3. Combinar placeholders

```bash
# Puedes usar múltiples placeholders del mismo argumento
sn add info "echo 'Archivo: ${1}, Extensión: ${2}, Nombre: ${nombre}'"
sn info documento txt
# Output: Archivo: documento, Extensión: txt, Nombre: documento
```

### 4. Argumentos opcionales

```bash
# Si un placeholder no tiene argumento, se queda como texto
sn add saludo "echo Hola ${1}"
sn saludo
# Output: Hola ${1}  (no se reemplaza)
```

## Ejemplos avanzados

### Ejemplo 5: Scripting con variables

```bash
# Crear backup con timestamp
sn add backup-completo "tar -czf backup_${1}_$(date +%Y%m%d).tar.gz ${2} && echo Backup de ${nombre} completado"
sn backup-completo proyecto /home/user/project
```

### Ejemplo 6: Comandos condicionales

```bash
# Verificar y luego ejecutar
sn add instalar-seguro "if command -v ${1} &> /dev/null; then echo 'Ya instalado'; else sudo apt install -y ${1}; fi"
sn instalar-seguro htop
```

### Ejemplo 7: Trabajo con redes

```bash
# Test de conectividad
sn add ping-host "ping -c 4 ${1} | grep 'packet loss'"
sn ping-host google.com
```

## Preguntas frecuentes

### ¿Puedo usar más de 3 argumentos?
**¡Sí!** Puedes usar `${4}`, `${5}`, `${6}`, etc. sin límite.

### ¿Qué pasa si no paso todos los argumentos?
Los placeholders sin argumento se mantienen como texto en el comando.

### ¿Puedo usar el mismo argumento múltiples veces?
**¡Sí!** Puedes usar `${1}` tantas veces como necesites en el mismo comando.

### ¿Los argumentos son case-sensitive?
**Sí**, los placeholders `${nombre}` y `${NOMBRE}` son diferentes.

## Resumen

```bash
# Sintaxis general
sn add <nombre> "<comando con ${placeholders}>"
sn <nombre> arg1 arg2 arg3 ...

# Placeholders disponibles
${1}, ${2}, ${3}, ...    # Por posición
${nombre}, ${archivo}    # Primer argumento  
${extension}             # Segundo argumento
${ruta}                  # Tercer argumento
```
