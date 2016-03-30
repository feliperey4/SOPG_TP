/*
 * svr_sem.h
 *
 *  Created on: 21/3/2016
 *      Author: felipe
 */

#ifndef CLTE_SEM_H_
#define CLTE_SEM_H_

/*==================[inclusions]=============================================*/

#include <sys/ipc.h>
#include <sys/sem.h>

/*==================[macros]=================================================*/

#define DIR_KEY_SEM "/home/felipe/ciaa/SO-PG/var.txt"	/* Directorio para la generaciond e llave para los semaforos. */

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/


/*
 * Funcion para inciar un semavforo con estado inicial.
 * @Param llave Llave del semaforo.
 * @Param valIni Valor inicial del semaforo.
 * @Return int Entero que indica -1 si ocurrio algun error.
 */
int SEM_init ( key_t llave, int valIni);

/*
 * Retorna el semaforo que contiene la llave de semaforos.
 * @Param llave Llave del semaforo.
 * @Retorna int Semaforo id.
 */
int SEM_get (key_t llave);

/*
 * Funcion para bloquear semaforo.
 * @Param semid Id del semaforo.
 */
void SEM_lock (int semid);

/*
 * Funcion para desbloquear semaforo.
 * @Param semid Id del semaforo.
 */
void SEM_unlock (int semid);

/*
 * Funcion para destruir semaforo.
 * @Param semid Id del semaforo.
 */
void SEM_destroy (int semid);

/*
 * Retorna la llave del semaforo de la lista de comandos por terminal.
 * @Retorna int Llave de lista de comandos por terminal.
 */
int SEM_getKeyCmdTerminal();

/*==================[end of file]============================================*/

#endif /* CLTE_SEM_H_ */
