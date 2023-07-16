LIBNAME1=gpiod
LIBNAME2=nbiot
LIBNAME3=bc28
LIBNAME4=atcmd
LIBNAME5=comport
LIBNAME6=sht20
LIBNAME7=adc_mq2
LIBNAME8=led
LIBNAME9=pwm_beep

LIBPATH1=`pwd`/inc
LIBPATH2=/usr/lib/arm-linux-gnueabihf
APPNAME=main
CFLAGS+=-I ${LIBPATH1}
LDFLAGS+=-L ${LIBPATH1}
LDFLAGS+=-l${LIBNAME2}
LDFLAGS+=-l${LIBNAME3}
LDFLAGS+=-l${LIBNAME4}
LDFLAGS+=-l${LIBNAME5}
LDFLAGS+=-l${LIBNAME6}
LDFLAGS+=-l${LIBNAME7}
LDFLAGS+=-l${LIBNAME8}
LDFLAGS+=-l${LIBNAME9}

CC=arm-linux-gnueabihf-gcc
AR=ar

all:  
	${CC} -c ${LIBNAME2}.c
	${CC} -c ${LIBNAME3}.c
	${CC} -c ${LIBNAME4}.c
	${CC} -c ${LIBNAME5}.c
	${CC} -c ${LIBNAME6}.c
	${CC} -c ${LIBNAME7}.c
	${CC} -c ${LIBNAME8}.c
	${CC} -c ${LIBNAME8}.c
	${CC} -c ${LIBNAME9}.c
	${AR} -rcs lib${LIBNAME2}.a ${LIBNAME2}.o
	${AR} -rcs lib${LIBNAME3}.a ${LIBNAME3}.o
	${AR} -rcs lib${LIBNAME4}.a ${LIBNAME4}.o
	${AR} -rcs lib${LIBNAME5}.a ${LIBNAME5}.o
	${AR} -rcs lib${LIBNAME6}.a ${LIBNAME6}.o
	${AR} -rcs lib${LIBNAME7}.a ${LIBNAME7}.o
	${AR} -rcs lib${LIBNAME8}.a ${LIBNAME8}.o
	${AR} -rcs lib${LIBNAME9}.a ${LIBNAME9}.o
	${CC} ${CFLAGS} main.c -o ${APPNAME} ${LDFLAGS} -L ${LIBPATH2} -l${LIBNAME1} -static 

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf a.out

distclean: clean
