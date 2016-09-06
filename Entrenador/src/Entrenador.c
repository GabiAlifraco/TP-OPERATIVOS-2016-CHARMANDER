#include "Entrenador.h"
char *Objetivo;
int totalMapas=0;

int leerConfiguracionMapa(entrenador_datos *datos )
{
	char nombre[10];
	printf("%s", "Nombre del Entrenador?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[40] = "Entrenadores/";
	strcat(pathconfigMetadata, nombre);
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);

		if ( config_has_property(config, "nombre") && config_has_property(config, "simbolo")
		&& config_has_property(config, "hojaDeViaje") && config_has_property(config, "vidas"))
		{
			/*char *configuracion;
			configuracion = string_duplicate("\nConfiguración:");
			string_append(&configuracion, config_get_string_value(config, "nombre"));
			datos->nombre  = config_get_string_value(config, "nombre");

			string_append(&configuracion, "\nsimbolo: ");
			datos->simbolo  = config_get_string_value(config, "simbolo");
			string_append(&configuracion,config_get_string_value(config, "simbolo"));

			string_append(&configuracion, "\nvidas: ");
			datos->vidas = config_get_int_value(config, "vidas");
			string_append(&configuracion,config_get_int_value(config, "vidas"));

			string_append(&configuracion,"\nhojaDeViaje");
			string_append(&configuracion,(char*)config_get_string_value(config,"\nhojaDeViaje"));
			datos->hojaDeViaje = string_get_string_as_array(config_get_string_value(config, "hojaDeViaje"));
			char* palabra = string_new();
			int k=0;
			while(datos->hojaDeViaje[totalMapas] != NULL)
				{
				string_append_with_format(&palabra,"obj[%s]",datos->hojaDeViaje[totalMapas]);
				string_append_with_format(&config,"\nobj[%s]",datos->hojaDeViaje[totalMapas]);
				Objetivo = (char*)config_get_string_value(config,palabra);
				list_add(datos->ObjMapa, Objetivo);
				string_append(&config,(char*)list_get(datos->ObjMapa,k));
				totalMapas++;
				k++;
				palabra = string_new();
				}*/
			return 1;
			}
			else
		    {
			return -1;
		 }
	}


int main(){
	entrenador_datos infoEntrenador;
	/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

		if ( leerConfiguracionMapa ( &infoEntrenador ) == 1 )
			log_info(logger, "Archivo de configuracion leido correctamente");
		else
			log_error(logger,"Error la leer archivo de configuracion");

	/*
	 *  ¿Quien soy? ¿Donde estoy? ¿Existo?
	 *
	 *  Estas y otras preguntas existenciales son resueltas getaddrinfo();
	 *
	 *  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	 *
	 */
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	/*
	 * 	Ya se quien y a donde me tengo que conectar... ¿Y ahora?
	 *	Tengo que encontrar una forma por la que conectarme al server... Ya se! Un socket!
	 *
	 * 	Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.
	 *
	 */
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	//int buf;
	//int i;
	//ssize_t nbytes;

	/*
	 * 	Perfecto, ya tengo el medio para conectarme (el archivo), y ya se lo pedi al sistema.
	 * 	Ahora me conecto!
	 *
	 */
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	/*
	 *	Estoy conectado! Ya solo me queda una cosa:
	 *
	 *	Enviar datos!
	 *
	 *	Vamos a crear un paquete (en este caso solo un conjunto de caracteres) de size PACKAGESIZE, que le enviare al servidor.
	 *
	 *	Aprovechando el standard immput/output, guardamos en el paquete las cosas que ingrese el usuario en la consola.
	 *	Ademas, contamos con la verificacion de que el usuario escriba "exit" para dejar de transmitir.
	 *
	 */
	int enviar = 1;
	char message[PACKAGESIZE];

	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");
	log_info(logger, "Conectado al servidor");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(message,"exit\n"))
			{
			enviar = 0;
			log_info(logger, "El usuario decidio salir");// Chequeo que el usuario no quiera salir
			}
		if (enviar)
			{
			send(serverSocket, message, strlen(message) + 1, 0);
			log_info(logger, "Se envio el mensaje");  // Solo envio si el usuario no quiere salir.
			}
	}

	/*
	 *	Listo! Cree un medio de comunicacion con el servidor, me conecte con y le envie cosas...
	 *
	 *	...Pero me aburri. Era de esperarse, ¿No?
	 *
	 *	Asique ahora solo me queda cerrar la conexion con un close();
	 */

	close(serverSocket);
	return 0;

	/* ADIO'! */
}
