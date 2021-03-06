/*
 * Global.h
 *
 *  Created on: 20/9/2016
 *      Author: utnso
 */

#ifndef POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_
#define POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_

#include "osada.h"
#include <commons/bitarray.h>


pthread_mutex_t semaforoBitmap, semaforoTablaDeArchivos, semaforoTablaDeAsignaciones;
char * disco;
osada_header* header;
t_bitarray* bitmap;
osada_file* tablaDeArchivos;
osada_block_pointer* tablaDeAsignaciones;
osada_block* bloquesDeDatos;
int inicioDeBloqueDeDatos;
int cantidadDeBloques;
int sobraDeBitmap;


#endif /* POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_ */
