/*
 ============================================================================
 Name        : ServerTP.c
 Author      : Felipe Rey
 Version     :
 Copyright   : Felipe
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "svr_shared.h"
#include "svr_sockets.h"
#include "svr_clientes.h"
#include "svr_sem.h"
#include "svr_listas.h" 
#include "svr_admin.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void) {
	puts("Inicio");
	pid_t pid;

	//Crea semaforos.
	SEM_init(SEM_getKeyListaClienteP(),1);
	SEM_init(SEM_getKeyCmdTerminal(),1);

	// Crea shared
	int *detener;
	int *crearCliente;
	int * idImpr;
	sListaClienteP *listaClientesP;
	sListaClienteA *listaClientesA;
	sPeticionClienteNuevo *clienteP;
	eCmdTerminal * cmdTerminal;

	detener= Shared_getDetener(SHR_CREAR);
	idImpr= Shared_getIdImprimir(SHR_CREAR);
	crearCliente= Shared_getCrearCliente(SHR_CREAR);
	cmdTerminal=Shared_getCmdTerminal(SHR_CREAR);
	listaClientesP=Shared_getListaClienteP(SHR_CREAR);
	listaClientesA=Shared_getListaClienteA(SHR_CREAR);
	clienteP=Shared_getClienteP(SHR_CREAR);

	Lista_CP_Nueva(listaClientesP);
	Lista_CA_Nueva(listaClientesA);
	*detener=0;
	*idImpr=-1;
	*crearCliente=1;
	*cmdTerminal=NADA;
	clienteP->id=0;
	clienteP->puertoCtl=0;
	clienteP->puertoDatos=0;
	clienteP->isLanzado=0;

	//Crea proceso de Sockets.
	if ((pid=fork()) == 0){
		//Proceso de sockets
		printf(" Sockets: pid: %d\n", getpid());
		atenderSockets();
		exit(0);
	}
	else if(pid < 0){
		perror("fork");
		exit(1);
	}
 
	//Crea proceso administrador.
	if ((pid=fork()) == 0){
		//Proceso Lanzador.
		printf(" Administracion: pid: %d\n", getpid());
		gestionarAdministracion();
		exit(0);
	}
	else if(pid < 0){
		perror("fork");
		exit(1);
	}


	printf(" Admin Papa: pid: %d\n", getpid());

	//Espera a terminar todos los procesos.
	printf(" Esperando a terminar los procesos. \n");
	int i=2;
	while(wait()){
		printf(" Termina proceso .\n");
		i--;
		if(i==0)
			break;
	}

	//Destruye shared
	Shared_destruirDetener(detener);
	Shared_destruirIdImprimir(idImpr);
	Shared_destruirCrearCliente(crearCliente);
	Shared_destruirCmdTerminal(cmdTerminal);
	Shared_destruirListaClienteP(listaClientesP);
	Shared_destruirListaClienteA(listaClientesA);
	Shared_destruirClienteP(clienteP);

	// Destruye semanforos.
	SEM_destroy(SEM_getKeyListaClienteP());
	SEM_destroy(SEM_getKeyCmdTerminal());

	puts("Fin");
	return EXIT_SUCCESS;
}

/*==================[end of file]============================================*/
