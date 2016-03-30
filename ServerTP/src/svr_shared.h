/*
 * svr_shared.h
 *
 *  Created on: 20/3/2016
 *      Author: felipe
 */


#ifndef SVR_SHARED_H_
#define SVR_SHARED_H_

/*==================[inclusions]=============================================*/

#include "svr_listas.h"
#include "svr_sockets.h"

/*==================[macros]=================================================*/

#define SHR_CREAR 1  									/* Bandera para crear la Shared memory. */
#define SHR_OBTENER 2 									/* Bandera para obtener una shared memory. */

#define DIR_KEY_SHR "/home/felipe/ciaa/SO-PG/var2.txt" 	/* Directorio de la llave de la shared memory. */

/*==================[typedef]================================================*/

/*
 * Enumeracion que representa las entradas
 * por consola y su accion.
 */
typedef enum {
   NADA,					/* No realiza nada. */
   APAGAR,					/* Comando para apagar el servidor. */
   IMPRIMIR_ULTIMO_CLIENTE,	/* Compando para imprimir el ultimo cliente conectado al servidor. */
   IMPRIMIR_POR_ID, 		/* Comando para imprimir el cliente por ID. */
   GUARDAR_TODO 			/* Comando para guardar todos los clientes conectados y sus datos. */
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
 * Retorna la shared memory de permiso de crear clientes,
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna int* Puntero de la shared memory crear clientes.
 */
int* Shared_getCrearCliente(int flags);

/*
 * Destruye la shared memory de permisos de crear clientes.
 * @Param *crearCliente Puntero de shared memory crear cliente.
 */
void Shared_destruirCrearCliente(int *crearCliente);

/*
 * Retorna la shared memory de Comando ingresado por la terminal.
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna eCmdTerminal* Puntero de la shared memory de comando ingresado por la terminal.
 */
eCmdTerminal * Shared_getCmdTerminal(int modo);

/*
 * Destruye la shared memory de comandos ingresados por consola.
 * @Param *cmd Puntero de shared memory de comandos ingresados por consola.
 */
void Shared_destruirCmdTerminal(eCmdTerminal * cmd);

/*
 * Retorna la shared memory del id de cliente ha imprimir.
 * @Param modo Entero que representa la bandera de crear o obtener.
 * @Retorna int* Puntero de la shared memory del id de cliente ha imprimir.
 */
int * Shared_getIdImprimir(int modo);

/*
 * Destruye la shared memory del id de cliente ha imprimir.
 * @Param *id Puntero de shared memory del id de cliente ha imprimir.
 */
void Shared_destruirIdImprimir(int * id);

/*
 * Retorna la shared memory de lista de clientes pendientes.
 * @Param modo Entero que representa la bandera de crear o obtener.
 * @Retorna sListaClienteP* Puntero de la shared memory de lista de clientes pendientes.
 */
sListaClienteP * Shared_getListaClienteP(int flags);

/*
 * Destruye la shared memory de lista de clientes pendientes.
 * @Param *lista Puntero de shared memory de lista de clientes pendientes.
 */
void Shared_destruirListaClienteP(sListaClienteP *lista);

/*
 * Retorna la shared memory de lista de clientes activos.
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna sListaClienteA* Puntero de la shared memory de lista de clientes activos.
 */
sListaClienteA * Shared_getListaClienteA(int flags);

/*
 * Destruye la shared memory de lista de clientes activos.
 * @Param *lista Puntero de shared memory de lista de clientes activos.
 */
void Shared_destruirListaClienteA(sListaClienteA *lista);

/**
 * Retorna la shared memory de cliente pendiente.
 * @Param flags Entero que representa la bandera de crear o obtener.
 * @Retorna sPeticionClienteNuevo* Puntero de la shared memory de cliente pendiente.
 */
sPeticionClienteNuevo * Shared_getClienteP(int flags);

/*
 * Destruye la shared memory de cliente pendiente.
 * @Param *clienteP Puntero de shared memory de cliente pendiente.
 */
void Shared_destruirClienteP(sPeticionClienteNuevo *clienteP);

/*==================[end of file]============================================*/

#endif /* SVR_SHARED_H_ */
