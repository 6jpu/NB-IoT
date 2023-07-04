CC=arm-linux-gnueabihf-gcc
AR=ar

all:  
	${CC} -c comport.c
	${CC} -c atcmd.c
	${CC} -c bc28.c
	${CC} -c nbiot.c
	${AR} -rcs libcomport.a comport.o
	${AR} -rcs libatcmd.a atcmd.o
	${AR} -rcs libbc28.a bc28.o
	${AR} -rcs libnbiot.a nbiot.o
	${CC} main.c -I `pwd` -L `pwd` -lnbiot -lbc28 -latcmd -lcomport -static 

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf a.out

distclean: clean
