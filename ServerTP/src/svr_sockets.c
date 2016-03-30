/*
 * svr_sockets.c
 *
 *  Created on: 17/3/2016
 *      Author: felipe
 */


/*==================[inclusions]=============================================*/

#include "svr_sockets.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include "svr_listas.h"
#include "svr_shared.h"
#include "svr_sem.h"
#include "svr_clientes.h"

/*==================[macros and definitions]=================================*/

#define MAX_HILOS_LOCAL 5 /* Numero maximo de hilos creados. */

/*==================[internal data declaration]==============================*/

/* Semaforo para controlar la lista de clientes pendientes. */
static pthread_mutex_t mutexListaClienteP = PTHREAD_MUTEX_INITIALIZER;;

/*==================[internal functions declaration]=========================*/

/*
 * Funcion para escuchar conexiones nuevas en un socket, con un timeout.
 * @Param *set Puntero de estructura de lista de FD.
 * @Param fd File descriptor.
 * @Param *dir Puntero de estructura que representa configuracion de socket.
 * @Param *sin_size Puntero de tamaño de estructura de socket.
 * @Param timeOut Tiempo en segundo que se bloquea, eperando una nueva conexion al socket.
 * @Return int File descriptor de la nueva conexion del cliente al socket. Null no hay cliente nuevo.
 */
static int acceptSelect(fd_set *set,int fd,void *dir,unsigned int *sin_size,int timeOut);

/*
 * Hilo lanzador de clientes nuevos.
 * @Param *prt Puntero de estructura.
 * @Return void* Puntero de estructura.
 */
static void *lanzadorClientesHilo(void *prt);

/*
 * Hilo buscar clientes pendientes para ser lanzados.
 * @Param *prt Puntero de estructura.
 * @Return void* Puntero de estructura.
 */
static void *verificarLanzadorClientes(void *prt);

/*
 * Hilo para atender socket.
 * @Param *prt Puntero de estructura de tipo de socket.
 * @Return void* Puntero de estructura.
 */
static void *atenderSocket(void *prt);

/*==================[internal data definition]===============================*/

/* Variable que almacena la numeracion de id de los clientes nuevos. */
static int clienteId=0;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static int acceptSelect(fd_set *set,int fd,void *dir,unsigned int *sin_size,int timeOut){
	// Retorna 1 si el dato es correcto, 0 si no recibio nada y -1 si hay error.
	int sal=0;
	int newfd;
	struct timeval tv;
	tv.tv_sec = timeOut;            
    tv.tv_usec = 50; 

	FD_ZERO (set);
	FD_SET (fd, set);
	if(select(fd + 1, set, NULL, NULL, &tv)>0){
		if(FD_ISSET (fd, set)){
			sal= ((newfd = accept(fd, (struct sockaddr *)dir, sin_size))==-1)?-1:newfd;
		}
	}
	return sal;
}

static void *lanzadorClientesHilo(void *prt){
	lanzadorClientes();
	return NULL;
}

static void *verificarLanzadorClientes(void *prt){
	int *detener=NULL;
	sListaClienteP *listaClientesP=NULL;
	sPeticionClienteNuevo *clienteLanzar=NULL;

	int semidCP=SEM_get(SEM_getKeyListaClienteP());

	listaClientesP=Shared_getListaClienteP(SHR_OBTENER);
	detener=Shared_getDetener(SHR_OBTENER);
	clienteLanzar=Shared_getClienteP(SHR_OBTENER);

	while(*detener!=1){
		int i;
		if(listaClientesP->tamano>0){
			pthread_mutex_lock( &mutexListaClienteP );
			sClienteP *clienteP=NULL;
			int idBorrar=-1;
			for(i=0;i<listaClientesP->tamano;i++){
				clienteP=(i==0)?listaClientesP->inicio:clienteP->siguiente;
				if(clienteP->ctlFd!=-1 && clienteP->datosFd!=-1 && clienteLanzar->isLanzado==0){
					SEM_lock(semidCP);
					clienteLanzar->id=clienteP->id;
					clienteLanzar->puertoCtl=clienteP->ctlFd;
					clienteLanzar->puertoDatos=clienteP->datosFd;
					clienteLanzar->isLanzado=1;
					idBorrar=clienteP->id;
					SEM_unlock(semidCP);
					break;
				}
			}
			if(idBorrar!=-1){
				int numE;
				if((numE=Lista_CP_BorrarById(listaClientesP,idBorrar))<=-1){
					printf("[SVR] - Fallo borrando id: %d  code error: %d\n",idBorrar,numE);
				}
				//printf("Elem borrado: %d  - tam: %d\n",idBorrar,listaClientesP->tamano);
			}
			pthread_mutex_unlock(&mutexListaClienteP);
		}
		else{
			sleep(1);
		}
	}

	//Borra todos los clientes listo para lanzar.
	int i;
	for(i=0;i<listaClientesP->tamano;i++){
		sClienteP *clienteP=NULL;
		pthread_mutex_lock( &mutexListaClienteP );
		clienteP=(i==0)?listaClientesP->inicio:clienteP->siguiente;
		if(clienteP->ctlFd!=NULL)close(clienteP->ctlFd);
		if(clienteP->datosFd!=NULL)close(clienteP->datosFd);
		pthread_mutex_unlock( &mutexListaClienteP );
	}

	Lista_CP_BorrarTodo(listaClientesP);
	return NULL;
}

static void *atenderSocket(void *prt){
	sSocket * paramSkt;
	paramSkt=(sSocket *) prt;

	int *detener=NULL;
	sListaClienteP *listaClientesP;
	int *crearCliente;

	crearCliente=Shared_getCrearCliente(SHR_OBTENER);
	listaClientesP=Shared_getListaClienteP(SHR_OBTENER);
	detener=Shared_getDetener(SHR_OBTENER);

	sPeticionClienteNuevo clienteNuevo;
	fd_set set;
	int	sock_fd;
	int	aux;
	int newfd;
	struct sockaddr_in my_addr,their_addr;
	unsigned int sin_size;

	sin_size = sizeof(struct sockaddr_in);
	clienteId=1;

	printf("[SVR] - Inicia hilo de skts: %d\n",paramSkt->tipo);

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "[SVR] - Error en funcion socket.\n");
		return NULL;
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(paramSkt->puerto);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	int numRx;
	sPeticionClienteNuevo peticion;

	if ( (aux = bind (sock_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))) == -1){
		fprintf(stderr, "[SVR] - Error en funcion bind. Codigo de error %d\n", aux);
		return NULL;
	}

	if ((aux = listen (sock_fd, MAX_COL_PETICIONES)) == -1){
		fprintf(stderr, "[SVR] - Error en funcion listen. Codigo de error %d\n", aux);
		return NULL;
	}

	while(*detener!=1){
		if ((newfd = acceptSelect(&set,sock_fd,&their_addr, &sin_size,1)) > 0){
			switch (paramSkt->tipo) {
				case SKT_ESCUCHA_CLIENTE:
					if(*crearCliente==1){
						clienteNuevo.id=clienteId;
						clienteNuevo.puertoCtl=PUERTO_CTL;
						clienteNuevo.puertoDatos=PUERTO_DATA;
						clienteNuevo.isLanzado=0;

						pthread_mutex_lock( &mutexListaClienteP );
						Lista_CP_Put(listaClientesP,clienteId,-1,-1);
						pthread_mutex_unlock( &mutexListaClienteP );

						if (write (newfd, &clienteNuevo , sizeof (clienteNuevo)) == -1){
						  perror("[SVR] - Error escribiendo mensaje en socket");
						  pthread_mutex_lock( &mutexListaClienteP );
						  Lista_CP_BorrarById(listaClientesP,clienteId);
						  pthread_mutex_unlock( &mutexListaClienteP );
						  return NULL;
						}

						clienteId++;
						close(newfd);
					}
					break;
				case SKT_CTL:
					if((numRx=read(newfd, &peticion, sizeof(peticion))) == -1){
						printf("[SVR] - Error de lectura en el socket");
						return NULL;
					}

					pthread_mutex_lock( &mutexListaClienteP );
					sClienteP *clienteP=NULL;
					if(numRx>0 && (Lista_CP_GetById(listaClientesP,peticion.id)!=NULL)){
						clienteP=Lista_CP_GetById(listaClientesP,peticion.id);
						clienteP->ctlFd=newfd;
					}else{
						printf("[SVR] - CTRL ---- No existe el id: %d   - Bytes: %d\n",peticion.id,numRx);
						close(newfd);
					}
					pthread_mutex_unlock( &mutexListaClienteP );
					break;
				case SKT_DATOS:
					if((numRx=read(newfd, &peticion, sizeof(peticion))) == -1){
						printf("[SVR] - Error de lectura en el socket");
						return NULL;
					}
					pthread_mutex_lock( &mutexListaClienteP );
					if(numRx>0 && Lista_CP_GetById(listaClientesP,peticion.id)!=NULL){
						sClienteP *clienteP;
						clienteP=Lista_CP_GetById(listaClientesP,peticion.id);
						clienteP->datosFd=newfd;
					}else{
						printf("[SVR] - DATOS ---- No existe el id: %d   - Bytes: %d\n",peticion.id,numRx);
						close(newfd);
					}
					pthread_mutex_unlock( &mutexListaClienteP );
					break;
				default:
					break;
			}
		}
		else if(newfd==-1){
			printf("[SVR] - Error durante la escucha de sockets.\n");
		}
	}
	printf("[SVR] - Deteniendo Hilo de skt: %d \n",paramSkt->tipo);
	return NULL;
}

/*==================[external functions definition]==========================*/

void atenderSockets(){
	pthread_t thread_id[MAX_HILOS_LOCAL];
	sSocket sckt[]={{SKT_ESCUCHA_CLIENTE,PUERTO_CLIENTE},{SKT_CTL,PUERTO_CTL},{SKT_DATOS,PUERTO_DATA}};
	//Crea hilos para cada sockets
	pthread_create( &(thread_id[0]), NULL, atenderSocket, &(sckt[0]));
	pthread_create( &(thread_id[1]), NULL, atenderSocket, &(sckt[1]));
	pthread_create( &(thread_id[2]), NULL, atenderSocket, &(sckt[2]));
	pthread_create( &(thread_id[3]), NULL, verificarLanzadorClientes, NULL);
	pthread_create( &(thread_id[4]), NULL, lanzadorClientesHilo, NULL);

	int j;
	for(j=0; j < MAX_HILOS_LOCAL; j++){
		pthread_join( thread_id[j], NULL);
	}

	pthread_mutex_destroy(&mutexListaClienteP);
}

/*==================[end of file]============================================*/
