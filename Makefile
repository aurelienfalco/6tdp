CC = mpicc
EX = mpiexec

VERSION = seq omp omp-bloc pthread
# which version to run. Must be chosen from VERSION list and mpi
v = mpi

INC = util common
OBJ = $(INC:=.o)
CFLAGS = -std=c99 -g -O0 -Wall -Wextra
n = 6
nr = 2
nc = 3
t = 10
s = 4096
p = 0

all: life_$(v)

life_om%: life_om%.c $(OBJ)
	$(CC) $(CFLAGS) -fopenmp $^ -o $@

life_%: life_%.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

life_pthread:%: %.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(VERSION):%: life_%
	./$< -t $(t) -s $(s) -p $(p)

mpi:%: life_%
	n=$(shell echo $(nr)\*$(nc) | bc); mpiexec -np $${n} $< -t $(t) -r $(nr) -c $(nc) -s $(s) -p $(p)

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
	rm -rf *.o $(v) $(addprefix life_,$(VERSION) mpi) *~

