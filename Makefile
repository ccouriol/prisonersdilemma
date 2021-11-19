#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# MAKE_CLIENT := src/client/Makefile

# MAKE_SERVER := src/server/Makefile

# define the documentation program to use
DOCC = doxygen

# define docs directory
DOCS	:= docs/html

# define output directory
OUTPUT	:= output

# define source directory
SRC		:= src

# define utils commands
RM = rm -f
RMD = rm -rf
MD	:= mkdir -p

.PHONY: clean
clean:
	$(RMD) $(DOCS)
	@echo Cleanup complete!

runserver: 
	./output/server
	
runclient: 
	./output/client

client:
	cd ./src/client/ && make && cd ../../

server:
	cd ./src/server/ && make && cd ../../

all: client server

cleanclient:
	cd ./src/client/ && make clean && cd ../../

cleanserver:
	cd ./src/server/ && make clean && cd ../../

cleanall: cleanclient cleanserver

doc:
	$(DOCC) dconfig