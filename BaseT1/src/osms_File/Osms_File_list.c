#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "Osms_File_list.h"
#include "Osms_File.h"

// Definimos el inicializador de la lista, recibe el valor a guardar
FilesList* fileslist_init(OsmsFile value)
{
  // Pedimos memoria para la lista
  FilesList* list = calloc(1, sizeof(FilesList));

  // Inicializamos la lista directamente
  *list = (FilesList) {
    .value = value,
    .next = NULL, // como no tiene siguiente lista y es un puntero, debe partir en NULL
  };

  // Retornamos la lista
  return list;
}

// Definimos una funcion para agregar un valor a la lista
void fileslist_append(FilesList* list, OsmsFile value)
{
  // Aqui podemos apreciar la utilidad de esta modelacion, el código queda
  // super ordenado y legible

  // Debemos ir al final de la lista, para esto podemos utilizar un while
  FilesList *last = list;
  while (last->next) {
    last = last->next;
  }

  // Creamos el nuevo nodo
  FilesList *new_list = fileslist_init(value);

  // Lo agregamos como el nodo siguiente al ultimo nodo de la lista
  last->next = new_list;
}

// Definimos una funcion que nos retorne el valor en un indice de la lista
OsmsFile* fileslist_at_index(FilesList* list, int index)
{
  // Nos ubicamos en el indice correspondiente
  FilesList* node = list;
  for (size_t i = 0; i < index; i++)
  {
    node = node->next;
  }

  // Retornamos el valor
  return &node->value;
}

// Definimos el destructor de la lista
void fileslist_destroy(FilesList *list)
{

  if (list == NULL)
  {
    return;
  }

  // Si hay un nodo en la sig posicion, llamamos recursivamente a la funcion
  if (list->next)
  {
    fileslist_destroy(list->next);
  }
  
  // Luego, liberamos la lista
  free(list);
}