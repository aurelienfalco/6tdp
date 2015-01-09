CC = mpicc
EX = mpiexec
EXEC = main.c
ALG = main.c
SRC=$(ALG:=.c)
OBJ=$(SRC:.c=.o)
VERSION= seq omp omp-bloc pthread
CFLAGS = -std=c99 -g -O0 -Wall -Wextra
n = 2
m = 6
seq = 0

all: $(EXEC)

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

exec: $(EXEC)
	@$(EX) -np $(n) $(EXEC) -t $(t) -x $(nx) -y $(ny)

qsub: $(EXEC)
	rm -rf res.*
	@qsub batch; 
	@sleep 3; 
	@cat res.*

$(EXEC): $(OBJ) main.c
	$(CC) $(CFLAGS) $^ -o $@ -lm

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -lm

tests: $(OBJ) tests.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

test: tests
	./tests $(N) $(e) $(p)

stat: $(EXEC)
	./stats.sh

plot: 
	@gnuplot -e "name='$(stat)';output='$(stat).png" plot_fox.gp 
	@eog $(stat).png 2>/dev/null &

plot-sp: 
	@gnuplot -e "name='Speedup';data='speed.data';output='Speedup.png" plot_sp.gp 
	@eog Speedup.png 2>/dev/null &

clean:
	rm -rf *.o $(EXEC) life_seq life_omp life_omp-bloc life_mpi *~

