/*
 * svr_shared.h
 *
 *  Created on: 20/3/2016
 *      Author: felipe
 */

#ifndef CLTE_SHARED_H_
#define CLTE_SHARED_H_

/*==================[inclusions]=============================================*/

#include "clte_listas.h"

/*==================[macros]=================================================*/

#define SHR_CREAR 1  									/* Bandera para crear la Shared memory. */
#define SHR_OBTENER 2 									/* Bandera para obtener una shared memory. */

#define KEY_DIR_SHR "/home/felipe/ciaa/SO-PG/var.txt"	/* Directorio de la llave de la shared memory. */

/*==================[typedef]================================================*/

/*
 * Enumeracion que representa las entradas
 * por consola y su accion.
 */
typedef enum {
   NADA,		/* No realiza nada. */
   APAGAR,		/* Comando para apagar el servidor. */
   GUARDAR_TODO	/* Comando para guardar todos los clientes conectados y sus datos. */
} eCmdTerminal;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Retorna la shared memory de la varible detener,
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna int* Puntero de la shared memory detener.
 */
int* Shared_getDetener(int flags);

/*
 * Destruye la shared memory detener.
 * @Param *detener Puntero de shared memory detener.
 */
void Shared_destruirDetener(int *detener);

/*
 * Retorna la shared memory de la lista de clientes,
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna sListaCliente* Puntero de la shared memory de lista de clientes.
 */
sListaCliente * Shared_getListaCliente(int flags);

/*
 * Destruye la shared memory de lista de clientes.
 * @Param *lista Puntero de shared memory de lista de clientes.
 */
void Shared_destruirListaCliente(sListaCliente *lista);

/*
 * Retorna la shared memory del comando de la reminal,
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna eCmdTerminal* Puntero de la shared memory del comando de la reminal.
 */
eCmdTerminal * Shared_getCmdTerminal(int modo);

/*
 * Destruye la shared memory del comando de la reminal,
 * @Param *cmd Puntero de shared memory del comando de la reminal,
 */
void Shared_destruirCmdTerminal(eCmdTerminal * cmd);

/*==================[end of file]============================================*/

#endif /* CLTE_SHARED_H_ */
