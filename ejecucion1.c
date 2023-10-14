#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defines.h"
#include <sys/wait.h>


char * ordenes[PIPELINE];
char * argumentos[PIPELINE][MAXARG];
int  ncmd = 0;
int  narg[PIPELINE];
int  es_background;
int  es_append;
char fich_entrada[255];
char fich_salida[255];
 
struct cmdfd
{
  int infd;
  int outfd;
}cmdfd[PIPELINE];

int nproc, mxfd;


void construye_orden (char *argv []);

void construye_pipe (void) ;

int main(int argc, char *argv[])
{ 
  int i;
  int status, fd[2];

  if (argc !=4 ) {
    fprintf(stderr, "Uso: %s fich_entrada:[nombre | -] fich_salida:[nombre | -] append:[1 | 0] \n", argv[0]);
    exit(1);
  }

  construye_orden(argv);
 
  construye_pipe();  
  
  if (fork() == 0) { /* CODIGO DEL HIJO */
        if (cmdfd[0].infd != STDIN_FILENO)  dup2( cmdfd[0].infd, STDIN_FILENO );
	if (cmdfd[0].outfd!= STDOUT_FILENO) dup2( cmdfd[0].outfd, STDOUT_FILENO );
    
	
    fprintf(stderr, "orden: %s, fd[entrada]: %d fd[salida]: %d \n", ordenes[0],cmdfd[0].infd, cmdfd[0].outfd);
    printf(" mxfd=%d\n", mxfd);
    for (i=2;i<=mxfd;i++){
      close(i);
    }
    execvp(ordenes[0], argumentos[0]);
    fprintf(stderr, "%s no encontrado o no ejecutable\n",ordenes[0] );
    exit(1);
    }

  /* CODIGO DEL PADRE*/

   if (cmdfd[1].infd != STDIN_FILENO)  dup2( cmdfd[1].infd, STDIN_FILENO );
   if (cmdfd[1].outfd!= STDOUT_FILENO) dup2( cmdfd[1].outfd, STDOUT_FILENO);


    fprintf(stderr, "orden: %s, fd[entrada]: %d fd[salida]: %d\n", ordenes[1],cmdfd[1].infd, cmdfd[1].outfd);
    printf(" mxfd=%d\n", mxfd);
    for (i=2;i<=mxfd;i++){
      close(i);
    }
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


void construye_pipe (void)
{

  int i, fds[2];

  int fd;


  /* Inicializamos la estructura del pipe */ 

  for (i=0; i<PIPELINE; i++){
    cmdfd[i].infd=0;
    cmdfd[i].outfd=1;
  }

  mxfd=3;

  nproc=ncmd;

  /* Redireccionamos la entrada */

   if (strcmp(fich_entrada,"\0"))
     {
      if ((fd = open(fich_entrada, O_RDONLY)) == -1) 
	{
	  printf("Error al leer el fichero de entrada %s. \n",fich_entrada); 
	  exit(1);
	 }
      else cmdfd[0].infd = fd;
     }

  /* Redireccionamos la salida */

     if (strcmp(fich_salida,"\0"))
     if (es_append==FALSE)
        if((fd = open(fich_salida, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
	  {
	    printf("Error al leer el fichero de salida %s. \n",fich_salida); 
	    exit(1);
	  }
        else cmdfd[nproc-1].outfd = fd;
     else 
        if((fd = open(fich_salida, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1)
	  {
	    printf("Error al leer el fichero de salida %s. \n",fich_salida);
	    exit(1);
	  }
        else cmdfd[nproc-1].outfd = fd;


     /* Creamos el tubo */

      if (pipe(fds) == -1) {  /* crear un tubo */
        fprintf(stderr, "ush: error al intentar crear un pipe\n");
        exit(1) ;
       }
      cmdfd[0].outfd=fds[1];
      cmdfd[1].infd=fds[0];
      mxfd=max(mxfd,fds[1]);
   
   if (cmdfd[0].infd==0 && es_background) cmdfd[0].infd = open ("/dev/null", O_RDONLY);
}
