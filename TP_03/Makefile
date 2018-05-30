CC=gcc -Wall -ansi
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src
HOST=../hostfile

all: directories min_abr min_echo

directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}

# regles generales :
$(OBJ)/%.o: $(SRC)/%.c
	mpicc -c -o $@ $< -I$(INC) 
$(BIN)/% : $(OBJ)/%.o
	mpicc -o $@ $<
	#fin regles generales
#exo 1 :
min_abr: $(BIN)/min_abr
	mpirun -np 7 --hostfile $(HOST) $(BIN)/min_abr
#exo 2 :
min_echo: $(BIN)/min_echo
	mpirun -np 7 --hostfile $(HOST) $(BIN)/min_echo
clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*
cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
