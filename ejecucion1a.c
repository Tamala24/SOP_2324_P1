#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "defines.h"
#include "redireccion.h"
#include "profe.h"


char * ordenes[PIPELINE];
char * argumentos[PIPELINE][MAXARG];
int  ncmd = 0;
int  narg[PIPELINE];
int  es_background;
int  es_append;
char fich_entrada[255];
char fich_salida[255];


extern struct cmdfd
{
  int infd;
  int outfd;
}cmdfd[PIPELINE];




void construye_orden (char *argv []);


int main(int argc, char *argv[])
{ 
  int i;
  int status, fd[2];

  if (argc !=4 ) {
    fprintf(stderr, "Uso: %s fich_entrada:[nombre | -] fich_salida:[nombre | -] append:[1 | 0] \n", argv[0]);
    exit(1);
  }

  construye_orden(argv);

  if ( (pipeline(2,argv[1],argv[2],es_append,es_background)) !=OK ) exit(1);  


  if (fork() == 0) { /* CODIGO DEL HIJO */

    redirigir_entrada(0);
    redirigir_salida(0);

    fprintf(stderr, "orden: %s, fd[entrada]: %d fd[salida]: %d \n", ordenes[0],cmdfd[0].infd, cmdfd[0].outfd);

    cerrar_fd();

    execvp(ordenes[0], argumentos[0]);
    fprintf(stderr, "%s no encontrado o no ejecutable\n",ordenes[0] );
    exit(1);
    }

  /* CODIGO DEL PADRE*/

    redirigir_entrada(1);
    redirigir_salida(1);

    fprintf(stderr, "orden: %s, fd[entrada]: %d fd[salida]: %d\n", ordenes[1],cmdfd[1].infd, cmdfd[1].outfd);

    cerrar_fd();

    execvp(ordenes[1], argumentos[1]);
    fprintf(stderr, "%s no encontrado o no ejecutable\n",ordenes[1] );
    wait(&status);
    exit(1);
}


void construye_orden(char * argv[])
{ 
  int i,j;

  ncmd=2;
  narg[0]=1;
  narg[1]=1;
  es_background=FALSE;

  for(i=0; i<PIPELINE; i++) 
       for(j=0; j<MAXARG; j++) argumentos[i][j]=NULL;
  ordenes[0]="cat";
  ordenes[1]="sort";
  argumentos[0][0] = "cat";
  argumentos[1][0] = "sort";
  (strcmp("-",argv[1])==0) ? strcpy(fich_entrada, "\0") : strcpy(fich_entrada, argv[1]);
  (strcmp("-",argv[2])==0) ? strcpy(fich_salida, "\0")  : strcpy(fich_salida, argv[2]);
  es_append =( (strcmp("1",argv[3])==0 ) ? TRUE : FALSE);
}

