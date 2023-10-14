#ifndef __PROFES_H__
#define __PROFES_H__

int leerLinea_profe(char * linea,int tamanyLinea);
int visualizar_profe(void);
int pipeline_profe(int ncmd, char * infile, char * outfile, int append, int bgnd);
int ejecutar_profe (int nordenes , int *nargs , char **ordenes , char ***args , int bgnd);
int redirigir_entrada_profe(int i);
int redirigir_salida_profe(int i);
int cerrar_fd_profe();

#endif



