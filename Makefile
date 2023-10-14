machine = $(shell uname -m)
ifeq ("$(machine)","x86_64")
	lib_file=profe.a
else
	lib_file=profe.a
endif


all: ush

todo: ush ejecucion1 ejecucion1a ejecucion2 ejecucion3

ejecucion1: ejecucion1.c
	gcc ejecucion1.c -o ejecucion1
	strip ejecucion1

ejecucion2: ejecucion2.c
	gcc ejecucion2.c -o ejecucion2
	strip ejecucion2
	
ejecucion3: ejecucion3.c
	gcc ejecucion3.c -o ejecucion3
	strip ejecucion3

ejecucion1a: ejecucion1a.c defines.h redireccion.h redireccion.o
	gcc ejecucion1a.c redireccion.o -o ejecucion1a
	strip ejecucion1a

ush: ush.c defines.h analizador.h redireccion.h $(lib_file) analizador.o redireccion.o ejecucion.o
	gcc ush.c $(lib_file) analizador.o redireccion.o ejecucion.o -o ush
	strip ush

analizador.o: analizador.c defines.h
	gcc -c analizador.c

redireccion.o: redireccion.c defines.h
	gcc -c redireccion.c

ejecucion.o: ejecucion.c defines.h analizador.c redireccion.h
	gcc -c ejecucion.c

clean:
	rm *.o ush ejecucion1 ejecucion2 ejecucion1a ejecucion3

profe:  ush.c defines.h analizador.h profe.c 
	gcc -fno-stack-protector -m32 -c -o profe_32.a profe.c 
	gcc -m64 -c -o profe_64.a profe.c 



