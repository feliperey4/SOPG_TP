/*
 * svr_clientes.c
 *
 *  Created on: 17/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "svr_clientes.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
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
#include "svr_sockets.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/* Semaforo de hilos para controlar el acceso a la lista de clientes activos. */
static pthread_mutex_t mutexListaClienteA = PTHREAD_MUTEX_INITIALIZER;

/* Semaforo de hilos para controlar el acceso al ultimo cliente conectado. */
static pthread_mutex_t mutexUltimoCliente = PTHREAD_MUTEX_INITIALIZER;

/* Puntero a la estrucura del ultimo cliente conectado. */
static sClienteA * ultimoClienteAct=NULL;

/*==================[internal functions declaration]=========================*/

/*
 * Retorna el tiempo actual del sistema.
 * @Param *prt Puntero de estructura de tiempo.
 * @Retorna int Entero que indica si fue correcta la ejecucion. -1 si hay error.
 */
static int getTiempoActual(void *prt);

/*
 * Hilo para gestionar cada cliente independientemente.
 * @Param *prt Puntero de estructura de entrada.
 * @Retorna void* Puntero de respuesta.
 */
static void *gestionarCliente(void *prt);

/*
 * Funcion para generar un archivo en el sistema, con la informacion de todos los
 * clientes activos.
 * @Param *archivo Nombre y formato del archivo de salida.
 */
static void generarAchivo(char * archivo);

/*
 * Funcion para recibir datos de un socket.
 * @Param *set Lista de FD.
 * @Param fd Numero del file descriptor.
 * @Param *prt puntero de la estructura del mensaje recibido.
 * @Param tam Tamaño de byte recibidos.
 * @Param timeOut Segundos de espera por un datos nuevo.
 * @Retorna int -1 si existe algun error. 0 si no hay datos nuevos. 1 si hay datos nuevos.
 */
static int recibir(fd_set *set,int fd,void *prt,int tam,int timeOut);

/*
 * Funcion para enviar datos por socket.
 * @Param fd File descriptor de socket.
 * @Param *prt Puntero de la estructura del mensaje ha enviar.
 * @Param tam Tamaño de byte del mensaje.
 * @Retorna -1 si hay error y 0 si fue enviado correctamente.
 */
static int enviar(int fd,void *prt,int tam);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void generarAchivo(char * archivo){
	sListaClienteA *listaClientesA=NULL;
	listaClientesA=Shared_getListaClienteA(SHR_OBTENER);

	char outstr[200];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		printf("[SVR-CLIENTE] - Error localtime.\n");
		return;
	}

	if (strftime(outstr, sizeof(outstr), archivo, tmp) == 0) {
		printf("[SVR-CLIENTE] - Error strftime.\n");
		return;
	}

	printf("[SVR-CLIENTE] - Nombre archivo: %s \n",outstr);

	FILE *doc;
	doc = fopen(outstr,"w");

	int i;
	sClienteA *clienteA=NULL;
	pthread_mutex_lock( &mutexListaClienteA );
	for(i=0;i<listaClientesA->tamano;i++){
		clienteA=(i==0)?listaClientesA->inicio:clienteA->siguiente;
		fprintf(doc,"[%d] -ID: %d -DATOS: %d  -FECHA: %s \n",i,clienteA->id,clienteA->datos,clienteA->fecha);
	}
	pthread_mutex_unlock( &mutexListaClienteA );
	fclose(doc);
}

static int recibir(fd_set *set,int fd,void *prt,int tam,int timeOut){
	// Retorna 1 si el dato es correcto, 0 si no recibio nada y -1 si hay error.
	int sal=0;
	int numByte;
	struct timeval tv;
	tv.tv_sec = timeOut;
    tv.tv_usec = 50;

	FD_ZERO (set);
	FD_SET (fd, set);
	if(select(fd + 1, set, NULL, NULL, &tv)>0 && FD_ISSET (fd, set)){
		sal=((numByte=read(fd,prt,tam)) == -1)?-1:1;
	}
	return sal;
}

static int enviar(int fd,void *prt,int tam){
	if (write (fd, prt , tam) == -1){
	  printf("[SVR-CLIENTE] - Error escribiendo mensaje al cliente.\n");
	  return -1;
	}
	return 0;
}

static int getTiempoActual(void *prt){
	if (clock_gettime(CLOCK_REALTIME, prt) == -1) {
		printf("[SVR-CLIENTE] - Error obteniendo el tiempo keep alive.\n");
		return 1;
	}
	return 0;
}

static void *gestionarCliente(void *prt){
	sClienteA *cliente;
	cliente=(sClienteA *) prt;
	cliente->estado=HILO_INICIADO;

	// Shared memory.
	int *detener=NULL;
	sListaClienteA *listaClientesA=NULL;
	detener=Shared_getDetener(SHR_OBTENER);
	listaClientesA=Shared_getListaClienteA(SHR_OBTENER);

	// Variables.
	sInfoCtl infoCtl;
	sInfoDatos infoDatos;
	int isApagar=0;
	int enviarCerrar=1;
	int enviarBroadCastOtros=0;
	fd_set set;
	int estadoRx=0;

	//Tiempo de Keep Alive
	struct timespec timeKeepAlive;
	struct timespec timeActual;
	isApagar=getTiempoActual(&timeKeepAlive);

	printf("[SVR-CLIENTE] - Iniciado cliente: %d\n",cliente->id);

	while(*detener!=1 && isApagar==0 && enviarCerrar==1){
		// Verifica si hay mensaje nuevo.
		if((estadoRx=recibir(&set,cliente->ctlFd,&infoCtl,sizeof(infoCtl),0))==1){
			 // Verifica que el mensaje sea del cliente.
			if(infoCtl.id==cliente->id){
				// 1.0 KEEPALIVE
				//printf("[SVR-CLIENTE] - MSG recibido: %d\n",infoCtl.tipoMsg);
				if(infoCtl.tipoMsg==CMD_KEEP_ALIVE){
					//printf("[SVR-CLIENTE] - KEEPALIVE recibido.\n");
					infoCtl.tipoMsg=CMD_KEEP_ALIVE_OK;
					isApagar = enviar(cliente->ctlFd, &infoCtl , sizeof (infoCtl));
					isApagar=getTiempoActual(&timeKeepAlive);
					//printf("[SVR-CLIENTE] - KEEPALIVE respondido.\n");
				}// 2.0 Recibe datos del Cliente	
				else if(infoCtl.tipoMsg==CMD_RECIBIR_DATO){
					infoCtl.tipoMsg=CMD_RECIBIR_DATO_OK;
					isApagar = enviar(cliente->ctlFd, &infoCtl , sizeof (infoCtl));

					// 2.1 Recibe el dato de la informacion.
					if((estadoRx=recibir(&set,cliente->datosFd,&infoDatos,sizeof(infoDatos),0))==1){
						if(infoDatos.id==cliente->id && infoDatos.tipoMsg==CMD_RECIBIR_DATO){
							//pthread_mutex_lock(&mutexListaClienteA);
							cliente->datos=infoDatos.datos;
							strcpy(cliente->fecha,infoDatos.fecha);
							pthread_mutex_lock(&mutexUltimoCliente);
							ultimoClienteAct=cliente;
							pthread_mutex_unlock(&mutexUltimoCliente);
							//pthread_mutex_unlock(&mutexListaClienteA);
							enviarBroadCastOtros=1;
							//printf("[SVR-CLIENTE] - Dato recibido: %d ,\n",infoDatos.datos);
						}	
					}
					else if(estadoRx==-1){
						printf("[SVR-CLIENTE] - Error recibiendo mensaje, se apaga el cliente %d\n",cliente->id);
					  	isApagar=1;	
					}			
				}// 3.0 Peticion de cerrar de conecion del cliente añ SVR.
				else if(infoCtl.tipoMsg==CMD_CERRAR_CTE_SVR){
					infoCtl.tipoMsg=CMD_CERRAR_CTE_SVR_OK;
					isApagar = enviar(cliente->ctlFd, &infoCtl , sizeof (infoCtl));
					isApagar=1;
					enviarCerrar=0;
				}
			}
		}
		else if(estadoRx==-1){
			printf("[SVR-CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",cliente->id);
		  	isApagar=1;	
		}

		// 4.0 Verifica tiempo de Keep Alive
		isApagar=getTiempoActual(&timeActual);
		if(timeActual.tv_sec>(timeKeepAlive.tv_sec+TIEMPO_KEEP_ALIVE)){
			printf("[SVR-CLIENTE] - KEEP ALIVE vencido, se detiene el cliente 	%d\n",cliente->id);
			isApagar=1;
		}

		// 5.0 Activa el broadcast de todos los clientes.
		if(enviarBroadCastOtros==1){
			int i;
			sClienteA *clienteA=NULL;
			pthread_mutex_lock( &mutexListaClienteA );
			for(i=0;i<listaClientesA->tamano;i++){
				clienteA=(i==0)?listaClientesA->inicio:clienteA->siguiente;
				if(cliente->id!=clienteA->id)clienteA->activarBroadCast=BROADCAST_ON;
			}
			pthread_mutex_unlock( &mutexListaClienteA );
			enviarBroadCastOtros=0;
		}
		//5.1 Envia BroadCast.
		if(cliente->activarBroadCast==BROADCAST_ON){
			infoCtl.tipoMsg=CMD_ENVIAR_BROADCAST;
			isApagar = enviar(cliente->ctlFd, &infoCtl , sizeof (infoCtl));
			if(isApagar==0){
				int i;
				sClienteA *clienteA=NULL;
				infoDatos.id=cliente->id;
				infoDatos.tipoMsg=CMD_ENVIAR_BROADCAST;
				int contUltimo=0;
				pthread_mutex_lock( &mutexListaClienteA );
				for(i=0;i<listaClientesA->tamano;i++){
					clienteA=(i==0)?listaClientesA->inicio:clienteA->siguiente;
					if(cliente->id!=clienteA->id){
						infoDatos.idInfoClte=clienteA->id;
						infoDatos.datos=clienteA->datos;
						strcpy(infoDatos.fecha,clienteA->fecha);
						infoDatos.esUltimo=(contUltimo==(listaClientesA->tamano-2))?1:0;
						contUltimo++;
						isApagar = enviar(cliente->datosFd, &infoDatos , sizeof (infoDatos));
					}
				}
				cliente->activarBroadCast=BROADCAST_OFF;
				pthread_mutex_unlock( &mutexListaClienteA );
			}
		}
	}

	// 6.0 Envia informacion de apagar cliente.
	if(enviarCerrar==1){
		infoCtl.tipoMsg=CMD_CERRAR_SVR_CTE;
		isApagar = enviar(cliente->ctlFd, &infoCtl , sizeof (infoCtl));
	}

	//Cierra todas las conexiones.
	if(cliente->ctlFd!=NULL){
		close(cliente->ctlFd);
		FD_CLR(cliente->ctlFd, &set);
		cliente->ctlFd=NULL;
	}
	if(cliente->datosFd!=NULL){
		close(cliente->datosFd);
		FD_CLR(cliente->datosFd, &set);
		cliente->datosFd=NULL;
	}
	cliente->estado=HILO_DETENIDO;
	printf("[SVR-CLIENTE] - Detenido cliente: %d\n",cliente->id);
}

/*==================[external functions definition]==========================*/

void lanzadorClientes(){
	//Shared globales.
	int *detener=NULL;
	eCmdTerminal *cmd;
	int *idImpr;
	sPeticionClienteNuevo *clienteLanzar=NULL;
	sListaClienteA *listaClientesA=NULL;

	detener=Shared_getDetener(SHR_OBTENER);
	idImpr=Shared_getIdImprimir(SHR_OBTENER);
	cmd=Shared_getCmdTerminal(SHR_OBTENER);
	clienteLanzar=Shared_getClienteP(SHR_OBTENER);
	listaClientesA=Shared_getListaClienteA(SHR_OBTENER);

	//Semaforos globales.
	int semidCP=SEM_get(SEM_getKeyListaClienteP());
	int semCmd=SEM_get(SEM_getKeyCmdTerminal());

	while(*detener!=1){
		//Verifica si hay cliente nuevo para lanzar.
		if(clienteLanzar->isLanzado==1){
			SEM_lock(semidCP);
			pthread_mutex_lock( &mutexListaClienteA );
			sClienteA *clienteA;
			clienteA=Lista_CA_Put(listaClientesA,
					clienteLanzar->id,
					clienteLanzar->puertoCtl,
					clienteLanzar->puertoDatos,
					-1,"  ");

			//Lanza el hilo para gestinar ese cliente.
			pthread_create( &(clienteA->hilo), NULL, gestionarCliente, (void*) clienteA);

			printf("+++++++++ Lanzando Cliente +++++++++\n");
			printf("- Id: %d , - CTL: %d , - DATOS: %d \n",
					clienteLanzar->id,clienteLanzar->puertoCtl,
					clienteLanzar->puertoDatos);
			printf("++++++++++++++++++++++++++++++++++++\n");
			pthread_mutex_unlock( &mutexListaClienteA );
			clienteLanzar->id=0;
			clienteLanzar->puertoCtl=0;
			clienteLanzar->puertoDatos=0;
			clienteLanzar->isLanzado=0;
			SEM_unlock(semidCP);
		}
		else{
			SEM_lock(semCmd);
			sClienteA *clienteImp;
			//printf("[SVR-CLIENTE] - CMD: %d \n",*cmd);
			switch(*cmd){
			case IMPRIMIR_POR_ID:
				clienteImp=Lista_CA_GetById(listaClientesA,*idImpr);
				if(clienteImp==NULL){
					printf("[SVR-CLIENTE] - El cliente '%d' no existe. \n",*idImpr);
				}
				else{
					printf("[SVR-CLIENTE] - ID: %d , - Dato: %d , - Fecha: %s \n",clienteImp->id,clienteImp->datos,clienteImp->fecha);
				}
				*cmd=NADA;
				break;
			case IMPRIMIR_ULTIMO_CLIENTE:
				if(ultimoClienteAct==NULL || listaClientesA->tamano==0){
					printf("[SVR-CLIENTE] - No hay clientes conectados. \n");
				}else{
					printf("[SVR-CLIENTE] - ID: %d , - Dato: %d , - Fecha: %s \n",ultimoClienteAct->id,ultimoClienteAct->datos,ultimoClienteAct->fecha);
				}
				*cmd=NADA;
				break;
			case GUARDAR_TODO:
				printf("[SVR-CLIENTE] - Guardando todo. \n");
				generarAchivo(ARCHIVO_CON);
				*cmd=NADA;
				break;
			}
			SEM_unlock(semCmd);

			// Borra los hilos que se detuvieron por si mismo.
			int i;
			sClienteA *clienteA=NULL;
			int idBorrar=-1;
			for(i=0;i<listaClientesA->tamano;i++){
				clienteA=(i==0)?listaClientesA->inicio:clienteA->siguiente;
				if(clienteA->estado==HILO_DETENIDO){
					idBorrar=clienteA->id;
					break;
				}
			}
			if(*detener==0 && idBorrar!=-1){
				pthread_mutex_lock( &mutexListaClienteA );
				printf("[SVR-CLIENTE] - auto-detenido id: %d\n",idBorrar);
				pthread_join( clienteA->hilo, NULL);
				Lista_CA_BorrarById(listaClientesA,idBorrar);
				pthread_mutex_unlock( &mutexListaClienteA );
			}
			sleep(1);
		}
	}

	printf("[SVR-CLIENTE] - Numero de clientes activos: %d.\n",listaClientesA->tamano);
	int i;
	sClienteA *clienteA=NULL;
	for(i=0;i<listaClientesA->tamano;i++){
		pthread_mutex_lock( &mutexListaClienteA );
		clienteA=(i==0)?listaClientesA->inicio:clienteA->siguiente;
		pthread_join( clienteA->hilo, NULL);
		pthread_mutex_unlock( &mutexListaClienteA );
	}

	printf("[SVR-CLIENTE] - Borrando todos los clientes.\n");
	generarAchivo(ARCHIVO_BCK);
	Lista_CA_BorrarTodo(listaClientesA);
	pthread_mutex_destroy(&mutexListaClienteA);
	pthread_mutex_destroy(&mutexUltimoCliente);
}

/*==================[end of file]============================================*/
