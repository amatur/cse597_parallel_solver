# Makefile for parallelization of jacobi
# to use this, modules:
# module use /storage/work/a/awl5173/toShare/tauPdt/tau
# module load adamsTau_2.27 

#CXX=mpic++

CXX=mpic++
#CC=mpic++
#CXX=tau_cxx.sh
#CC=tau_cc.sh
CCFLAGS=-g -Wall -O3 
#CXX=g++
#~ F90= tau_f90.sh
#~ CXX=tau_cxx.sh
#~ CC=tau_cc.sh
#~ CCFLAGS=-Wall -O3


all: jacobi jacobi_s

data:	
	echo -e '"Dimension"' " " '"Time (sec)"' ;
	for i in 5 10 20 30 40 50 ;	\
	do	\
	 ./jacobi_s $$i; \
	done

run: 
	echo "### TESTING WITH 4 PROCESSES ###"; mpirun -np 4 ./jacobi 30 \
	echo "RUN SERIAL"; ./jacobi_s 30 \	
#~ run: 
#~ 	echo "TESTING";\
#~ 	./test

jacobi: jacobi.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

jacobi_s: jacobi_s.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

jacobi_s.o: jacobi_s.cpp jacobi.h
	$(CXX) $(CCFLAGS) -c $<
	
lu: lu.o
	$(CXX) $(CCFLAGS) -o $@ $^
	
test: test.o jacobi.o lu.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

%.o: %.cpp %.h
	$(CXX) $(CCFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $<

clean:
	rm -f *.o jacobi lu test jacobi_s
