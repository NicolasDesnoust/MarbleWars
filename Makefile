# Makefile de compilation avec GTK3 - 21/02/2016

SHELL   = /bin/bash
CC      = gcc
MKDEP   = gcc -MM
RM      = rm -f

# Si vous voulez utiliser gdb, rajouter "-g" a la fin de CFLAGS ;
# Si vous utilisez des fonctions de <math.h>, rajoutez "-lm" a la fin de LIBS.
CFLAGS  = $$(pkg-config gtk+-3.0 --cflags) -Wall -std=c99 -pedantic -O2
LIBS    = $$(pkg-config gtk+-3.0 --libs) -lm

# Mettre ici la liste des fichiers .c separes par un espace ;
# si besoin on peut eclater la liste sur plusieurs lignes avec "\".
CFILES  = main-app.c font.c curve.c util.c game.c mydata.c drawings.c menus.c gui.c

# Mettre ici le nom de l'executable.
EXEC    = marbleWars

# Calcul automatique de la liste des fichiers ".o" a partir de CFILES.
OBJECTS := $(CFILES:%.c=%.o)

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c

all :: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

clean ::
	$(RM) *.o *~ $(EXEC) depend

depend : *.c *.h
	$(MKDEP) *.c >| depend

# Inclut le fichier des dependances. 
# Le "-" devant include supprime l'erreur si le fichier est absent.
-include depend

