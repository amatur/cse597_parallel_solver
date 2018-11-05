# Makefile for parallelization of jacobi
# to use this, modules:
# module use /storage/work/a/awl5173/toShare/tauPdt/tau
# module load adamsTau_2.27 

#CXX=mpic++
#CC=mpic++

#CXX=mpic++ -fopenmp
CXX=tau_cxx.sh
#CC=tau_cc.sh

LIBS=-lm

CCFLAGS= -Wall -g -O3 
#CXX=g++
#~ F90= tau_f90.sh
#~ CXX=tau_cxx.sh
#~ CC=tau_cc.sh
#~ CCFLAGS=-Wall -O3


#all: jacobi jacobip2
#all: jacobi_s_opt jacobi_s
#all: jacobi_s
all: jacobi

data:	
	echo -e '"Dimension"' " " '"Time (sec)"' ;
	for i in 5 10 20 30 40 50 ;	\
	do	\
	 ./jacobi_s $$i; \
	done

data_parallel:	
	for i in 10;\
	do	\
		mpirun -np $$i ./jacobi 10; \
	done
data_so:	
	echo -e '"Dimension"' " " '"Time (sec)"' ;
	for i in 5 10 20 30 40 50 ;	\
	do	\
	 ./jacobi_s_opt $$i; \
	done
run: 
	#echo "## v2"; mpirun -np 20 ./jacobip2 1600 ; 
	echo 20 \
	echo "### TESTING WITH 4 PROCESSES ###"; mpirun -np 40 ./jacobi 80 \
	#echo "RUN SERIAL"; ./jacobi_s 40 \	

#~ run: 
#~ 	echo "TESTING";\
#~ 	./test

jacobip2: jacobip2.o
	$(CXX) $(CCFLAGS) -o $@ $^ $(LIBS)


jacobi: jacobi.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^ $(LIBS)

jacobi_s: jacobi_s.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

jacobi_s.o: jacobi_s.cpp jacobi.h
	$(CXX) $(CCFLAGS) -c $<

jacobi_s_opt: jacobi_s_opt.o matrix_util.o
	$(CXX) $(CCFLAGS) -o $@ $^

jacobi_s_opt.o: jacobi_s_opt.cpp jacobi.h
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
	rm -f *.o jacobi lu test jacobi_s jacobi_s_opt
