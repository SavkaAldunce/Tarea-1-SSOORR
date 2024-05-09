# Autores

- Nombre: Savka Andrea Aldunce Aravena, Número de alumno: 19625561
- Nombre: Cristóbal Andrés Pérez Pérez, Número de alumno: 19640196

# Principales decisiones

- Guardar los archivos OsmsFile en una lista ligada para su correcta manipulación
- Para el struct OsmsFile decidimos incluirle los atributos file_id, process_id, file_name, current_position current_position_read. Donde current_position desde que byte del frame se empieza a escribir y current_position_read es el byte en el cual quedo la lectura la ultima vez que se leyó el archivo con os_read.

# Supuestos Tarea 1

- Se asume que la escritura de un archivo se hara únicamente en 1 frame (Cada archivo tiene su frame asociado y no lo puede compartir con otro archivo). Por lo tanto la lectura también se realiza en un único frame por archivo

- Por favor no borrar la función "os_free()" del main, esta es la funcion que requerimos para liberar la memoria de manera correcta

