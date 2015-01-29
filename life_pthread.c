#include <pthread.h>
#include <semaphore.h>

#include "util.h"
#include "common.h"

pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barrier_cond = PTHREAD_COND_INITIALIZER;
int barrier_count = 0;
int barrier_id = 0;
sem_t *sems_neig;
sem_t *sems_majs;
pthread_t *threads;
int nb_threads;
int threads_per_side;
int block_size;

void barrier() {
    int id;
	
    pthread_mutex_lock(&barrier_mutex);
    id = barrier_id;
    barrier_count++;
    if (barrier_count == nb_threads) {
        barrier_count = 0;
        barrier_id++;
        pthread_cond_broadcast(&barrier_cond);
    }
	
    while (id == barrier_id)
        pthread_cond_wait(&barrier_cond, &barrier_mutex);
    pthread_mutex_unlock(&barrier_mutex);
}

void* start_work(void *param) {
    int loop, i, j, num_alive, thread_id = (int)param, count;
	int istart = (thread_id / threads_per_side) * block_size + 1;
	int iend = ((thread_id / threads_per_side) + 1) * block_size;
	int jstart = (thread_id % threads_per_side) * block_size + 1;
	int jend = ((thread_id % threads_per_side) + 1) * block_size;
	int end_num = threads_per_side * block_size;
	
	if (thread_id == 1) {
		printf("block_size=%d\n", block_size);
		printf("proc=%d\n", thread_id);
		printf("maxloop=%d\n", maxloop);
		printf("istart=%d\n", istart);
		printf("iend=%d\n", iend);
		printf("jstart=%d\n", jstart);
		printf("jend=%d\n", jend);
		printf("end_num=%d\n", end_num);
	}
	
    for (loop = 1; loop <= maxloop; loop++) {
        // Chaque proc copie ses données dans les cellules fantomes associées à ses cellules
		if (istart == 1) {
			if (jstart == 1)
				cell(end_num+1, end_num+1) = cell(1, 1);
			else if (jend == end_num)
				cell(end_num+1, 0) = cell(1, end_num);
			for (j = jstart; j <= jend; j++) {
				cell(end_num+1, j) = cell(1, j);
				printf("(%d,%d)", 1, j);
			}
		}
		else if (iend == end_num) {
			if (jstart == 1)
				cell(0, end_num+1) = cell(end_num, 1);
			else if (jend == end_num)
				cell(0, 0) = cell(end_num, end_num);
			for (j = jstart; j <= jend; j++) {
				cell(0, j) = cell(end_num, j);
			}
		}
		if (jstart == 1) {
			for (i = istart; i <= iend; i++)
				cell(i, end_num+1) = cell(i, 1);
		}
		else if (jend == end_num) {
			for (i = istart; i <= iend; i++)
				cell(i, 0) = cell(i, end_num);
		}		
		
		/*for (i = 0; i < 3; i++) {
			int proc = thread_id;
			if (i == 0)
				proc = (proc % threads_per_side == 0?proc + threads_per_side - 1:proc-1);
			else if (i == 2)
				proc = ((proc+1)  % threads_per_side == 0?proc - threads_per_side + 1:proc+1);
			//if (thread_id == 0)
				printf("thread_id=%d -- i=%d -- proc=%d\n", thread_id, i, proc);
			for (j = 0; j < 3; j++) {
				if (!(i == 1 && j == 1)) {
					if (j == 0)
						proc = (proc / threads_per_side == 0?proc + threads_per_side*(threads_per_side-1):proc-threads_per_side);
					else if (j == 2)
						proc = (proc % threads_per_side == (threads_per_side-1)?(proc + threads_per_side) % threads_per_side:proc+threads_per_side);
					if (thread_id == 0) {
						printf("(%d,%d) => %d\n", i, j, proc);
					}
					sem_post(&sems_majs[proc]);
				}
			}
		}		
		
		count = 8;
		while (count) {
			sem_wait(&sems_majs[thread_id]);
			count--;
		}*/
		
		barrier();
		
		if (thread_id > 0) {
			for (i=0; i<=BS+2; i++) {
				for (j=0; j<=BS+2; j++) {
					printf("%d", cell(i, j));
				}
				printf("\n");
			}
			printf("------------\n");
		}
		
		barrier();
		
		// Calcul des voisins
 		for (j = jstart; j <= jend; j++) {
 			for (i = istart; i <= iend; i++) {
 				ngb( i, j ) =
					cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
					cell( i-1, j   ) +                  cell( i+1, j   ) +
					cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
 			}
 		}
		
		/*for (i = 0; i < 3; i++) {
			int proc = thread_id;
			if (i == 0)
				proc = (proc % threads_per_side == 0?proc + threads_per_side - 1:proc-1);
			else if (i == 2)
				proc = ((proc+1)  % threads_per_side == 0?proc - threads_per_side + 1:proc+1);
			for (j = 0; j <= 3; j++) {
				if (!(i == 1 && j == 1)) {
					if (j == 0)
						proc = (proc / threads_per_side == 0?proc + threads_per_side*(threads_per_side-1):proc-threads_per_side);
					else if (j == 2)
						proc = (proc % threads_per_side == (threads_per_side-1)?(proc + threads_per_side) % threads_per_side:proc+threads_per_side);
					sem_post(&sems_neig[proc]);
				}
			}
		}		
		
		count = 8;
		while (count) {
			sem_wait(&sems_neig[thread_id]);
			count--;
			}*/
		
		barrier();
		
		// MAJ
		num_alive = 0;
 		for (j = jstart; j <= jend; j++) {
 			for (i = istart; i <= iend; i++) {
 				if ( (ngb( i, j ) < 2) || 
					 (ngb( i, j ) > 3) ) {
 					cell(i, j) = 0;
				}
				else {
					if ((ngb( i, j )) == 3)
						cell(i, j) = 1;
				}
				if (cell(i, j) == 1) {
					num_alive ++;
				}
			}
		}
	}

	return NULL;
}

int main(int argc, char* argv[])
{
 	int i, j;
 	double t1, t2;
 	double temps;
    
    threads_per_side = 2;
	nb_threads = threads_per_side * threads_per_side;
 	
 	get_arg(argc,argv,NULL,NULL);
	init();
	
	if (print) 
    	output_board( BS, &(cell(1, 1)), ldboard, maxloop);
    
 	printf("Starting number of living cells = %d\n", num_alive);
 	t1 = mytimer();
	
    threads = malloc(nb_threads * sizeof(pthread_t));
    sems_neig = malloc(nb_threads * sizeof(sem_t));
	sems_majs = malloc(nb_threads * sizeof(sem_t));

    block_size = BS / threads_per_side;
	
    for (i = 0; i < nb_threads; i++) {
        if (sem_init(&sems_majs[i], 0, 0)) {
			fprintf(stderr, "cannot create semaphore\n");
			return -1;
		}
        if (sem_init(&sems_neig[i], 0, 0)) {
			fprintf(stderr, "cannot create semaphore\n");
			return -1;
		}
		pthread_create(&threads[i], NULL, start_work, (void*)i);
	}
	
	for (i = 0; i < nb_threads; i++) {
		pthread_join(threads[i], NULL);
		sem_destroy(&sems_neig[i]);
		sem_destroy(&sems_majs[i]);
	}

	t2 = mytimer();
	temps = t2 - t1;
	printf("Final number of living cells = %d\n", num_alive);
	printf("time=%.2lf ms\n",(double)temps * 1.e3);

	for (i=1; i<=BS; i++) {
		for (j=1; j<=BS; j++) {
				printf("%d", ngb(i, j));
		}
		printf("\n");
	}

	if (print) {
    	output_board( BS, &(cell(1, 1)), ldboard, maxloop);
		printf("----------------------------\n");
    	output_board( BS+2, &(cell(0, 0)), ldboard, maxloop);
	}

	free(board);
	free(nbngb);
	return EXIT_SUCCESS;
}

