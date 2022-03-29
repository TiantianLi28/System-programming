TARGET=ppmcvt
CC=gcc 
CFLAGS=-g 

.PHONY: cleanall cleanobj

all: $(TARGET)

ppmcvt: pbm.o pbm_aux.o ppmcvt.o

cleanobj:
	$(RM) pbm.o pbm_aux.o ppmcvt.o

cleanall: cleanobj
	$(RM) $(TARGET)
