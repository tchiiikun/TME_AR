CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src
HOST=../hostfile

all: directories min_phase

directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}


# regles generales :
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< -I$(INC)

$(BIN)/% : $(OBJ)/%.o
	$(CC) -o $@ $<
#fin regles generales

#exo 1 :
min_phase: $(BIN)/min_phase
	mpirun -np 7 --hostfile $(HOST) $(BIN)/min_phase

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
