/*
 ============================================================================
 Name        : ClienteTP.c
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

#include "clte_sockets.h"
#include "clte_listas.h"
#include "clte_sem.h"
#include "clte_shared.h"
#include "clte_admin.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void) {
	puts("************ INCIO ********\n");
	pid_t pid;
	sCliente cliente;

	SEM_init(SEM_getKeyCmdTerminal(),1);

	int *detener;
	sListaCliente *listaClientes;
	eCmdTerminal * cmdTerminal;

	detener= Shared_getDetener(SHR_CREAR);
	listaClientes=Shared_getListaCliente(SHR_CREAR);
	cmdTerminal=Shared_getCmdTerminal(SHR_CREAR);

	if ((pid=fork()) == 0){
		//Proceso de sockets
		printf(" Clientes: pid: %d\n", getpid());
		conectarCliente(&cliente);
		exit(0);
	}
	else if(pid < 0){
		perror("fork");
		exit(1);
	}

	if ((pid=fork()) == 0){
		//Proceso de administrador
		printf(" Administrador: pid: %d\n", getpid());
		gestionarAdministracion();
		exit(0);
	}
	else if(pid < 0){
		perror("fork");
		exit(1);
	}

	//Espera a terminar todos los procesos.
	printf(" Esperando a terminar los procesos. \n");
	int i=1;
	int estado;
	while(wait(estado)){
		printf(" Termina proceso pid: %d\n", estado);
		i--;
		if(i==0)
			break;
	}

	Shared_destruirDetener(detener);
	Shared_destruirListaCliente(listaClientes);
	Shared_destruirCmdTerminal(cmdTerminal);

	SEM_destroy(SEM_getKeyCmdTerminal());

	puts("************ FIN ********\n");
	return EXIT_SUCCESS;
}

/*==================[end of file]============================================*/
