/*
 * svr_listas.c
 *
 *  Created on: 19/3/2016
 *      Author: felipe
 */

/*==================[inclusions]=============================================*/

#include "svr_listas.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void Lista_CP_Nueva(sListaClienteP * lista){
	lista->inicio = NULL;
	lista->fin = NULL;
	lista->tamano = 0;
}

int Lista_CP_Put(sListaClienteP * lista,int id, int ctlFd,int datosFd){
	sClienteP *elementoNuevo=NULL;
	if((elementoNuevo=Lista_CP_GetById(lista,id))!=NULL){
		elementoNuevo->ctlFd=ctlFd;
		elementoNuevo->datosFd=datosFd;
	}
	else{
		if ((elementoNuevo = (sClienteP *) malloc(sizeof (sClienteP))) == NULL)
			return -1;
		elementoNuevo->siguiente=NULL;
		elementoNuevo->id=id;
		elementoNuevo->ctlFd=ctlFd;
		elementoNuevo->datosFd=datosFd;

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
	return 0;
}

sClienteP* Lista_CP_GetById(sListaClienteP * lista, int id){
	if(lista->tamano<=0)
		return NULL;

	sClienteP *elementoActual=NULL;
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

void Lista_CP_BorrarInicio(sListaClienteP *lista){
	if(lista->tamano<=0)
		return;

	sClienteP *elementoBorrar=NULL;
	elementoBorrar=lista->inicio;
	sClienteP *elementoPrimero=NULL;
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
	free(elementoBorrar);
}

int Lista_CP_BorrarById(sListaClienteP * lista,int id){
	if(lista->tamano<=0)
		return -1;
	
	if(lista->inicio->id==id){
		Lista_CP_BorrarInicio(lista);
		return 0;
	}
	
	if(lista->tamano==1)return -1;

	sClienteP *elemento=NULL;
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
		sClienteP *elementoBorrar=NULL;
		elementoBorrar=elemento->siguiente;
		elemento->siguiente=NULL;
		lista->tamano--;

		free(elementoBorrar);
		return 0;
	}

	if(borrar==0)
		return -3;

	sClienteP *elementoBorrar=NULL;
	elementoBorrar=elemento->siguiente;
	elemento->siguiente=elementoBorrar->siguiente;
	lista->tamano--;

	free(elementoBorrar);
	return 0;
}

void Lista_CP_BorrarTodo(sListaClienteP * lista){
	if(lista->tamano==0)
		return;
	while(lista->tamano>0){
		Lista_CP_BorrarInicio(lista);
	}
}


void Lista_CA_Nueva(sListaClienteA * lista){
	lista->inicio = NULL;
	lista->fin = NULL;
	lista->tamano = 0;
}

sClienteA* Lista_CA_Put(sListaClienteA * lista,int id, int ctlFd,int datosFd,int datos,char * fecha){
	sClienteA *elementoNuevo=NULL;
	if((elementoNuevo=Lista_CA_GetById(lista,id))!=NULL){
		elementoNuevo->ctlFd=ctlFd;
		elementoNuevo->datosFd=datosFd;
		elementoNuevo->datos=datos;
		elementoNuevo->estado=HILO_PAUSADO;
		elementoNuevo->activarBroadCast=BROADCAST_ON;
		strcpy(elementoNuevo->fecha,fecha);
	}
	else{
		if ((elementoNuevo = (sClienteA *) malloc(sizeof (sClienteA))) == NULL)
			return -1;
		if ((elementoNuevo->fecha = (char *) malloc (20 * sizeof (char))) == NULL)
					return -1;
		elementoNuevo->siguiente=NULL;
		elementoNuevo->id=id;
		elementoNuevo->ctlFd=ctlFd;
		elementoNuevo->datosFd=datosFd;
		elementoNuevo->datos=datos;
		elementoNuevo->estado=HILO_PAUSADO;
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

sClienteA* Lista_CA_GetById(sListaClienteA * lista, int id){
	if(lista->tamano<=0)
		return NULL;

	sClienteA *elementoActual=NULL;
	elementoActual=lista->inicio;

	if(elementoActual->id==id)return elementoActual;

	while(elementoActual->siguiente!=NULL){
		if(elementoActual->id==id)return elementoActual;
		elementoActual=elementoActual->siguiente;
	}
	if(elementoActual->id==id)return elementoActual;
	return NULL;
}

void Lista_CA_BorrarInicio(sListaClienteA *lista){
	if(lista->tamano<=0)
		return;

	sClienteA *elementoBorrar=NULL;
	elementoBorrar=lista->inicio;
	sClienteA *elementoPrimero=NULL;
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
	//printf("_____________ Borrando fecha\n");
	free(elementoBorrar->fecha);
	//printf("_____________ Borrando ClienteA\n");
	free(elementoBorrar);
}

int Lista_CA_BorrarById(sListaClienteA * lista,int id){
	if(lista->tamano<=0)
		return -1;

	if(lista->inicio->id==id){
		Lista_CA_BorrarInicio(lista);
		return 0;
	}
	
	if(lista->tamano==1)return -1;

	sClienteA *elemento=NULL;
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
		sClienteA *elementoBorrar=NULL;
		elementoBorrar=elemento->siguiente;
		elemento->siguiente=NULL;
		lista->tamano--;

		free(elementoBorrar->fecha);
		free(elementoBorrar);
		return 0;
	}

	if(borrar==0)
		return -1;

	sClienteA *elementoBorrar=NULL;
	elementoBorrar=elemento->siguiente;
	elemento->siguiente=elementoBorrar->siguiente;
	lista->tamano--;

	free(elementoBorrar->fecha);
	free(elementoBorrar);
	return 0;
}

void Lista_CA_BorrarTodo(sListaClienteA * lista){
	if(lista->tamano==0)
		return;
	while(lista->tamano>0){
		Lista_CA_BorrarInicio(lista);
	}
}

/*==================[end of file]============================================*/
