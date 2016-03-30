/*
 * svr_clientes.h
 *
 *  Created on: 17/3/2016
 *      Author: felipe
 */


#ifndef SVR_CLIENTES_H_
#define SVR_CLIENTES_H_

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*==================[macros]=================================================*/

#define ARCHIVO_CON "con%m%d%y%H%M.log"	/* Formato de archivo para guardar. */
#define ARCHIVO_BCK "bck%m%d%y%H%M.log"	/* Formato de archivo para guardar al cerrar el servidor. */

#define TIEMPO_ESPERA 1					/* Time out de lectura en la recepcion de da datos. Segundos. */

#define TIEMPO_KEEP_ALIVE 15			/* Tiempo de espera del keep alive de cada cliente. Segundos. */

#define CMD_KEEP_ALIVE 1				/* Comando de envio de keep alive. */
#define CMD_KEEP_ALIVE_OK 2				/* Comando de confirmacion de keep alive. */
#define CMD_RECIBIR_DATO 3				/* Comando para recibir datos. */
#define CMD_RECIBIR_DATO_OK 4			/* Comando de confirmacion de recepcion de datos. */
#define CMD_ENVIAR_BROADCAST 5			/* Comando para enviar broadcast. */
#define CMD_CERRAR_CTE_SVR 6			/* Comando de apagado del cliente. */
#define CMD_CERRAR_CTE_SVR_OK 7			/* Comando de confirmacion de apadado del cliente, */
#define CMD_CERRAR_SVR_CTE 8			/* Comando para apagar el cliente. */

/*==================[typedef]================================================*/

/*
 * Estructura para enviar informacion por el socket de control.
 */
typedef struct {
 	int id;			/* Id del cliente. */
 	int tipoMsg; 	/* Comando que se envia. */
} sInfoCtl;

/*
 * Estructura para enviar informacion por el socket de datos.
 */
typedef struct {
 	int id;			/* Id del cliente que recibe el mensaje. */
 	int idInfoClte;	/* Id del cliente al que pertenece la informacion del mensaje, */
 	int tipoMsg;	/* Comando que se envia. */
 	int esUltimo;	/* Bande que indica si es el ultimo mensaje enviado. */
 	int datos;		/* Contenido de los datos. */
 	char fecha[20];	/* Contenido de la fecha del mensaje. */
} sInfoDatos;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para lanzar y crear un hilo nuevo por cada cliente.
 */
void lanzadorClientes();

/*==================[end of file]============================================*/

#endif /* SVR_CLIENTES_H_ */
