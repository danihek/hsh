# hsh

PKG_CONFIG = pkg-config
PKGS = 

LIBS = #`$(PKG_CONFIG) --cflags $(PKGS)`
CFLAGS = #`$(PKG_CONFIG) --libs $(PKGS)`

FLAGS = -D_POSIX_C_SOURCE=200809L -ggdb -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wshadow -Wunused-macros -Werror=implicit -Werror=return-type -Werror=incompatible-pointer-types

all: hsh
hsh: hsh.o
	$(CC) hsh.o $(FLAGS) $(CFLAGS) $(LIBS) -o $@
hsh.o: hsh.c

clean:
	rm -f hsh *.o
