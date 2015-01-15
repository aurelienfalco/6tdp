#include "util.h"

int BLOCK_SIZE = 64;

 int main(int argc, char* argv[])
 {
 	int i, j, k, l, loop, num_alive;
 	int ldboard, ldnbngb;
 	double t1, t2;
 	double temps;

 	int *board;
 	int *nbngb;

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

 	for (loop = 1; loop <= maxloop; loop++) {

 		cell(   0, 0   ) = cell(BS, BS);
 		cell(   0, BS+1) = cell(BS,  1);
 		cell(BS+1, 0   ) = cell( 1, BS);
 		cell(BS+1, BS+1) = cell( 1,  1);

 		for (i = 1; i <= BS; i++) {
 			cell(   i,    0) = cell( i, BS);
 			cell(   i, BS+1) = cell( i,  1);
 			cell(   0,    i) = cell(BS,  i);
 			cell(BS+1,    i) = cell( 1,  i);
 		}
#pragma omp parallel for private(i,j) collapse(2) 
 		for (l = 0; l < BS/BLOCK_SIZE; l++) {
 			for (k = 0; k < BS/BLOCK_SIZE; k++) {
 				for (j = 1; j <= BLOCK_SIZE; j++) {
 					for (i = 1; i <= BLOCK_SIZE; i++) {
 						int x = i + k * BLOCK_SIZE;
 						int y = j + l * BLOCK_SIZE;
 						ngb( x, y) =
 						cell( x-1, y-1 ) + cell( x, y-1 ) + cell( x+1, y-1 ) +
 						cell( x-1, y   ) +                  cell( x+1, y   ) +
 						cell( x-1, y+1 ) + cell( x, y+1 ) + cell( x+1, y+1 );
 					}
 				}
 			}
 		}
 				num_alive = 0;
#pragma omp parallel for private(i,j) reduction(+:num_alive)
 				for (j = 1; j <= BS; j++) {
 					for (i = 1; i <= BS; i++) {
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

		//output_board( BS, &(cell(1, 1)), ldboard, loop);
#ifdef PRINT_ALIVE
 			printf("%d \n", num_alive);
#endif
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

