CXX = g++-10
CXXFLAGS = -std=c++17 -Wall -MMD -g -I../include -O2 -pthread
RM = rm -rf
MAIN = unittest

SRC = $(shell cat source.list)
OBJ = ${SRC:.cpp=.o}
DEP = ${SRC:.cpp=.d}

all: ${MAIN}
	@echo running unit test
	./unittest

${MAIN}: ${OBJ}
	${CXX} ${OBJ} ${CXXFLAGS} -o ${MAIN}


.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} ${MAIN}

-include ${DEP}
