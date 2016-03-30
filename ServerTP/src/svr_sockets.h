/*
 * svr_sockets.h
 *
 *  Created on: 17/3/2016
 *      Author: felipe
 */


#ifndef SVR_SOCKETS_H_
#define SVR_SOCKETS_H_

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

#define PUERTO_CLIENTE 1230 	/* Puerto de escucha de clientes nuevos. */
#define PUERTO_CTL 1231 		/* Puerto de control con los clientes. */
#define PUERTO_DATA 1232 		/* Puerto de intercambio de datos con los clientes. */

#define MAX_COL_PETICIONES 1000 /* Maximo numero de clientes conectados al mismo tiempo. */

/*==================[typedef]================================================*/

/*
 * Enumeracion del tipo de socket.
 */
typedef enum {
	SKT_ESCUCHA_CLIENTE, 	/* Socket de escucha de clientes.*/
	SKT_CTL,				/* Socket de control. */
	SKT_DATOS				/* Socket de datos. */
} eTipoSocket;

/*
 * Estructura que representa la peticion de un cliente nuevo.
 */
typedef struct {
	int id;				/* ID del cliente. */
	int puertoCtl; 		/* Puerto de conexion de control. */
	int puertoDatos;	/* Puerto de conexion de datos. */
	int isLanzado;		/* Estado del cliente, indicando si ya ha pasado la etapa de conexion. */
} sPeticionClienteNuevo;

/*
 * Estructura que representa un socket.
 */
typedef struct {
	eTipoSocket tipo; 	/* Tipo de socket */
	int puerto; 		/* Puerto de conexion para socket. */
} sSocket;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para atender y gestionar todos los sockets.
 */
void atenderSockets();

/*==================[end of file]============================================*/

#endif /* SVR_SOCKETS_H_ */
