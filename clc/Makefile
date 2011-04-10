CLROOT=/usr/include/nvidia-current
CLWRAPPERROOT=../../
CXX=clang++
CXXFLAGS=-g3 -Wall -Wextra -I${CLROOT} -I${CLWRAPPERROOT} -lOpenCL

clc: clc.o
	${CXX} ${CXXFLAGS} -o $@ $^

