/*
 * clte_admin.c
 *
 *  Created on: 24/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "clte_admin.h"
#include "clte_shared.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/*==================[macros and definitions]=================================*/

#define MAX_HILOS_LOCAL 2	/* Cantidad de hilos maximos para crear. */

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*
 * Hilo para monitorear los recursos de cpu y memoria del servidor.
 * @Param *prt Puntero de estructura.
 * @Return void* Puntero de estructura.
 */
static void * monitorRecursos(void *prt);

/*
 * Hilo para monitorear la entrada por consola de comandos.
 * @Param *prt Puntero de estructura.
 * @Return void* Puntero de estructura.
 */
static void * monitorConsola(void *prt);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void * monitorConsola(void *prt){
	int *detener;
	eCmdTerminal *cmdTerminal;

	detener= Shared_getDetener(SHR_OBTENER);
	cmdTerminal=Shared_getCmdTerminal(SHR_OBTENER);

	int semCmd=SEM_get(SEM_getKeyCmdTerminal());

	struct timeval tv = { 1L, 50L };
	fd_set fds;
	char buf[20];
	char cmd;
	int id;
	while(*detener==0){
		FD_ZERO(&fds);
		FD_SET(0, &fds);FD_SET(0, &fds);
		if(select(1, &fds, NULL, NULL, &tv)>0){
			id=-1;
			cmd=' ';
			if ((read(0, buf, 20)) > 0) {
				sscanf(buf,"%c%d",&cmd,&id);
			}

			SEM_lock(semCmd);
			//printf("[ADMIN] - CMD: %c \n",cmd);
			switch(cmd){
			case 'I':
				*cmdTerminal=GUARDAR_TODO;
				break;
			case 'q':
				*cmdTerminal=APAGAR;
				*detener=1;
				break;
			default:
				*cmdTerminal=NADA;
				break;
			}
			SEM_unlock(semCmd);
		}
	}
}


static void * monitorRecursos(void *prt){
	int *detener;
	detener= Shared_getDetener(SHR_OBTENER);

	int memVal;
	int cpuVal;
	long double a[4], b[4];
	long double mTotal,mLibre;
	FILE *fp;

	while(*detener==0){
		fp = fopen("/proc/stat","r");
		fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
		fclose(fp);
		sleep(1);
		fp = fopen("/proc/stat","r");
		fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
		fclose(fp);

		cpuVal = (int) (((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2]))*100 / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3])));

		//Memoria
		fp = fopen("/proc/meminfo","r");
		fscanf(fp,"%*s %Lf %*s\n%*s %Lf",&mTotal,&mLibre);
		fclose(fp);

		memVal = (int) ((mLibre/mTotal)*100);

		// Verifica valores.
		//*crearCliente=!(memVal<=CREAR_CLIENTE_MEM || cpuVal>=CREAR_CLIENTE_CPU);
		*detener=(memVal<=DETENER_CLIENTE_MEM || *detener==1);
	}

	printf("[ADMIN] - CPU : %d %% \n",cpuVal);
	printf("[ADMIN] - Memoria disponible: %d %%\n",memVal);

}

/*==================[external functions definition]==========================*/

void gestionarAdministracion(){
	pthread_t thread_id[MAX_HILOS_LOCAL];

	pthread_create( &(thread_id[0]), NULL, monitorRecursos, NULL);
	pthread_create( &(thread_id[1]), NULL, monitorConsola, NULL);

	int j;
	for(j=0; j < MAX_HILOS_LOCAL; j++){
		pthread_join( thread_id[j], NULL);
	}

	printf("[ADMIN] - Finaliza administracion.\n");
}

/*==================[end of file]============================================*/
