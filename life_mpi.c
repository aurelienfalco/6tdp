#include "mpi.h"
#include "util.h"

#define DIMENSION 2

int main(int argc, char* argv[])
{
	int i, j, loop = 0, num_alive;
	int ldboard, ldnbngb;
	double t1, t2;
	double temps;
	int rank, nb_proc, nb_row, nb_col, grid_rank[DIMENSION] = {42, 42};

	MPI_Status status;
	MPI_Comm grid;
	MPI_Datatype blocktype, blocktype2;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	get_arg(argc,argv,&nb_row,&nb_col);
	printf("nb_row = %d, nb_col = %d\n",nb_row, nb_col );
	// Create processus grid
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

	// Create types for sending local boards
	MPI_Type_vector(col_block_size, row_block_size, ldboard, MPI_INT, &blocktype2);
	printf("Created type size %d x %d\n", row_block_size, col_block_size);
	MPI_Type_create_resized( blocktype2, 0, sizeof(int), &blocktype);
	MPI_Type_commit(&blocktype);

	int disps[nb_row * nb_col];
	int counts[nb_row * nb_col];
	for (j=0; j<nb_col; j++) {
		for (i=0; i<nb_row; i++) {
			disps[i* nb_col +j] = i * row_block_size  * ldboard+ j * col_block_size;
			printf("%d %d: %d\n",i,j,disps[i * nb_col+  j] );
			counts [i* nb_col +j] = 1;
		}
	}
	if (rank == 0)
		output_board( BS, &(cell(1, 1)), ldboard, 0);

	// Scattering board on grid
	MPI_Scatterv(board, counts, disps, blocktype, local_board, row_block_size * col_block_size, MPI_INT, 0, MPI_COMM_WORLD);

	if (grid_rank[1] == 1)
		local_cell(4, 3) = 1;


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

		// exchange cells with proc

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
				if ( (ngb( i, j ) < 2) || (ngb( i, j ) > 3) ) {
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

	// MPI_Reduce(num_alive);
	MPI_Gatherv(local_board,  row_block_size * col_block_size, MPI_INT, board, counts, disps, blocktype, 0, MPI_COMM_WORLD);
	if(rank == 0){
		output_board( BS, &(cell(1, 1)), ldboard, loop);
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

