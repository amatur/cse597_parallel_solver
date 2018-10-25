# Makefile for parallelization of jacobi

CXX=g++
CCFLAGS=-Wall -O3


all: test

run: 
	echo "TESTING";\
	./test

jacobi: jacobi.o
	$(CXX) $(CCFLAGS) -o $@ $^
	
lu: lu.o
	$(CXX) $(CCFLAGS) -o $@ $^
	
test: test.o jacobi.o lu.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

%.o: %.cpp %.h
	$(CXX) $(CCFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $<
	
clean:
	rm -f *.o jacobi lu test
