/*
 * svr_sem.c
 *
 *  Created on: 21/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "clte_sem.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int SEM_init ( key_t llave, int valIni){
	int semid = semget(llave,1,IPC_CREAT|0666);
	if ( semid==-1 ) return -1;
	semctl ( semid, 0, SETVAL, valIni);
	return semid;
}

int SEM_get (key_t llave){
	return semget(llave,1,0);
}

void SEM_lock (int semid){
	struct sembuf op_P [] ={0, -1, 0};
	semop ( semid, op_P, 1 );
}

void SEM_unlock (int semid){
	struct sembuf op_P [] ={0, 1, 0};
	semop ( semid, op_P, 1 );
}

void SEM_destroy (int semid){
	semctl ( semid, 0, IPC_RMID,0);
}

int SEM_getKeyCmdTerminal(){
	return ftok(DIR_KEY_SEM, 'L');
}

/*==================[end of file]============================================*/
