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
//#define PATH_MAX 50

//
// Declaraciones de funciones locales
//
void visualizar( void );
int leerLinea( char *linea, int tamanyLinea );


//
// Prog. ppal.
// 
int main(int argc, char * argv[])
{
    
  char line[255];
  int res;
  char **m_ordenes;
  char ***m_argumentos;
  int *m_num_arg;
  int m_n;

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
                if (pipeline(m_n,fich_entrada(),fich_salida(),es_append(),es_background()) == OK) {
                    if(ejecutar(m_n,m_num_arg,m_ordenes,m_argumentos,es_background()) == ERROR) 
		   	return ERROR;
		} else 
		    return ERROR;         
    	     }
      visualizar();
    } 
 }    

  return 0;
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
  printf("%s%s>",get_current_dir_name(),PROMPT);
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



