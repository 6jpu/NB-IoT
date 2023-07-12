CC=arm-linux-gnueabihf-gcc
AR=ar

all:  
	${CC} -c comport.c
	${CC} -c atcmd.c
	${CC} -c bc28.c
	${CC} -c nbiot.c
	${CC} -c sht20.c
	${CC} -c adc_mq2.c
	${AR} -rcs libcomport.a comport.o
	${AR} -rcs libatcmd.a atcmd.o
	${AR} -rcs libbc28.a bc28.o
	${AR} -rcs libnbiot.a nbiot.o
	${AR} -rcs libsht20.a sht20.o
	${AR} -rcs libadc_mq2.a adc_mq2.o
	${CC} main.c -I `pwd` -L `pwd` -lnbiot -lbc28 -latcmd -lcomport -lsht20 -ladc_mq2 -static 

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf a.out

distclean: clean
