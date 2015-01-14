CC = mpicc
EX = mpiexec

VERSION = seq omp omp-bloc pthread
# which version to run. Must be chosen from VERSION list and mpi
v = mpi

ALG = $(v)
SRC=$(ALG:=.c)
OBJ=$(SRC:.c=.o)
CFLAGS = -std=c99 -g -O0 -Wall -Wextra -fopenmp
n = 6
nx = 3
ny = 2
seq = 0

all: life_$(v)

life_%: life_%.c
	$(CC) $(CFLAGS) $^ -o $@

life_om%: life_om%.c
	$(CC) $(CFLAGS) -fopenmp $^ -o $@

life_thread: life_thread.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(VERSION):%: life_%
	./$^

mpi:%: life_%
	n=$(shell echo $(nx)\*$(ny) | bc); mpiexec -np $${n} $^ -x $(nx) -y $(ny)

exec: $(v)

qsub: $(v)
	rm -rf res.*
	@qsub batch; 
	@sleep 3; 
	@cat res.*

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -lm

tests: $(OBJ) tests.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

test: tests
	./tests $(N) $(e) $(p)

stat: $(v)
	./stats.sh

plot: 
	@gnuplot -e "name='$(stat)';output='$(stat).png" plot_fox.gp 
	@eog $(stat).png 2>/dev/null &

plot-sp: 
	@gnuplot -e "name='Speedup';data='speed.data';output='Speedup.png" plot_sp.gp 
	@eog Speedup.png 2>/dev/null &

clean:
	rm -rf *.o $(v) life_seq life_omp life_omp-bloc life_mpi *~

