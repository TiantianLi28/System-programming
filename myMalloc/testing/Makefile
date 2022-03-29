TARGET=ppmcvt my_malloc-driver
CC=gcc 
CFLAGS=-g -std=gnu99 -Wno-deprecated-declarations

.PHONY: cleanall cleanobj

all: $(TARGET)

ppmcvt: pbm.o pbm_aux.o ppmcvt.o
my_malloc-driver: my_malloc.o my_malloc-driver.o

cleanobj:
	$(RM) pbm.o pbm_aux.o ppmcvt.o
	$(RM) my_malloc.o my_malloc-driver.o

cleanall: cleanobj
	$(RM) $(TARGET)
