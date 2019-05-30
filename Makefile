CC = g++-9 -std=c++11

OBJ = obj
SRC = src
APP = saApp

all: utils cleaner
	${CC} -g -o ${APP} -fopenmp -O3 ${SRC}/main.cpp ${OBJ}/utils.o ${OBJ}/simulatedAnnealing.o

utils:
	${CC} -g -fopenmp -c ${SRC}/utils.cpp
	${CC} -g -fopenmp -c ${SRC}/simulatedAnnealing.cpp

cleaner:
	mkdir -p ${OBJ}
	mv *.o ${OBJ}

clean:
	rm -R ${OBJ}
	rm ${APP}
	rm -R ${APP}.dSYM
	rm SOLV
