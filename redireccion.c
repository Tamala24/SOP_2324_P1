/*-----------------------------------------------------+      
 |     R E D I R E C C I O N . C                       |
 +-----------------------------------------------------+
 |     Version    :  4-Julio-2002/ 2006                      
 |     Autor :   Grupo de profesores de SO DISCA/DSIC
 |     Asignatura :  SO2                               
 |     Descripcion: Funciones encargadas del manejo de descriptores de
 |    fichero necesarios para las utilización de tuberías y redirecciones
 +-----------------------------------------------------*/
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"

CMDFD cmdfd;

extern int redireccion_ini(void) {
	int i;
	for(i=0;i<PIPELINE;i++) {
		cmdfd[i].infd = 0;	// Valor predeterminado de stdin = 0 (STDIN_FILENO)
		cmdfd[i].outfd = 1;	// Valor predeterminado de stdout = 1 (STDOUT_FILEOUT)
	}
	if(i == PIPELINE)
		return OK; // Si funciona todo bien
	else
		return ERROR;
} // Inicializar los valores de la estructura cmdfd



int pipeline(int ncmd, char * infile, char * outfile, int append, int bgnd) { 
	if(redireccion_ini() == ERROR) {
		fprintf(stderr, "Error al iniciar el vector de descriptores\n");
		return ERROR;
	}
	int fd, fds[2]; 
	if(strcmp(infile, "")) { 
		if((fd = open(infile, O_RDONLY)) == -1) {
			fprintf(stderr, "Error al abrir el fichero de entrada %s\n", infile);
			return ERROR;
		} else 
			cmdfd[0].infd = fd;
	}
	if(ncmd != 1) {
		for(int i=0;i<(ncmd-1);i++) {
			if(pipe(fds) == -1) {
				fprintf(stderr, "Error al intentar crear tubería\n");
				return ERROR;
			}
			cmdfd[i].outfd = fds[1];
			cmdfd[i+1].infd = fds[0];
		}
	} 
	if(strcmp(outfile, "")) {
		if(append == FALSE)
			if((fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
				fprintf(stderr, "Error al abrir el fichero de salida %s\n", outfile);
				return ERROR;
			} else
				cmdfd[ncmd-1].outfd = fd; 
		else
			if((fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1) {
				fprintf(stderr, "Error al abrir el fichero de salida %s\n", outfile);
                                return ERROR;
			} else
				cmdfd[ncmd-1].outfd = fd;
	}
	return OK;
} // Fin de la función "pipeline"






/*
 Funciones para la fase 3: módulo ejecución
 */
int cerrar_fd() // Recorre la estructura cmdfd para cerrar lo que sea distinto de 0 (stdin) y de 1 (stdout)
{
	for(int i=0;i<PIPELINE;i++) {
		if(cmdfd[i].infd != 0)
			if(close(cmdfd[i].infd) == -1)
				return ERROR;
		if(cmdfd[i].outfd != 1)
			if(close(cmdfd[i].outfd) == -1)
				return ERROR;	
	}
	return OK;
} // Fin de la función "cerrar_fd"



int redirigir_entrada(int i) {
	if(cmdfd[i].infd != STDIN_FILENO)
		if(dup2(cmdfd[i].infd, STDIN_FILENO) == -1)
			return ERROR;
	return OK;
} // Fin de la función "redirigir_entrada"



int redirigir_salida(int i) {
	if(cmdfd[i].outfd != STDOUT_FILENO)
		if(dup2(cmdfd[i].outfd, STDOUT_FILENO) == -1)
			return ERROR;
	return OK;
} // Fin de la función "redirigir_salida"
