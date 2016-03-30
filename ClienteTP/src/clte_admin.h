/*
 * clte_admin.h
 *
 *  Created on: 24/3/2016
 *      Author: felipe
 */

#ifndef CLTE_ADMIN_H_
#define CLTE_ADMIN_H_

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

#define DETENER_CLIENTE_MEM 2	/* Minimo porcentaje de memoria disponible, para detener el servidor. */

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para gestionar la administracion del servido, tales como:
 * las entradas por consolas y el monitoreo de la gestio de recursos de cpu y memoria.
 */
void gestionarAdministracion();

/*==================[end of file]============================================*/

#endif /* CLTE_ADMIN_H_ */
