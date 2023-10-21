/*-----------------------------------------------------+
 |     U S H. C                                        
 +-----------------------------------------------------+
 |     Versión :                                       |                      
 |     Autor :                                         |
 |     Asignatura :  SOP-GIIROB                        |                               
 |     Descripción :                                   |
 +-----------------------------------------------------*/
#define _GNU_SOURCE
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "profe.h"
#include <stdio.h>
#include <stdlib.h>
#define PATH_MAX 100

char *internas[] = {"exit" , "cd"};

//
// Declaraciones de funciones locales
//
void visualizar( void );
int leerLinea( char *linea, int tamanyLinea );
void return_dir(char *last_dir);

//
// Prog. ppal.
// 
int main(int argc, char * argv[], char *envp[])
{
    
  char line[255];
  int res;
  char **m_ordenes;
  char ***m_argumentos;
  int *m_num_arg;
  int m_n;
  char last_dir[PATH_MAX]; // array para guardar la ruta del directorio anterior. Se usa en cd
  int interna = 0; //hay o no orden interna en la línea de comandos
  getcwd(last_dir,PATH_MAX);
  
  printf("%s",LOGO);
  while(1)
  {
    
    do
    {
        res=leerLinea(line,MAXLINE);
        if (res==-2){
      		fprintf(stdout,"logout\n");
            	exit(0);
        }
    	if (res==-1){
		   fprintf(stdout,"linea muy larga\n");
        }
     }while(res<0);

     if (analizar(line)==OK)
     {
	     m_n=num_ordenes();
	     m_num_arg=num_argumentos();
	     m_ordenes=get_ordenes();
	     m_argumentos=get_argumentos();
	     if(m_n>0)
	     {
		for(int i=0;i < m_n; i++) {
			if(!strcmp(m_ordenes[i], internas[0])) {
				if(m_num_arg[i] == 1) {
					exit(0);
				} else if(m_num_arg[i] == 2) {
					int n = atoi(m_argumentos[i][1]);
					if(n > -1)
						exit(n);
					else {
						fprintf(stderr,"argumento fuera de rango\n");
						return ERROR;
					}
				} else {
					fprintf(stderr,"exit requiere de un argumento int\n");
					return ERROR;
				}
			} else if(!strcmp(m_ordenes[i], internas[1])) {
				interna = 1;
				if(m_num_arg[i] == 1) {			
					getcwd(last_dir,PATH_MAX);
					if(chdir(getenv("HOME"))) {
							fprintf(stderr,"error al cambiar de directorio\n");
                                                	return ERROR;
					}
					/*while (*envp)
						if((*envp[0] == 'H') && (*envp[1] == 'O') && (*envp[2] == 'M') && (*envp[3] == 'E')) {	
							chdir((*envp)+5);
						} else {
 							envp++;
						}*/
                                } else if(m_num_arg[i] == 2) {
                                        if(!strcmp(m_argumentos[i][1], "-")) { 
  						return_dir(last_dir);	      
					} else {	
  						getcwd(last_dir,PATH_MAX);
                                     		if(chdir(m_argumentos[i][1])) {
                                                	fprintf(stderr,"error al cambiar de directorio\n");
                                                	return ERROR;
						}
                                        }
				}
			}

		}
		if(interna) {
			interna = 0; //reiniciar para la siguiente linea de comandos
		} else {
		  if (pipeline(m_n,fich_entrada(),fich_salida(),es_append(),es_background()) == OK) {
                    if(ejecutar(m_n,m_num_arg,m_ordenes,m_argumentos,es_background()) == ERROR) 
		   	return ERROR;
		} else 
		    return ERROR;         
		}
	     }
      visualizar();
    } 
 }    

  return 0;
}

/*función que cambia al directorio anterior*/
void return_dir(char *last_dir) {
	char aux[PATH_MAX];
	strcpy(aux, last_dir);
	getcwd(last_dir,PATH_MAX);
	chdir(aux);      		
}

/****************************************************************/
/*                       leerLinea                             
  --------------------------------------------------------------
                                                               
   DESCRIPCIÓN:                                                 
   Obtiene la línea de órdenes para el mShell.    
   Util para depuracion.                                        
                                                                
   ENTRADA:                                                 
    linea - puntero a un vector de carácteres donde se almancenan los caracteres 
   leídos del teclado
    tamanyLinea - tamaño máximo del vector anterior

   SALIDA:
    -- linea - si termina bien, contiene como último carácter el retorno de carro.
    -- leerLinea -  Entero que representa el motivo de finalización de la función:
     > 0 - terminación correcta, número de caracteres leídos, incluído '\n'
     -1 - termina por haber alcanzado el número máximo de caracteres que se 
    espera leer de teclado, sin encontrar '\n'.
     -2 - termina por haber leído fin de fichero (EOF).
*/
/****************************************************************/
//char * getline(void)
int leerLinea( char *linea, int tamanyLinea )
{
  /*char path[PATH_MAX]; // array para guardar la ruta del directorio actual
  getcwd(path,PATH_MAX);
  printf("%s%s",path,PROMPT);*/
  printf("aniegil@upv.edu.es:%s%s>",get_current_dir_name(),PROMPT);
  //printf("Shell-Alumno$>");
  int i = 0;
  while(i < tamanyLinea) {
    char c = getchar();
    if(c == EOF) {
      //printf("logout\n");
      return -2;
    }
    i++;
    *linea = c;
    linea++;
    if(c == '\n') {
      return i;
    }
  }
  if(i == tamanyLinea) {
    printf("ERROR. Número máximo de caracteres en línea alcanzado.\n");
    return -1;
  }
}




/****************************************************************/
/*                       visualizar                             */
/*--------------------------------------------------------------*/
/*                                                              */
/* DESCRIPCIÓN:                                                 */
/* Visualiza los distintos argumentos de la orden analizada.    */
/* Util para depuracion.                                        */
/*                                                              */
/* ENTRADA: void                                                */
/*                                                              */
/* SALIDA: void                                                 */
/*                                                              */
/****************************************************************/
void visualizar( void )
{  
  printf("**********************************\n");
  printf("********   VISUALIZADOR   ********\n");
  printf("**********************************\n\n\n");
  printf("** ORDENES\n");
  int nord = num_ordenes();
  printf("** Numero de Ordenes : %d  \n", nord);
  if(!es_background())
    printf("** Orden en foreground\n");
  else
    printf("** Orden en background\n");
  int *narg = num_argumentos();
  //char **ordenes = get_ordenes();
  char ***argum = get_argumentos();
  for(int i=0;i < nord;i++) {
    printf("** Orden %d \n", i);
    for(int j=0;j < narg[i];j++)
      printf("**   Argumento %d --> %s \n", j, argum[i][j]);
  }
  printf("\n********************************** \n");
  printf("** REDIRECCION ENTRADA \n");
  printf("**  Redireccion entrada a --> %s\n", fich_entrada());
  printf("********************************** \n");
  printf("** REDIRECCION SALIDA \n");
  printf("**  Redireccion de salida a --> %s\n", fich_salida());
    if(strcmp(fich_salida(), "")) {
    	if(es_append())
      	printf("**    Es append\n");
    	else 
      	printf("**    Es trunc\n");
    }
  printf("********************************** \n\n");
} // Fin de "visualizar 



