CC = mpicc
EX = mpiexec

SEQ = seq
THREAD =  omp omp-bloc pthread
NO_MPI = $(SEQ) $(THREAD)
# which version to run. Must be chosen from VERSION list and mpi
MPI_VERSION = mpi-synchrone mpi-asynchrone mpi-persistant
PARALLEL = $(THREAD) $(MPI_VERSION)
ALL = $(SEQ) $(PARALLEL)
v = mpi-asynchrone

INC = util common
OBJ = $(INC:=.o)
CFLAGS = -std=c99 -g -O0 -Wall -Wextra
n = 6
nr = 2
nc = 3
t = 10
s = 3600
p = 0

all: life_$(v)

life_om%: life_om%.c $(OBJ)
	$(CC) $(CFLAGS) -fopenmp $^ -o $@

life_%: life_%.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

life_pthread:%: %.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm -lpthread

$(NO_MPI):%: life_%
	./$< -t $(t) -s $(s) -p $(p) -n $(n)

$(MPI_VERSION):%: life_%
	n=$(shell echo $(nr)\*$(nc) | bc); mpiexec -np $${n} $< -t $(t) -r $(nr) -c $(nc) -s $(s) -p $(p)

exec: $(v)

qsub: $(v)
	rm -rf res.*
	@qsub batch; 
	@sleep 3; 
	@cat res.*

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -lm

%.data: stat-%

stat-%:
	./stats.sh $(subst stat-,,$@)

stat: $(addprefix stat-,$(PARALLEL))

plot-mpi: $(addsuffix .data,$(MPI_VERSION))
	image=speedup-mpi
	@gnuplot -e "xname='Nombre de processus';name1='MPI synchrone';data1='mpi-synchrone.data';name2='MPI asynchrone';data2='mpi-asynchrone.data';name3='MPI persistant';data3='mpi-persistant.data';output='$(image).png" plot_sp.gp 
	@eog $(image).png 2>/dev/null &

plot-thread: $(addsuffix .data,$(THREAD))
	image=speedup-thread
	@gnuplot -e "xname='Nombre de threads';name1='OpenMP';data1='omp.data';name2='pthread';data2='pthread.data';name3='OpenMP bloc';data3='omp-bloc.data';output='$(image).png" plot_sp.gp 
	@eog $(image).png 2>/dev/null &


clean:
	rm -rf *.o $(v) $(addprefix life_,$(PARALLEL)) *~ *.png $(addsuffix .data,$(PARALLEL))

