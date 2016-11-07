/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"

void* memoria(int cantidad) {
	void* puntero = NULL;
	if (cantidad !=0) {
		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

t_paquete* empaquetar(int codigo, void* datos, int size){
	t_paquete* paquete= malloc(sizeof(t_paquete));
	paquete->codigo= codigo;
	paquete->datos= datos;
	paquete->tamanio= size;
	return paquete;
}

void* acoplador(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
	void* paqueteSalida = memoria(size_header + paquete->tamanio);
	memcpy(paqueteSalida, paquete, size_header);
	memcpy(paqueteSalida + size_header, paquete->datos, paquete->tamanio);
	return paqueteSalida;
}

t_paquete* desacoplador1(char* buffer,int sizeBuffer) {
    	t_paquete* paquete;
    	paquete= memoria(sizeof(t_paquete));
    	memcpy(paquete,buffer,size_header);
    	paquete->datos = memoria(sizeBuffer);
    	memcpy(paquete->datos, buffer + size_header, sizeBuffer);
    	return paquete;
}

t_paquete* desacoplador(char* buffer)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	t_paquete* paquete;
  	paquete= memoria(sizeof(t_paquete));
  	memcpy(paquete,buffer,size_header);
  	paquete->datos= memoria(paquete->tamanio);
  	memcpy(paquete->datos, buffer + size_header, paquete->tamanio);
  	return paquete;
  }

int crearSocketServidor(char *puerto) {
	int BACKLOOG = 5;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	int yes =1;
	setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOOG);
	return listenningSocket;
}

int IniciarSocketServidor(int puertoServer)
{
	struct sockaddr_in socketInfo;
		int socketEscucha;
		int optval = 1;

		// Crear un socket
		socketEscucha = socket (AF_INET, SOCK_STREAM, 0);
		if (socketEscucha == -1)
		 	return -1;

		setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval,
				sizeof(optval));
		socketInfo.sin_family = AF_INET;
		socketInfo.sin_port = htons(puertoServer);
		socketInfo.sin_addr.s_addr = INADDR_ANY;
		if (bind (socketEscucha,(struct sockaddr *)&socketInfo,sizeof (socketInfo)) != 0)
		{
			close (socketEscucha);
			return -1;
		}

		/*
		* Se avisa al sistema que comience a atender llamadas de clientes
		*/
		if (listen (socketEscucha, 10) == -1)
		{
			close (socketEscucha);
			return -1;
		}
		/*
		* Se devuelve el descriptor del socket servidor
		*/
		return socketEscucha;
}

int AceptarConexionCliente(int socketServer) {
	socklen_t longitudCliente;//esta variable tiene inicialmente el tamaño de la estructura cliente que se le pase
	struct sockaddr cliente;
	int socketNuevaConexion;//esta variable va a tener la descripcion del nuevo socket que estaria creando
	longitudCliente = sizeof(cliente);
	socketNuevaConexion = accept (socketServer, &cliente, &longitudCliente);//acepto la conexion del cliente
	if (socketNuevaConexion < 0){
		return -1;
	}
	return socketNuevaConexion;

}

void recibirQueSos(int newfd){
	char* buforecibidox;
	char* buforecibido;
	osada_file* archivo;
	void* enviar;
	t_paquete* paqueteSend = malloc(sizeof(t_paquete));
	t_paquete* paqueteRead;
	char buffer[MAX_BUFFERSIZE];
	int sizebytes;
	int indice;
	if((sizebytes = recv(newfd, &buffer, MAX_BUFFERSIZE - 1,0)) <= 0)
	{
		puts("ERROR RECIBIR");
		//log_error(logDelPersonaje, "Error al recibir paquete del cliente \n");
		exit(1);
	}
	printf("El cliente me envía un paquete \n");
	t_paquete* paqueterecv = desacoplador(buffer);

	printf("el codigo es %d \n", paqueterecv->codigo);
	printf("el tamaño es %d \n", paqueterecv->tamanio);

		switch(paqueterecv->codigo){
		case 1:
			archivo = obtenerArchivo(paqueterecv->datos);
			if(archivo == NULL || archivo->state == 0) {
				paqueteSend = empaquetar(100,"error",6);
				enviar = acoplador(paqueteSend);
			}
			else {
				paqueteSend = empaquetar(1,archivo,sizeof(osada_file));
				enviar = acoplador(paqueteSend);
			}
			break;
			case 2:
			indice= obtenerIndice(paqueterecv->datos);
			t_list* listaDeHijos= listaDeHijosDelArchivo(indice);
			char* bufo= memoria(17*list_size(listaDeHijos));
			int i;

			for(i=0;i<list_size(listaDeHijos);i++){
				osada_file* archivoHijo= list_get(listaDeHijos,i);
				strcat(bufo,archivoHijo->fname);
				strcat(bufo, "/");
			}

			if(list_size(listaDeHijos) != NULL){
				paqueteSend= empaquetar(2,bufo,strlen(bufo)+1);
				enviar= acoplador(paqueteSend);
			} else {
				paqueteSend= empaquetar(100,"error",6);
				enviar= acoplador(paqueteSend);
			}

			break;
			case 3:
				crear_archivo(paqueterecv->datos,2);
				break;
			case 4:
				crear_archivo(paqueterecv->datos,1);
				break;
			case 5:
				archivo = obtenerArchivo(paqueterecv->datos);
				if(archivo == NULL || archivo->state == 0) {
					paqueteSend = empaquetar(100,"error",6);
					enviar = acoplador(paqueteSend);
				}
				else {
					paqueteSend = empaquetar(1,archivo,sizeof(osada_file));
					enviar = acoplador(paqueteSend);
				}
				break;
			case 6:
				paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
				archivo = obtenerArchivo(paqueteRead->datos);
				char* buf = malloc(minimoEntre(archivo->file_size, paqueteRead->tamanio));
				int size = leer_archivo(paqueteRead->datos,0,archivo->file_size,buf);
				paqueteSend = empaquetar(6,buf,size);
				enviar = acoplador(paqueteSend);
				break;
			case 7:
				paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
				char* bufpath = malloc(paqueterecv->tamanio - size_header - paqueteRead->tamanio -1);
				char* bufbuf = malloc(paqueterecv->tamanio);
				memcpy(bufpath, paqueteRead->datos, paqueterecv->tamanio -size_header - paqueteRead->tamanio -1);
				memcpy(bufbuf,paqueteRead->datos + strlen(bufpath), paqueteRead->tamanio);
				int tam = escribir_archivo(bufpath,paqueteRead->codigo,paqueteRead->tamanio,bufbuf);
				paqueteSend = empaquetar(7,bufbuf,tam);
				enviar = acoplador(paqueteSend);
				break;
			case 8:
				archivo = obtenerArchivo(paqueterecv->datos);
				if(archivo->state == 1) {
					borrar_archivo(paqueterecv->datos);
				}
				else if(archivo->state == 2) {
					borrar_directorio_vacio(paqueterecv->datos);
				}
				if(archivo == NULL || archivo->state == 0) {
					paqueteSend = empaquetar(100,"error",6);
					enviar = acoplador(paqueteSend);
				}
				break;
			case 9:
				break;
			case 10:
				paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
				indice = obtenerArchivo(paqueterecv->datos);
				if(indice == NULL) {
					paqueteSend = empaquetar(100,"error",6);
					enviar = acoplador(paqueteSend);
				} else {
					truncar_archivo(archivo,(uint32_t)size);
				}
				break;
			case 11:
				buforecibido= malloc(paqueterecv->tamanio);
				buforecibido= paqueterecv->datos;
				char** bufonuevo= string_split(buforecibido,"%");
				renombrar_archivo(bufonuevo[0],bufonuevo[1]);
				break;
			case 12:
				buforecibidox= malloc(paqueterecv->tamanio);
				buforecibidox= paqueterecv->datos;
				char** bufonuevox= string_split(buforecibidox,"%");
				copiar_archivo(bufonuevox[0],bufonuevox[1]);
				break;
			}

		if(send(newfd,enviar,paqueteSend->tamanio + size_header ,0)<0) {
					puts("ERROR ENVIO");
					exit(1);
		}
}

int main(void) {

	int newfd;
	int socketServidor = crearSocketServidor("9999");
	IniciarSocketServidor(atoi("9999"));
	reconocerOSADA("/home/utnso/disco.bin");
	puts("conecok");
	while (1) {
		newfd = AceptarConexionCliente(socketServidor);
		recibirQueSos(newfd);
	}
	return 0;
}




