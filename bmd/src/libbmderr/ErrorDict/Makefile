CFLAGS=-g -DHAVE_PCRE_H
INCLUDE_FLAGS=-I ../../../include -I /opt/pq824/include -I /usr/include/pgsql -I /home/marua/pq8210/include -I /usr/include/libxml2

all: make_err_dict

make_err_dict: make_err_dict.o
	gcc -o make_err_dict make_err_dict.o

make_err_dict.o: make_err_dict.c
	gcc -c $(CFLAGS) $(INCLUDE_FLAGS) make_err_dict.c

clean:
	rm -f *.o make_err_dict
