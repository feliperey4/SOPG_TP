/*
 * clte_listas.c
 *
 *  Created on: 19/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "clte_listas.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


void Lista_C_Nueva(sListaCliente * lista){
	//lista = (sListaClienteP*) malloc(sizeof (sListaClienteP));
	lista->inicio = NULL;
	lista->fin = NULL;
	lista->tamano = 0;
}

sCliente * Lista_C_Put(sListaCliente * lista,int id, int datos,char *fecha){
	sCliente *elementoNuevo=NULL;
	if((elementoNuevo=Lista_C_GetById(lista,id))!=NULL){
		elementoNuevo->datos=datos;
		strcpy(elementoNuevo->fecha,fecha);
	}
	else{
		if ((elementoNuevo = (sCliente *) malloc(sizeof (sCliente))) == NULL)
			return NULL;
		if ((elementoNuevo->fecha = (char *) malloc (20 * sizeof (char))) == NULL)
			return NULL;
		elementoNuevo->siguiente=NULL;
		elementoNuevo->id=id;
		elementoNuevo->datos=datos;
		strcpy(elementoNuevo->fecha,fecha);

		if(lista->tamano==0){
			lista->inicio=elementoNuevo;
			lista->fin=elementoNuevo;
		}
		else{
			lista->fin->siguiente=elementoNuevo;
			lista->fin=elementoNuevo;
		}
		lista->tamano++;
	}
	return elementoNuevo;
}

sCliente* Lista_C_GetById(sListaCliente * lista, int id){
	if(lista->tamano<=0)
		return NULL;

	sCliente *elementoActual=NULL;
	elementoActual=lista->inicio;

	if(elementoActual->id==id){
		//elemento=elementoActual;
		return elementoActual;
	}
	while(elementoActual->siguiente!=NULL){
		if(elementoActual->id==id){
			//elemento=elementoActual;
			return elementoActual;
		}
		elementoActual=elementoActual->siguiente;
	}
	if(elementoActual->id==id){
		//elemento=elementoActual;
		return elementoActual;
	}
	return NULL;
}

void Lista_C_BorrarInicio(sListaCliente *lista){
	if(lista->tamano<=0)
		return;

	sCliente *elementoBorrar=NULL;
	elementoBorrar=lista->inicio;
	sCliente *elementoPrimero=NULL;
	if(lista->tamano==1){
		lista->inicio=NULL;
		lista->fin=NULL;
		lista->tamano=0;
	}
	else{
		elementoPrimero=elementoBorrar->siguiente;
		lista->inicio=elementoPrimero;
		lista->tamano--;
	}

	//3.0 Libera recursos.
	free(elementoBorrar->fecha);
	free(elementoBorrar);
}

int Lista_C_BorrarById(sListaCliente * lista,int id){
	if(lista->tamano<=0)
		return -1;
	
	if(lista->inicio->id==id){
		Lista_C_BorrarInicio(lista);
		return 0;
	}
	
	if(lista->tamano==1)return -1;

	sCliente *elemento=NULL;
	elemento=lista->inicio;
	int borrar=0;
	int i;
	for(i=0;i<(lista->tamano-1);i++){
		if(elemento->siguiente==NULL)
			break;
		if(elemento->siguiente->id==id){
			borrar=1;
			break;
		}
		elemento=elemento->siguiente;
	}

	//Ultima posicion.
	if(elemento->siguiente->id==id && borrar==0){
		sCliente *elementoBorrar=NULL;
		elementoBorrar=elemento->siguiente;
		elemento->siguiente=NULL;
		lista->tamano--;
		free(elementoBorrar->fecha);
		free(elementoBorrar);
		return 0;
	}

	if(borrar==0)
		return -3;

	sCliente *elementoBorrar=NULL;
	elementoBorrar=elemento->siguiente;
	elemento->siguiente=elementoBorrar->siguiente;
	lista->tamano--;

	free(elementoBorrar->fecha);
	free(elementoBorrar);
	return 0;
}

void Lista_C_BorrarTodo(sListaCliente * lista){
	if(lista->tamano==0)
		return;
	while(lista->tamano>0){
		Lista_C_BorrarInicio(lista);
	}
}

/*==================[end of file]============================================*/
