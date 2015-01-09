#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "mpi.h"
#include <getopt.h>

//#define PRINT_ALIVE
#define BS 4096
#define DIMENSION 2

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define local_cell( _i_, _j_ ) local_board[ row_block_size * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

double mytimer(void)
{
	struct timeval tp;
	gettimeofday( &tp, NULL );
	return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *board, int ldboard, int loop)
{
	int i,j;
	printf("loop %d\n", loop);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			if ( cell( i, j ) == 1)
				printf("X");
			else
				printf(" ");
		}
		printf("\n");
	}
}

int generate_initial_board(int N, int *board, int ldboard)
{
	int i, j, num_alive = 0;

	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			if (i == N/2 || j == N/2) {
				cell(i, j) = 1;
				num_alive ++;
			}
			else {
				cell(i, j) = 0;
			}
		}
	}

	return num_alive;
}

int main(int argc, char* argv[])
{
	int i, j, loop, num_alive, maxloop = 10;
	int ldboard, ldnbngb;
	double t1, t2;
	double temps;
	char c;

	int rank, nb_proc, nb_row, nb_col, grid_rank[DIMENSION] = {42, 42};
	MPI_Status status;
	MPI_Comm grid;
	MPI_Datatype blocktype, blocktype2;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);


	while ((c = getopt (argc, argv, "t:x:y:")) != -1){
		switch (c) {
			case 't':
			maxloop = atoi(optarg);
			break;
			case 'x':
			nb_row = atoi(optarg);
			break;
			case 'y':
			nb_col = atoi(optarg);
			break;
			default:
			return 1;
		}
	}

	// pour tester
	int dims[DIMENSION] = {nb_row, nb_col}, periods[DIMENSION] = {1, 0}, reorder = 1;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid);

	MPI_Cart_coords(grid, rank, DIMENSION, grid_rank);

	int *board, *local_board;
	int *nbngb;

	num_alive = 0;

    /* Leading dimension of the board array */
	ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
	ldnbngb = BS;

	board = malloc( ldboard * ldboard * sizeof(int) );
	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

	num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );

	int row_block_size = ldboard / nb_row;
	int col_block_size = ldboard / nb_col;
	local_board = malloc( row_block_size * col_block_size * sizeof(int) );

	MPI_Type_vector(col_block_size, row_block_size, ldboard, MPI_INT, &blocktype2);
	MPI_Type_create_resized( blocktype2, 0, sizeof(int), &blocktype);
	MPI_Type_commit(&blocktype);

	int disps[nb_row * nb_col];
	int counts[nb_row * nb_col];
	for (j=0; j<nb_col; j++) {
		for (i=0; i<nb_row; i++) {
			disps[i + nb_row*j] = i * ldboard * row_block_size + j * col_block_size;
			counts [i + nb_row*j] = 1;
		}
	}
	
	MPI_Scatterv(board, counts, disps, blocktype, local_board, row_block_size * col_block_size, MPI_INT, 0, MPI_COMM_WORLD);
    // output_board( row_block_size, &(cell(1, 1)), row_block_size, 0 );

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

	// #pragma omp parallel for private(i,j)
		for (j = 1; j <= BS; j++) {
			for (i = 1; i <= BS; i++) {
				ngb( i, j ) =
				cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
				cell( i-1, j   ) +                  cell( i+1, j   ) +
				cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
			}
		}

		num_alive = 0;
	// #pragma omp parallel for private(i,j) reduction(+:num_alive)
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
MPI_Finalize();
return EXIT_SUCCESS;
}

