/*
 * svr_admin.h
 *
 *  Created on: 24/3/2016
 *      Author: felipe
 */

#ifndef SVR_ADMIN_H_
#define SVR_ADMIN_H_

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

#define DETENER_CLIENTE_MEM 2 	/* Minimo porcentaje de memoria disponible, para detener el servidor. */

#define CREAR_CLIENTE_MEM 5 	/* Minimo porcentaje de memoria para permitir conexion de clientes nuevos al servidor. */
#define CREAR_CLIENTE_CPU 100 	/* Maximo porcentaje de cpu para permitir conexion de clientes nuevos al servidor. */

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Funcion para gestionar la administracion del servido, tales como:
 * las entradas por consolas y el monitoreo de la gestio de recursos de cpu y memoria.
 */
void gestionarAdministracion();

/*==================[end of file]============================================*/

#endif /* SVR_ADMIN_H_ */
