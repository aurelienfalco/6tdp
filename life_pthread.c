#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "util.h"
#include "common.h"

pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barrier_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t alive_mutex = PTHREAD_MUTEX_INITIALIZER;
int barrier_count = 0;
int barrier_id = 0;
pthread_t *threads;
int nb_threads;
int threads_per_row;
int threads_per_col;
int col_block_size;
int row_block_size;

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
    int loop, i, j, thread_id = (int)param, count, local_alive;
	int istart = (thread_id / threads_per_row) * col_block_size + 1;
	int iend = ((thread_id / threads_per_row) + 1) * col_block_size;
	int jstart = (thread_id % threads_per_row) * row_block_size + 1;
	int jend = ((thread_id % threads_per_row) + 1) * row_block_size;
	int end_num = BS;
	
    for (loop = 1; loop <= maxloop; loop++) {
        // Chaque proc copie ses données dans les cellules fantomes associées à ses cellules
		if (istart == 1) {
			if (jstart == 1)
				cell(end_num+1, end_num+1) = cell(1, 1);
			else if (jend == end_num)
				cell(end_num+1, 0) = cell(1, end_num);
			for (j = jstart; j <= jend; j++) {
				cell(end_num+1, j) = cell(1, j);
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
		
		num_alive = 0;
		barrier();
		
		// MAJ
	    local_alive = 0;
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
					local_alive ++;
				}
			}
		}
		
		pthread_mutex_lock(&alive_mutex);
		num_alive += local_alive;
		pthread_mutex_unlock(&alive_mutex);
	}
	
	return NULL;
}

int main(int argc, char* argv[])
{
 	int i, j;
 	double t1, t2;
 	double temps;
 	
 	get_arg(argc,argv,NULL,NULL);
	init();
    
    threads_per_row = 2;
	nb_threads = n;
	threads_per_col = nb_threads / threads_per_row;
	
	if (print) 
    	output_board( BS, &(cell(1, 1)), ldboard, maxloop);
    
 	printf("Starting number of living cells = %d\n", num_alive);
 	t1 = mytimer();
	
    threads = malloc(nb_threads * sizeof(pthread_t));

    row_block_size = BS / threads_per_row;
	col_block_size = BS / threads_per_col;
	
    for (i = 0; i < nb_threads; i++) {
		pthread_create(&threads[i], NULL, start_work, (void*)i);
	}
	
	for (i = 0; i < nb_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	t2 = mytimer();
	temps = t2 - t1;
	printf("Final number of living cells = %d\n", num_alive);
	printf("time=%.2lf ms\n",(double)temps * 1.e3);

	if (print) {
    	output_board( BS, &(cell(1, 1)), ldboard, maxloop);
		printf("----------------------------\n");
    	output_board( BS+2, &(cell(0, 0)), ldboard, maxloop);
	}
	
	pthread_mutex_destroy(&barrier_mutex);
	pthread_mutex_destroy(&alive_mutex);
	pthread_cond_destroy(&barrier_cond);
	free(threads);
	free(board);
	free(nbngb);
	return EXIT_SUCCESS;
}

