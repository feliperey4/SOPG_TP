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
#include "svr_shared.h"

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
	if ((key = ftok(DIR_KEY_SHR, llave)) == -1) {
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

int * Shared_getDetener(int modo){
	int *detener;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	detener= (int *) shmat (getShmidLocal('R',flags), NULL, 0);
	return detener;
}

void Shared_destruirDetener(int *detener){
	shmdt(detener);
	shmctl (getShmidLocal('R',0644), IPC_RMID, 0);
}

int * Shared_getIdImprimir(int modo){
	int *id;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	id= (int *) shmat (getShmidLocal('Q',flags), NULL, 0);
	return id;
}

void Shared_destruirIdImprimir(int * id){
	shmdt(id);
	shmctl (getShmidLocal('Q',0644), IPC_RMID, 0);
}


int * Shared_getCrearCliente(int modo){
	int *crearCliente;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	crearCliente= (int *) shmat (getShmidLocal('M',flags), NULL, 0);
	return crearCliente;
}
void Shared_destruirCrearCliente(int *crearCliente){
	shmdt(crearCliente);
	shmctl (getShmidLocal('M',0644), IPC_RMID, 0);
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

sListaClienteP * Shared_getListaClienteP(int modo){
	sListaClienteP *lista;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	lista= (sListaClienteP *) shmat (getShmidLocal('A',flags), NULL, 0);
	return lista;
}

void Shared_destruirListaClienteP(sListaClienteP *lista){
	shmdt(lista);
	shmctl (getShmidLocal('A',0644), IPC_RMID, 0);
}

sListaClienteA * Shared_getListaClienteA(int modo){
	sListaClienteA *lista;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	lista= (sListaClienteA *) shmat (getShmidLocal('B',flags), NULL, 0);
	return lista;
}

void Shared_destruirListaClienteA(sListaClienteA *lista){
	shmdt(lista);
	shmctl (getShmidLocal('B',0644), IPC_RMID, 0);
}

sPeticionClienteNuevo * Shared_getClienteP(int modo){
	sPeticionClienteNuevo *clienteP;
	int flags=(modo==SHR_CREAR)? 0644 | IPC_CREAT:0644;
	clienteP= (sPeticionClienteNuevo *) shmat (getShmidLocal('H',flags), NULL, 0);
	return clienteP;
}

void Shared_destruirClienteP(sPeticionClienteNuevo *clienteP){
	shmdt(clienteP);
	shmctl (getShmidLocal('H',0644), IPC_RMID, 0);
}
/*==================[end of file]============================================*/




