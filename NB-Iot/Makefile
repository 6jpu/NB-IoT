#仅供下载至开发板时使用

APPNAME=smoke

all:
	rm -rf ${APPNAME}
	rm -rf Log/*
	rz
	chmod a+x ${APPNAME}
	./${APPNAME}

init:
	#BC28 硬件复位
	gpioset 0 15=1
	#关闭硬件的初始化LED
	echo 10 > /sys/class/gpio/export 
	echo out > /sys/class/gpio/gpio10/direction 
	echo 1 > /sys/class/gpio/gpio10/value
	echo 10 > /sys/class/gpio/unexport
	gpioset 0 15=0

close:
	#手动关闭蜂鸣器，警报灯
	echo 0 >/sys/class/pwm/pwmchip1/pwm0/enable
	echo 11 >/sys/class/gpio/export
	echo 1 >/sys/class/gpio/gpio11/value
	echo 11 >/sys/class/gpio/export

clean:


distclean: clean
	rm -rf Log/
