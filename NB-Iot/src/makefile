LIBNAME1=gpiod
LIBNAME2=nbiot
LIBNAME3=bc28
LIBNAME4=atcmd
LIBNAME5=comport
LIBNAME6=sht20
LIBNAME7=adc_mq2
LIBNAME8=led
LIBNAME9=pwm_beep
LIBNAME10=control
LIBNAME11=logger
LIBNAME12=data_check
LIBNAME13=mysqlite


INC_PATH1=`pwd`/../inc
INC_PATH2=/usr/local/include
LIBPATH1=`pwd`
LIBPATH2=/usr/lib/arm-linux-gnueabihf
APPNAME=main

CFLAGS+=-I ${INC_PATH1}
CFLAGS+=-I ${INC_PATH2}

LDFLAGS+=-L ${LIBPATH1}
LDFLAGS+=-l${LIBNAME2}
LDFLAGS+=-l${LIBNAME3}
LDFLAGS+=-l${LIBNAME4}
LDFLAGS+=-l${LIBNAME5}
LDFLAGS+=-l${LIBNAME6}
LDFLAGS+=-l${LIBNAME7}
LDFLAGS+=-l${LIBNAME8}
LDFLAGS+=-l${LIBNAME9}
LDFLAGS+=-l${LIBNAME10}
LDFLAGS+=-l${LIBNAME11}
LDFLAGS+=-l${LIBNAME12}
LDFLAGS+=-l${LIBNAME13}

CC=arm-linux-gnueabihf-gcc
AR=ar

all: compile
	@make install
	@make clean

compile:
	${CC} ${CFLAGS} -c *.c
	${AR} -rcs lib${LIBNAME2}.a ${LIBNAME2}.o
	${AR} -rcs lib${LIBNAME3}.a ${LIBNAME3}.o
	${AR} -rcs lib${LIBNAME4}.a ${LIBNAME4}.o
	${AR} -rcs lib${LIBNAME5}.a ${LIBNAME5}.o
	${AR} -rcs lib${LIBNAME6}.a ${LIBNAME6}.o
	${AR} -rcs lib${LIBNAME7}.a ${LIBNAME7}.o
	${AR} -rcs lib${LIBNAME8}.a ${LIBNAME8}.o
	${AR} -rcs lib${LIBNAME9}.a ${LIBNAME9}.o
	${AR} -rcs lib${LIBNAME10}.a ${LIBNAME10}.o
	${AR} -rcs lib${LIBNAME11}.a ${LIBNAME11}.o
	${AR} -rcs lib${LIBNAME12}.a ${LIBNAME12}.o
	${AR} -rcs lib${LIBNAME13}.a ${LIBNAME13}.o
	${CC} ${CFLAGS} main.c -o ${APPNAME} ${LDFLAGS} -L ${LIBPATH2} -l${LIBNAME1} -lsqlite3-arm -static 

install:
	mv ${APPNAME} `pwd`/../

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf a.out

distclean: clean
