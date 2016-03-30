/*
 * svr_shared.c
 *
 *  Created on: 20/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "clte_shared.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*
 * Retorna la shared memory id local, dependiendo de la una llave.
 * @Param llave Llave para generar shared memory.
 * @Param flags tipo de generacion del id.
 * @Retorna int Id de la shared memory local.
 */
static int getShmidLocal(char llave,int flags);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static int getShmidLocal(char llave,int flags){
	key_t key;
	int shmid;
	if ((key = ftok(KEY_DIR_SHR, llave)) == -1) {
		perror("ftok");
		return NULL;
	}
	if ((shmid = shmget(key, sizeof(int), flags)) == -1) {
		perror("shmget");
		return NULL;
	}
	return shmid;
}

/*==================[external functions definition]==========================*/

int* Shared_getDetener(int modo){
	int *detener;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	detener= (int *) shmat (getShmidLocal('R',flags), NULL, 0);
	return detener;
}

void Shared_destruirDetener(int *detener){
	shmdt(detener);
	shmctl (getShmidLocal('R',0644), IPC_RMID, 0);
}

sListaCliente * Shared_getListaCliente(int modo){
	sListaCliente *lista;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	lista= (sListaCliente *) shmat (getShmidLocal('A',flags), NULL, 0);
	return lista;
}

void Shared_destruirListaCliente(sListaCliente *lista){
	shmdt(lista);
	shmctl (getShmidLocal('A',0644), IPC_RMID, 0);
}

eCmdTerminal * Shared_getCmdTerminal(int modo){
	eCmdTerminal *cmd;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	cmd= (eCmdTerminal *) shmat (getShmidLocal('N',flags), NULL, 0);
	return cmd;
}

void Shared_destruirCmdTerminal(eCmdTerminal * cmd){
	shmdt(cmd);
	shmctl (getShmidLocal('N',0644), IPC_RMID, 0);
}
/*==================[end of file]============================================*/
