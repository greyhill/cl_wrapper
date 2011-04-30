CLROOT=/usr/include/nvidia-current
CLWRAPPERROOT=../../
CXX=g++
CXXFLAGS=-g3 -Wall -Wextra -I${CLROOT} -I${CLWRAPPERROOT}

clc: clc.o
	${CXX} ${CXXFLAGS} -o $@ $^ -lOpenCL

clean:
	${RM} clc clc.o

