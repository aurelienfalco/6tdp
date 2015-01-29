CC = mpicc
EX = mpiexec

VERSION = seq omp omp-bloc # pthread
# which version to run. Must be chosen from VERSION list and mpi
MPI_VERSION = mpi-synchrone mpi-asynchrone mpi-persistant
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
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(VERSION):%: life_%
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

stat: $(addprefix stat-,$(VERSION) $(MPI_VERSION))

plot: $(addsuffix .data,$(VERSION) $(MPI_VERSION) )
	@gnuplot -e "xname='Nombre de processus';name1='OpenMP';data1='seq.data';name2='pthread';data2='pthread.data';name3='MPI synchrone';data3='mpi-synchrone.data';name4='MPI asynchrone';data4='mpi-asynchrone.data';name5='MPI persistant';data5='mpi-persistant.data';output='Speedup.png" plot_sp.gp 
	@eog Speedup.png 2>/dev/null &

clean:
	rm -rf *.o $(v) $(addprefix life_,$(VERSION) $(MPI_VERSION)) *~ *.png $(addsuffix .data,$(VERSION) $(MPI_VERSION))

