#include "util.h"
#include <pthread.h>
#include <semaphore.h>

struct Param {
	int thread_id;
};

sem_t *sems;
pthread_t *threads;
int *board;
int *nbngb;
int nb_threads;
int ldboard, ldnbngb;
int block_size;

void* start_work(void *param) {
	int loop, i, j, num_alive, thread_id = (int)param;
	int istart = (thread_id / nb_threads) * block_size + 1;
	int iend = ((thread_id / nb_threads) + 1) * block_size;
	int jstart = (thread_id % nb_threads) * block_size + 1;
	int jend = ((thread_id % nb_threads) + 1) * block_size;
	
	if (thread_id == 0) {
		printf("proc=%d\n", thread_id);
		printf("maxloop=%d\n", maxloop);
		printf("istart=%d\n", istart);
		printf("iend=%d\n", iend);
		printf("jstart=%d\n", jstart);
		printf("jend=%d\n", jend);
	}
	
 	for (loop = 1; loop <= maxloop; loop++) {
		// copie des cellules dans les coins
 		cell(   0, 0   ) = cell(nb_threads*block_size, nb_threads*block_size);
 		cell(   0, nb_threads*block_size+1) = cell(nb_threads*block_size,  1);
 		cell(nb_threads*block_size+1, 0   ) = cell( 1, nb_threads*block_size);
 		cell(nb_threads*block_size+1, nb_threads*block_size+1) = cell( 1,  1);
		
		// copie des cellules des côtés
 		for (i = istart; i <= iend; i++) {
 			cell(   i,    istart) = cell( i, BS);
 			cell(   i,    iend+1) = cell( i,  1);
 			cell(   istart,    i) = cell(BS,  i);
 			cell(iend+1,       i) = cell( 1,  i);
 		}

 		for (j = jstart; j <= jend; j++) {
 			for (i = istart; i <= iend; i++) {
 				ngb( i, j ) =
					cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
					cell( i-1, j   ) +                  cell( i+1, j   ) +
					cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
 			}
 		}

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
 	int i, num_alive;
 	double t1, t2;
 	double temps;
	
	nb_threads = 4;
 	
 	get_arg(argc,argv,NULL,NULL);
 	num_alive = 0;

    /* Leading dimension of the board array */
 	ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
 	ldnbngb = BS;
	
 	board = malloc( ldboard * ldboard * sizeof(int) );
 	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );
	
 	num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );
	
    //output_board( BS, &(cell(1, 1)), ldboard, 0 );
	
 	printf("Starting number of living cells = %d\n", num_alive);
 	t1 = mytimer();
	
	threads = malloc(nb_threads * sizeof(pthread_t));
	sems = malloc(nb_threads * sizeof(sem_t));
	for (i = 0; i < nb_threads; i++) {
		if (sem_init(&sems[i], 0, 0)) {
			fprintf(stderr, "cannot create semaphore\n");
			return -1;
		}
		pthread_create(&threads[i], NULL, start_work, (void*)i);
	}
	
	for (i = 0; i < nb_threads; i++) {
		pthread_join(threads[i], NULL);
		sem_destroy(&sems[i]);
	}

	t2 = mytimer();
	temps = t2 - t1;
	printf("Final number of living cells = %d\n", num_alive);
	printf("time=%.2lf ms\n",(double)temps * 1.e3);

    //output_board( BS, &(cell(1, 1)), ldboard, maxloop);

	free(board);
	free(nbngb);
	return EXIT_SUCCESS;
}

