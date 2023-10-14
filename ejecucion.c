/*-----------------------------------------------------+      
 |     R E D I R E C C I O N . C                       |
 +-----------------------------------------------------+
 |     Version    :                                    |                      
 |     Autor :                                         |
 |     Asignatura :  SOP-GIIROB                        |
 |     Descripcion:                                    |
 +-----------------------------------------------------*/
#include "defines.h"
#include "redireccion.h"
#include "ejecucion.h"
#include "analizador.h"
#include <signal.h>
#include <sys/wait.h>


int ejecutar (int nordenes , int *nargs , char **ordenes , char ***args , int bgnd) {
  pid_t aux;
  int i;
  //if(pipeline(nordenes, fich_entrada(), fich_salida(), es_append(), bgnd) == ERROR)
//	fprintf(stderr,"Error al invocar la función pipeline\n"); 
  for(i=0;i < nordenes;i++) {
    aux = fork();
    if(aux == 0) {
      	if(redirigir_entrada(i) == ERROR) {	// Redireccionar entrada
		fprintf(stderr,"Error al intentar redirigir entrada en la orden:%s\n", ordenes[i]);	
		return ERROR;
	}
      	if(redirigir_salida(i) == ERROR) {	// Redireccionar salida
		fprintf(stderr,"Error al intentar redirigir salida en la orden:%s\n", ordenes[i]);	
		return ERROR;
	}
      	if(cerrar_fd() == ERROR) {		// Cerrar descriptores
		fprintf(stderr,"Error al cerrar descriptores en la orden:%s\n", ordenes[i]);	
		return ERROR;
	}
      	execvp(ordenes[i],args[i]);		// Acaba el proceso REVISAR
      	fprintf(stderr,"Error al ejecutar el proceso %s\n", ordenes[i]);
      	return ERROR;
    } else if(aux == -1) {
	fprintf(stderr,"Error al crear el proceso para la orden: %s\n", ordenes[i]);
  	return ERROR;
	}
  }
  if(cerrar_fd() == ERROR) {			// Cerrar descriptores
	fprintf(stderr,"Error al cerrar descriptores en la orden:%s\n", ordenes[i]);	
	return ERROR;	
  }
  while (wait(NULL) != -1); 			// Espera a que acaben los hijos
  printf("\n");
  return OK;
} // Fin de la funcion "ejecutar" 
