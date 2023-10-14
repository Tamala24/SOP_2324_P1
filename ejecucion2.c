#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defines.h"

#define BUFSIZE 1024

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

void construye_pipe (void) ;
void filtro(char *nombre);


main(int argc, char *argv[])
{ 
  int i;
  int status, fd[2];

  if (argc !=4 ) {
    fprintf(stderr, "Uso: %s fich_entrada:[nombre | -] fich_salida:[nombre | -] append:[1 | 0] \n", argv[0]);
    exit(1);
  }

  (strcmp("-",argv[1])==0) ? strcpy(fich_entrada, "\0") : strcpy(fich_entrada, argv[1]);
  (strcmp("-",argv[2])==0) ? strcpy(fich_salida, "\0")  : strcpy(fich_salida, argv[2]);
  es_append =( (strcmp("1",argv[3])==0 ) ? TRUE : FALSE);
  nproc=2;
  mxfd=3;
 
  construye_pipe();  
  
  if (fork() == 0) 
  { /* CODIGO DEL HIJO */
   
    if (cmdfd[0].infd != STDIN_FILENO)  dup2( cmdfd[0].infd, STDIN_FILENO );
	if (cmdfd[0].outfd!= STDOUT_FILENO) dup2( cmdfd[0].outfd, STDOUT_FILENO );
	
    fprintf(stderr, "Hijo: fd[entrada]: %d fd[salida]: %d \n", cmdfd[0].infd, cmdfd[0].outfd);
    for (i=3;i<=mxfd;i++){
      close(i);
    }
    
    printf("SOY EL HIJO Y DIGO:"); fflush(stdout);
    filtro("Hijo");
        
    exit(1);
    }

  /* CODIGO DEL PADRE*/

   if (cmdfd[1].infd != STDIN_FILENO)  dup2( cmdfd[1].infd, STDIN_FILENO );
   if (cmdfd[1].outfd!= STDOUT_FILENO) dup2( cmdfd[1].outfd, STDOUT_FILENO);


    fprintf(stderr, "Padre: fd[entrada]: %d fd[salida]: %d\n", cmdfd[1].infd, cmdfd[1].outfd);
    for (i=3;i<=mxfd;i++){
      close(i);
    }
    printf("SOY EL PADRE Y ME HAN ENVIADO: "); fflush(stdout);
    filtro("Padre");
    wait(&status);
    exit(1);
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


  /* Redireccionamos la entrada */

   if (strcmp(fich_entrada,"\0"))
     {
      if ((fd = open(fich_entrada, O_RDONLY)) == -1) 
	  {
	    printf("Error al leer el fichero de entrada %s. \n",fich_entrada); 
	    exit(1);
	  }
      else {cmdfd[0].infd = fd;}
     }

  /* Redireccionamos la salida */

     if (strcmp(fich_salida,"\0"))
     {
       if (es_append==FALSE)
       {
        if((fd = open(fich_salida, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
	    {
	      printf("Error al leer el fichero de salida %s. \n",fich_salida); 
	      exit(1);
	    }
        else {cmdfd[nproc-1].outfd = fd;}
       }
       else 
       {
        if((fd = open(fich_salida, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1)
	    {
	      printf("Error al leer el fichero de salida %s. \n",fich_salida);
	      exit(1);
	    }
        else {cmdfd[nproc-1].outfd = fd;}
       }
     }
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


void filtro(char * nombre)
{
  int count;
  char buf[BUFSIZE];

    while ( (count= read(STDIN_FILENO, buf, sizeof(buf))) >0 ) {
      if (write(STDOUT_FILENO, buf, count) != count){ 
        fprintf(stderr, "%s: no pudo leer del escribir en el tubo\n", nombre);
        exit(1);
      }
    }
}
