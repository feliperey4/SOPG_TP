/*
 * svr_listas.h
 *
 *  Created on: 19/3/2016
 *      Author: felipe
 */

#ifndef SVR_LISTAS_H_
#define SVR_LISTAS_H_

/*==================[inclusions]=============================================*/

#include <stdlib.h>
#include <string.h>

/*==================[macros]=================================================*/

#define HILO_PAUSADO 0	/* Estado de hilo pausado. */
#define HILO_INICIADO 1	/* Estado de hilo inciado o en ejecucion. */
#define HILO_DETENIDO 2	/* Estado de hilo detenido. */

#define BROADCAST_OFF 0	/* Estado de broadcast de apagado. */
#define BROADCAST_ON 1	/* Estado de broadcast de encendido. */

/*==================[typedef]================================================*/

/*
 * Estructura de elementos de lista de clientes pendientes.
 */
typedef struct sObjClienteP{
	int id;							/* Id del cliente. */
	int ctlFd;						/* Numero de file descriptor de control. */
	int datosFd;					/* Numero de file descriptor de datos. */
	struct sObjClienteP *siguiente;	/* Puntero de siguiente elemento en la lista. */
} sClienteP;

/*
 * Estructura de elementos de lista de clientes activos.
 */
typedef struct sObjClienteA{
	int id;							/* Id del cliente. */
	int ctlFd;						/* Numero de file descriptor de control. */
	int datosFd;					/* Numero de file descriptor de datos. */
	int datos;						/* Contenido del dato del cliente. */
	pthread_t hilo;					/* Hilo del cliente. */
	int estado;						/* Estado del hilo del cliente. */
	int activarBroadCast;			/* Bandera para activar el envio del broadcast para este cliente. */
	struct sObjClienteA *siguiente;	/* Puntero de siguiente elemento en la lista. */
	char *fecha;					/* Puntero de la fecha del cliente. */
} sClienteA;

/*
 * Estructura de lista de clientes activos.
 */
typedef struct sListaDescClienteA{
	sClienteA *inicio; 	/* Puntero al elemento incial de la lista. */
	sClienteA *fin;		/* Puntero al ultimo elemento de la lista. */
	int tamano;			/* Cantidad de elemento de la lista. */
} sListaClienteA;

/*
 * Estructura de lista de clientes pendientes.
 */
typedef struct sListaDescClienteP{
	sClienteP *inicio;	/* Puntero al elemento incial de la lista. */
	sClienteP *fin;		/* Puntero al ultimo elemento de la lista. */
	int tamano;			/* Cantidad de elemento de la lista. */
} sListaClienteP;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para inicializar una lista de clientes pendientes.
 * @Param *lista Lista de clientes pendientes.
 */
void Lista_CP_Nueva(sListaClienteP * lista);

/*
 * Funcion para agregar un elemento nuevo en la lista de cliente pendientes.
 * @Param *lista Lista de clientes pendientes.
 * @Param id Id del cliente nuevo.
 * @Param ctlFd File descriptor de control.
 * @Param datosFd File descriptor de datos.
 * @Retorna 0 si fue correcto y null ocurrio algun error.
 */
int Lista_CP_Put(sListaClienteP * lista,int id, int ctlFd,int datosFd);

/*
 * Funcion para obtener un elemento de la lista por id.
 * @Param *lista Lista de cliente pendientes.
 * @Param id Id del cliente pendiente.
 * Retorna *sCLienteP Puntero del cliente en la lista.
 */
sClienteP* Lista_CP_GetById(sListaClienteP * lista,int id);

/*
 * Funcion para borrar el primer elemnto de la lista de clientes pendientes.
 * @Param *lista Lista de clientes pendientes.
 */
void Lista_CP_BorrarInicio(sListaClienteP * lista);

/*
 * Funcion para borrar un cliente pendiente de la lista por id.
 * @Param *lista Puntero de lista de clientes pendientes.
 * @Param id Id de cliente pendientes en la lista.
 * @Retorna Null si hay algun error.
 */
int Lista_CP_BorrarById(sListaClienteP * lista,int id);

/*
 * Funcion para borrar todos los elementos de lista de clientes pendientes.
 * @Param *lista Puntero de lista de clientes pendientes.
 */
void Lista_CP_BorrarTodo(sListaClienteP * lista);

/*
 * Funcion para crear una lista nueva de clientes activos.
 * @Param *lista Puntero de lista de clientes activos.
 */
void Lista_CA_Nueva(sListaClienteA * lista);

/*
 * Funcion para agregar un nuevo elemento a la lista de clientes activos. Si el cliente ya existe en la lista, lo sobrescribe.
 * @Param *lista Lista de clientes activos.
 * @Param id Identificador unico en la lista de clientes activos.
 * @Param ctlFd File descriptor control para este clientes.
 * @Param datosFd File descriptor de datos para el cliente.
 * @Param datos Datos del contenido de cliente.
 * @Param *fecha FEcha del creacion del cliente,
 * @Retorna sClienteA* Puntero del cliente agregado.
 */
sClienteA* Lista_CA_Put(sListaClienteA * lista,int id, int ctlFd,int datosFd,int datos,char * fecha);

/*
 * Funcion para retornar un cliente activo de la lista, por id.
 * @Param *lista Lista de clientes activos.
 * @Param id Id del cliente que se retornara.
 * @Retorna sClienteA* Puntero del cliente encontrado.
 */
sClienteA* Lista_CA_GetById(sListaClienteA * lista,int id);

/*
 * Funcion para borrar el primer elemento de la lista de cliente activos.
 * @Param *lista Lista de clientes activos.
 */
void Lista_CA_BorrarInicio(sListaClienteA * lista);

/*
 * Funcion para borrar por id de una lista de clientes activos.
 * @Param *lista Lista de clientes activos.
 * @Param id Id del cliente de la lista ha borrar.
 * @Retorna Null si hay error.
 */
int Lista_CA_BorrarById(sListaClienteA * lista,int id);

/*
 * Funcion para borrar todos los elementos de la lista de clientes activos.
 * @Param *lista Lista de clientes activos.
 */
void Lista_CA_BorrarTodo(sListaClienteA * lista);

/*==================[end of file]============================================*/

#endif /* SVR_LISTAS_H_ */
