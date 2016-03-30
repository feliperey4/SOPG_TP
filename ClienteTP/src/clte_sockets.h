/*
 * clte_sockets.h
 *
 *  Created on: 21/3/2016
 *      Author: felipe
 */

#ifndef CLTE_SOCKETS_H_
#define CLTE_SOCKETS_H_

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

#define MAX_RAND 100000					/* Maximo valor que puede alcanzar los valores aleatorios. */

#define FMT_DATE "%d/%m/%Y - %H:%M"		/* Formato de fecha hora. */

#define ARCHIVO_CON "con%m%d%y%H%M.log"	/* Formato del nombre del arhivo ha guardar. */

#define TIEMPO_ESPERA 1					/* Time out en la lectura de sockets. */

#define PUERTO_SVR 1230

#define TIEMPO_KEEP_ALIVE 3				/* Tiempo de espera del keep alive de cada cliente. Segundos. */
#define TIEMPO_GENERAR_DATO 5			/* Inervalo de tiempo en que se genera n nuevo dato y es enviado. En segundos. */

#define CMD_KEEP_ALIVE 			1		/* Comando de envio de keep alive. */
#define CMD_KEEP_ALIVE_OK 		2		/* Comando de confirmacion de keep alive. */
#define CMD_RECIBIR_DATO 		3		/* Comando para recibir datos. */
#define CMD_RECIBIR_DATO_OK 	4		/* Comando de confirmacion de recepcion de datos. */
#define CMD_ENVIAR_BROADCAST 	5		/* Comando para enviar broadcast. */
#define CMD_CERRAR_CTE_SVR 		6		/* Comando de apagado del cliente. */
#define CMD_CERRAR_CTE_SVR_OK 	7			/* Comando de confirmacion de apadado del cliente, */
#define CMD_CERRAR_SVR_CTE 		8		/* Comando para apagar el cliente. */


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
 * Estructura que contiene los file descritor asignados al cliente.
 */
typedef struct {
	int id;			/* Id del cliente. */
	int ctlFd;		/* File descriptor de control. */
	int datosFd;	/* File descriptor de datos. */
} sClienteSkts;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para realizar la conexion al servidor.
 */
void conectarCliente();

/*==================[end of file]============================================*/

#endif /* CLTE_SOCKETS_H_ */
