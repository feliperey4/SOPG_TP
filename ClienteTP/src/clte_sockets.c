/*
 * clte_sockets.c
 *
 *  Created on: 21/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "clte_sockets.h"
#include "clte_listas.h"
#include "clte_sem.h"
#include "clte_shared.h"

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
#include <time.h>
#include <pthread.h>

/*==================[macros and definitions]=================================*/

#define FD_CTL 1	/* Indicador de file descriptor de control. */
#define FD_DATOS 2	/* Indicador de file descriptor de datos. */

/*==================[internal data declaration]==============================*/

/* Semaforo de hilos para controlar el acceso a la lista de clientes. */
static pthread_mutex_t mutexListaCliente = PTHREAD_MUTEX_INITIALIZER;

/*==================[internal functions declaration]=========================*/

/*
 * Funcion para conectar un socket.
 * @Param *clienteP Cliente de nueva peticion.
 * @Param tipoFd Tipo del file descriptor que se desea conectar.
 * @Retorna int File descriptor conectado.
 */
static int conectarFd(sPeticionClienteNuevo * clienteP, int tipoFd);

/*
 * Funcion para generar el primer paso para realizar un nueva conexion.
 * @Param *clienteP Puntero de cliente pendiente.
 */
static int nuevoCliente(sPeticionClienteNuevo * clienteP);

/*
 * Funcion para recibir datos de un socket.
 * @Param *set Lista de FD.
 * @Param fd Numero del file descriptor.
 * @Param *prt puntero de la estructura del mensaje recibido.
 * @Param tam Tamaño de byte recibidos.
 * @Param timeOut Segundos de espera por un datos nuevo.
 * @Retorna int -1 si existe algun error. 0 si no hay datos nuevos. 1 si hay datos nuevos.
 */
static int recibirPaquete(fd_set *set,int fd,void *prt,int tam,int timeOut);

/*
 * Funcion que realiza toda la gestio de conexion con el servidor.
 * @Param *ClienteSkts Puntero que contiene los datos del cliente y el file descriptor.
 */
static void gestionarCliente(sClienteSkts *clienteSkts);

/*
 * Funcion para enviar datos por socket.
 * @Param fd File descriptor de socket.
 * @Param *prt Puntero de la estructura del mensaje ha enviar.
 * @Param tam Tamaño de byte del mensaje.
 * @Retorna -1 si hay error y 0 si fue enviado correctamente.
 */
static int enviar(int fd,void *prt,int tam);

/*
 * Retorna el tiempo actual del sistema.
 * @Param *prt Puntero de estructura de tiempo.
 * @Retorna int Entero que indica si fue correcta la ejecucion. -1 si hay error.
 */
static int getTiempoActual(void *prt);

/*
 * Funcion para generar una fecha apartir de un formato determinado.
 * @Param *fecha Puntero donde se almacena la fecha.
 */
static void generarDate(char * fecha);

/*
 * Funcion para generar un archivo en el sistema, con la informacion de todos los
 * clientes activos.
 * @Param *archivo Nombre y formato del archivo de salida.
 */
static void generarAchivo(char * archivo);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void generarAchivo(char * archivo){
	sListaCliente *listaClientes=NULL;
	listaClientes=Shared_getListaCliente(SHR_OBTENER);

	char outstr[50];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		printf("[CLIENTE] - Error localtime.\n");
		return;
	}

	if (strftime(outstr, sizeof(outstr), archivo, tmp) == 0) {
		printf("[CLIENTE] - Error strftime.\n");
		return;
	}

	printf("[CLIENTE] - Nombre archivo: %s \n",outstr);

	FILE *doc;
	doc = fopen(outstr,"w");
	int i;
	sCliente *cliente=NULL;
	for(i=0;i<listaClientes->tamano;i++){
		cliente=(i==0)?listaClientes->inicio:cliente->siguiente;
		fprintf(doc,"[%d] -ID: %d -DATOS: %d  -FECHA: %s \n",i,cliente->id,cliente->datos,cliente->fecha);
	}
	fclose(doc);
}

static void generarDate(char * fecha){
	time_t t;
	struct tm *tmp;
	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		printf("[CLIENTE] - Error localtime.\n");
		return;
	}
	if (strftime(fecha, 20*sizeof(char),FMT_DATE, tmp) == 0) {
		printf("[CLIENTE] - Error strftime.\n");
		return;
	}
}

static int getTiempoActual(void *prt){
	if (clock_gettime(CLOCK_REALTIME, prt) == -1) {
		printf("[CLIENTE] - Error obteniendo el tiempo.\n");
		return 1;
	}
	return 0;
}

static int enviar(int fd,void *prt,int tam){
	if (write (fd, prt , tam) == -1){
	  printf("[CLIENTE] - Error escribiendo mensaje al servidor.\n");
	  return -1;
	}
	return 0;
}

static int conectarFd(sPeticionClienteNuevo * clienteP, int tipoFd) {
	int sockfd;
	struct sockaddr_in their_addr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("[CLIENTE] - Error en creacion de socket.\n");
		return -1;
	}
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons((tipoFd==FD_CTL)?clienteP->puertoCtl:clienteP->puertoDatos);
	their_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(their_addr.sin_zero), 8);
	if (connect(sockfd, (struct sockaddr *) &their_addr,sizeof(struct sockaddr)) == -1) {
		printf("[CLIENTE] - Error tratando de conectar al server.\n");
		return -1;
	}

	if (write(sockfd, clienteP, sizeof(sPeticionClienteNuevo)) == -1) {
		printf("[CLIENTE] - Error de lectura en el socket.\n");
		return -1;
	}
	return sockfd;
}

static int nuevoCliente(sPeticionClienteNuevo * clienteP) {
	int sockfd;
	struct sockaddr_in their_addr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("[CLIENTE] - Error en creacion de socket.\n");
		return -1;
	}
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PUERTO_SVR);
	their_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(their_addr.sin_zero), 8);
	if (connect(sockfd, (struct sockaddr *) &their_addr,
			sizeof(struct sockaddr)) == -1) {
		printf("[CLIENTE] - Error tratando de conectar al server.\n");
		return -1;
	}
	if (read(sockfd, clienteP, sizeof(sPeticionClienteNuevo))
			== -1) {
		printf("[CLIENTE] - Error de lectura en el socket.\n");
		return -1;
	}
	printf("[CLIENTE] - ID: %d , - Puerto CTL: %d, - Puerto DATOS: %d\n", clienteP->id,
			clienteP->puertoCtl, clienteP->puertoDatos);
	close(sockfd);
	return 0;
}

static int recibirPaquete(fd_set *set,int fd,void *prt,int tam,int timeOut){
	// Retorna 1 si el dato es correcto, 0 si no recibio nada y -1 si hay error.
	int sal=0;
	int numByte;
	struct timeval tv;  
	tv.tv_sec = timeOut;            
    tv.tv_usec = 200; 

	FD_ZERO (set);
	FD_SET (fd, set);
	if(select(fd + 1, set, NULL, NULL, &tv)>0 && FD_ISSET (fd, set)){
		sal=((numByte=read(fd,prt,tam)) == -1)?-1:1;
	}
	return sal;
}

static void gestionarCliente(sClienteSkts *clienteSkts){
	int *detener=NULL;
	eCmdTerminal *cmd;
	sListaCliente *listaClientes=NULL;

	detener=Shared_getDetener(SHR_OBTENER);
	cmd=Shared_getCmdTerminal(SHR_OBTENER);
	listaClientes=Shared_getListaCliente(SHR_OBTENER);

	int semCmd=SEM_get(SEM_getKeyCmdTerminal());

	Lista_C_Put(listaClientes,clienteSkts->id,0," ");

	sInfoCtl infoCtl;
	sInfoDatos infoDatos;
	int isApagar=0;
	int estadoRx=0;
	int peticionApagado=0;

	//Tiempo de Keep Alive
	struct timespec timeKeepAlive;
	struct timespec timeActual;
	struct timespec timeGenDato;

	isApagar=getTiempoActual(&timeKeepAlive);
	isApagar=(isApagar==0)?getTiempoActual(&timeGenDato):-1;

	// Configura FD no bloqueantes.
	int ctlFd=clienteSkts->ctlFd;
	int datosFd=clienteSkts->datosFd;
	fd_set set;

	while(*detener!=1 && isApagar==0){
		//Tiempo
		isApagar=(isApagar==0)?getTiempoActual(&timeActual):-1;
		
		// Recive datos
		if((estadoRx=recibirPaquete(&set,ctlFd,&infoCtl,sizeof(infoCtl),0))==1){
			if(infoCtl.id==clienteSkts->id){
				//printf("[CLIENTE] - Dato recibido code: %d.\n",infoCtl.tipoMsg);
				//Peticion de apagado.
				if(infoCtl.tipoMsg==CMD_CERRAR_SVR_CTE){
					printf("[CLIENTE]-Peticion de apagado recibido.\n");
					isApagar=1;
					*detener=1;
		  			peticionApagado=1;
				}// Recibe broadcast.
				else if(infoCtl.tipoMsg==CMD_ENVIAR_BROADCAST){
					printf("[CLIENTE] - Peticion de broadcast.\n");
					int finBroadcast=0;
					pthread_mutex_lock( &mutexListaCliente );
					Lista_C_BorrarTodo(listaClientes);
					do{
						if((estadoRx=recibirPaquete(&set,datosFd,&infoDatos,sizeof(infoDatos),TIEMPO_ESPERA))==1){
							if(infoDatos.id==clienteSkts->id){
								Lista_C_Put(listaClientes,infoDatos.idInfoClte,infoDatos.datos,infoDatos.fecha);
								finBroadcast=infoDatos.esUltimo;
							}
						}
						else if(estadoRx==-1){
							printf("[CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",clienteSkts->id);
							isApagar=1;
						}
						else{
							printf("[CLIENTE] - Se vencio tiempo de espera de BROADCAST.\n");
							finBroadcast=-1;
						}
					}while(*detener!=1 && isApagar==0 && finBroadcast==0);
					pthread_mutex_unlock( &mutexListaCliente );
				}
			}	
		}
		else if(estadoRx==-1){
			printf("[CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",clienteSkts->id);
		  	isApagar=1;	
		}

		// Envia KEEP ALIVE
		if(timeActual.tv_sec>(timeKeepAlive.tv_sec+TIEMPO_KEEP_ALIVE)){
			infoCtl.id=clienteSkts->id;
			infoCtl.tipoMsg=CMD_KEEP_ALIVE;
			//printf("[CLIENTE] - Enviando keep alive.\n");
			isApagar=enviar(ctlFd, &infoCtl, sizeof(sInfoCtl));

			if((estadoRx=recibirPaquete(&set,ctlFd,&infoCtl,sizeof(infoCtl),TIEMPO_ESPERA))==1){
				if(infoCtl.id==clienteSkts->id && infoCtl.tipoMsg==CMD_KEEP_ALIVE_OK){
					//printf("[CLIENTE] - Actualizando KEEP ALIVE.\n");
					isApagar=(isApagar==0)?getTiempoActual(&timeKeepAlive):-1;
				}	
			}
			else if(estadoRx==-1){
				printf("[CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",clienteSkts->id);
			  	isApagar=1;	
			}
			else{
				printf("[CLIENTE] - SVR no responde KEEP_ALIVE.\n");
				isApagar=1;
			}
		}

		// Envia Dato nuevo
		if(timeActual.tv_sec>(timeGenDato.tv_sec+TIEMPO_GENERAR_DATO)){
			//printf("[CLIENTE] - Enviando dato actual.\n");
			infoCtl.id=clienteSkts->id;
			infoCtl.tipoMsg=CMD_RECIBIR_DATO;
			isApagar=enviar(ctlFd, &infoCtl, sizeof(sInfoCtl));

			if((estadoRx=recibirPaquete(&set,ctlFd,&infoCtl,sizeof(infoCtl),TIEMPO_ESPERA))==1){
				if(infoCtl.id==clienteSkts->id && infoCtl.tipoMsg==CMD_RECIBIR_DATO_OK){
					infoDatos.id=clienteSkts->id;
					infoDatos.idInfoClte=clienteSkts->id;
					infoDatos.tipoMsg=CMD_RECIBIR_DATO;
					infoDatos.esUltimo=1;
					infoDatos.datos=(int) rand()%MAX_RAND;
					generarDate(infoDatos.fecha);

					pthread_mutex_lock( &mutexListaCliente );
					Lista_C_Put(listaClientes,infoDatos.id,infoDatos.datos,infoDatos.fecha);
					pthread_mutex_unlock( &mutexListaCliente );

					isApagar=enviar(datosFd, &infoDatos , sizeof (infoDatos));
					isApagar=(isApagar==0)?getTiempoActual(&timeGenDato):-1;
					//printf("[CLIENTE] - Dato enviado.\n");
				}	
			}
			else if(estadoRx==-1){
				printf("[CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",clienteSkts->id);
			  	isApagar=1;	
			}
			else{
				printf("[CLIENTE] - SVR no responde.\n");
				isApagar=1;
			}
		}

		//CMD
		SEM_lock(semCmd);
		sCliente *clienteImp;
		//printf("[SVR-CLIENTE] - CMD: %d \n",*cmd);
		switch(*cmd){
		case GUARDAR_TODO:
			printf("[CLIENTE] - Guardando todo. \n");
			generarAchivo(ARCHIVO_CON);
			*cmd=NADA;
			break;
		}
		SEM_unlock(semCmd);
	}

	// Envia Aviso de apagado.
	if(peticionApagado==0){
		printf("[CLIENTE] - Enviando apagado.\n");
		infoCtl.id=clienteSkts->id;
		infoCtl.tipoMsg=CMD_CERRAR_CTE_SVR;
		isApagar=enviar(ctlFd, &infoCtl , sizeof (infoCtl));
		if((estadoRx=recibirPaquete(&set,ctlFd,&infoCtl,sizeof(infoCtl),TIEMPO_ESPERA))==-1){
			printf("[CLIENTE] - Error recibiendo mensaje al cliente, se apaga el cliente %d\n",clienteSkts->id);
		  	isApagar=1;
		}
		else if(estadoRx==0){
			printf("[CLIENTE] - SVR no responde.\n");
			isApagar=1;
		}
	}
}

/*==================[external functions definition]==========================*/

void conectarCliente(){
	int *detener=NULL;
	sListaCliente *listaClientes=NULL;

	detener=Shared_getDetener(SHR_OBTENER);
	listaClientes=Shared_getListaCliente(SHR_OBTENER);

	sPeticionClienteNuevo clienteP;
	sClienteSkts clienteSkts;

	//1.0 Crea conexion con el servidor.
	if(nuevoCliente(&clienteP)==-1){
		printf("[CLIENTE] - Error\n");
		return;
	}
	clienteSkts.id=clienteP.id;

	//2.0 Abre el puerto de CTL.
	if((clienteSkts.ctlFd=conectarFd(&clienteP,FD_CTL))==-1){
		printf("[CLIENTE] - Error\n");
		return;
	}

	//3.0 Abre el puerto de DATOS.
	if((clienteSkts.datosFd=conectarFd(&clienteP,FD_DATOS))==-1){
		printf("[CLIENTE] - Error\n");
		return;
	}

	printf("*********************************************\n");
	printf("- ID: %d , - CTL: %d , - DATOS: %d\n",clienteSkts.id,clienteSkts.ctlFd,clienteSkts.datosFd);
	printf("*********************************************\n");

	//sleep(2);

	gestionarCliente(&clienteSkts);

	// Inicia a cerrar y a borrar todo.
	close(clienteSkts.ctlFd);
	close(clienteSkts.datosFd);

	Lista_C_BorrarTodo(listaClientes);

	//Elimina mutex
	pthread_mutex_destroy(&mutexListaCliente);
}

/*==================[end of file]============================================*/
