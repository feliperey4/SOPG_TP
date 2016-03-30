/*
 * clte_listas.h
 *
 *  Created on: 19/3/2016
 *      Author: felipe
 */

#ifndef CLTE_LISTAS_H_
#define CLTE_LISTAS_H_

/*==================[inclusions]=============================================*/

#include <stdlib.h>
#include <string.h>

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*
 * Estructura de elementos de lista de clientes.
 */
typedef struct sObjCliente{
	int id;							/* Id del cliente. */
	int datos;						/* Contenido del dato del cliente. */
	char *fecha;					/* Puntero de la fecha del cliente. */
	struct sObjCliente *siguiente;	/* Puntero de siguiente elemento en la lista. */
} sCliente;

/*
 * Estructura de lista de clientes.
 */
typedef struct sListaDescCliente{
	sCliente *inicio;	/* Puntero al elemento incial de la lista. */
	sCliente *fin;		/* Puntero al ultimo elemento de la lista. */
	int tamano;			/* Cantidad de elemento de la lista. */
} sListaCliente;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para inicializar una lista de clientes.
 * @Param *lista Lista de clientes.
 */
void Lista_C_Nueva(sListaCliente * lista);

/*
 * Funcion para agregar un elemento nuevo en la lista de cliente.
 * @Param *lista Lista de clientes.
 * @Param id Id del cliente nuevo.
 * @Param ctlFd File descriptor de control.
 * @Param datosFd File descriptor de datos.
 * @Retorna sCliente* Puntero de cliente agregado.
 */
sCliente * Lista_C_Put(sListaCliente * lista,int id, int datos,char *fecha);

/*
 * Funcion para obtener un elemento de la lista por id.
 * @Param *lista Lista de cliente.
 * @Param id Id del cliente.
 * Retorna *sCliente Puntero del cliente en la lista.
 */
sCliente* Lista_C_GetById(sListaCliente * lista,int id);

/*
 * Funcion para borrar el primer elemnto de la lista de clientes.
 * @Param *lista Lista de clientes.
 */
void Lista_C_BorrarInicio(sListaCliente * lista);

/*
 * Funcion para borrar un cliente de la lista por id.
 * @Param *lista Puntero de lista de clientes.
 * @Param id Id de cliente en la lista.
 * @Retorna Null si hay algun error.
 */
int Lista_C_BorrarById(sListaCliente * lista,int id);

/*
 * Funcion para borrar todos los elementos de lista de clientes.
 * @Param *lista Puntero de lista de clientes.
 */
void Lista_C_BorrarTodo(sListaCliente * lista);

/*==================[end of file]============================================*/

#endif /* CLTE_LISTAS_H_ */
