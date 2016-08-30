#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#define PATH_CONFIG "../Mapas/Ciudad Paleta/metadata"
#define PORT "10000"   // port we're listening on

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
typedef struct
{
	char  *ipEscucha;
	int puertoEscucha;
	int quantum;
	int retardo;
	char  *algoritmo;
	int batalla;
	int tiempoChequeoDeadlock;
	} mapa_datos;
int leerConfiguracionCpu(mapa_datos *datos )
{
	t_config* config = config_create(PATH_CONFIG);

	if ( config_has_property(config, "IP") && config_has_property(config, "Puerto") && config_has_property(config, "algoritmo") && config_has_property(config, "quantum") && config_has_property(config, "retardo") && config_has_property(config, "Batalla") && config_has_property(config, "TiempoChequeoDeadlock"))
	{
		datos->ipEscucha = config_get_string_value(config, "IP");
		datos->puertoEscucha  = config_get_int_value(config, "Puerto");
		datos->algoritmo  = config_get_string_value(config, "algoritmo");
		datos->quantum = config_get_int_value(config, "quantum");
		datos->retardo = config_get_int_value(config, "retardo");
		datos->batalla = config_get_int_value(config, "batalla");
		datos->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		return 1;
	}
	else
    {
		return -1;
    }

}
int main(void)
{

	/* Configuración de LOG */
	#define LOG_FILE "proceso_Mapa.log"


	/* Configuración de LOG */
	#define PROGRAM_NAME "MAPA"
	#define PROGRAM_DESCRIPTION "Proceso MAPA"
	#define IS_ACTIVE_CONSOLE true
	#define T_LOG_LEVEL LOG_LEVEL_INFO

	/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        log_info(logger, "Fallo la lectura de datos locales para el socket");
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        log_info(logger, "fallo el bind con el socket listener");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        //perror("accept");
                        log_info(logger, "Error en el accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            log_info(logger, "Error al recibir datos");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                    	log_info(logger, "Recibiendo datos de un cliente");
                    	switch (buf[0]){

                    	case '1': printf("sos un entrenador");
                    	break;
                    	case '2': printf("sos otra cosa");
                    	break;
                    	default: printf ("no se que sos");
                    	break;
                    	}
                    	printf("\n %s", buf);
                    	/*char *message = "recibido\0";
                    	if ((send(i, message,sizeof(message)+1, 0))<=0)
							{
								printf("error mandando");
							}
                    	else{
                    		printf("ok mandado");
                    	}




                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {


                               // except the listener and ourselves
                                if (j != listener (&& j != i)) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                        printf(buf);
                                    }
                                        else
                                        {
                                        	printf(buf);
                                        }

                                }
                            }
                        }*/
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
