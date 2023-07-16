APPNAME=main

all:
	@echo "Start compilation ..."
	make -C src
	@echo "Compilation completed!"
	mkdir Log

clean:
	make clean -C src
	
distclean: clean
	rm -rf ${APPNAME}
	rm -rf Log/
