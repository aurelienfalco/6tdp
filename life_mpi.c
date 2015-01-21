#include "mpi.h"
#include "util.h"

#define DIMENSION 2

int main(int argc, char* argv[])
{
	int i, j, loop = 0, num_alive;
	int ldboard, ldnbngb;
	int *board, *tmp_board, *local_board, *local_ngb;
	int *nbngb;
	double t1, t2;
	double temps;
	int rank, nb_proc, nb_col, nb_row, grid_rank[DIMENSION] = {42, 42};
	MPI_Status status;
	MPI_Comm grid;
	MPI_Datatype blocktype, blocktype2, row_type2, row_type;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	get_arg(argc,argv,&nb_row,&nb_col);
	printf("nb_col = %d, nb_row = %d\n",nb_col, nb_row );
	// Create processus grid
	int dims[DIMENSION] = {nb_col, nb_row}, periods[DIMENSION] = {1, 1}, reorder = 1;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid);
	MPI_Cart_coords(grid, rank, DIMENSION, grid_rank);

	num_alive = 0;
    /* Leading dimension of the board array */
	ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
	ldnbngb = BS;

	board = malloc( ldboard * ldboard * sizeof(int) );
	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

	// num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );
	for (i = 0; i < BS; i++) {
		cell(1, i) = 1;
		cell(i, 1) = 1;
		cell(BS-1, i) = 1;
		cell(i, BS-1) = 1;
	}

	int col_block_size = ldnbngb / nb_col;
	int row_block_size = ldnbngb / nb_row;
	int local_ld = row_block_size + 2;

	tmp_board = malloc( col_block_size * row_block_size * sizeof(int) );
	local_ngb = malloc( col_block_size * row_block_size * sizeof(int) );
	local_board = malloc( (col_block_size + 2)  * (row_block_size + 2) * sizeof(int) );

	// Create types for sending local boards
	MPI_Type_vector(col_block_size, row_block_size, ldboard, MPI_INT, &blocktype2);
	printf("Created type size %d x %d\n", row_block_size, col_block_size);
	MPI_Type_create_resized( blocktype2, 0, sizeof(int), &blocktype);
	MPI_Type_commit(&blocktype);

	MPI_Type_vector(col_block_size, 1, local_ld, MPI_INT, &row_type2);
	MPI_Type_create_resized( row_type2, 0, sizeof(int), &row_type);
	MPI_Type_commit(&row_type);

	int disps[nb_col * nb_row];
	int counts[nb_col * nb_row];
	for (j=0; j<nb_col; j++) {
		for (i=0; i<nb_row; i++) {
			disps[i + nb_row * j] = i * row_block_size + j * col_block_size * ldboard;
			counts [i + nb_row * j] = 1;
		}
	}
	// print board before scatter
	if (rank == 0)
		output_board( BS, &(cell(1, 1)), ldboard, 0);

	// Scattering board on grid
	MPI_Scatterv(board + ldboard + 1, counts, disps, blocktype, tmp_board, col_block_size * row_block_size, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < col_block_size; ++i)
	{
		memcpy(local_board + local_ld * (i+1) + 1, tmp_board + row_block_size * i, row_block_size* sizeof(int));
	}
	// if (grid_rank[1] == 0){
	for (int i = 0; i < 4; ++i)
	{
		// cell_ld(local_board, 1, i, local_ld) = 1;
		// cell_ld(local_board, row_block_size-1, i, local_ld) = 1;
	}
	cell_ld(local_board, row_block_size/2, col_block_size/2, local_ld) = 1;
	if (grid_rank[0] == 1 && grid_rank[1] == 1)
		output_block(row_block_size, col_block_size, local_board, local_ld, 0);
	// }


	printf("Starting number of living cells = %d\n", num_alive);
	t1 = mytimer();

	for (loop = 1; loop <= maxloop; loop++) {
	/*
		MPI_Send(local_board+1, 1, row_type, grid_rank[1]-1, 99, MPI_COMM_WORLD);
		MPI_Send(local_board+ldboard+1, 1, row_type, grid_rank[1]+1, 99, MPI_COMM_WORLD);
		MPI_Send(local_board, local_ld, MPI_INT, grid_rank[0]+1, 99, MPI_COMM_WORLD);
		cell_ld(   0, 0   ) = cell_ld(row_block_size, col_block_size);
		cell_ld(   0, col_block_size+1) = cell_ld(BS,  1);
		cell_ld(row_block_size+1, 0   ) = cell_ld( 1, col_block_size);
		cell_ld(row_block_size+1, col_block_size+1) = cell_ld( 1,  1);

		for (i = 1; i <= ; i++) {
			cell_ld(   i,    0) = cell_ld( i, col_block_size);
			cell_ld(   i, col_block_size+1) = cell_ld( i,  1);
			cell_ld(   0,    i) = cell_ld(row_block_size,  i);
			cell_ld(row_block_size+1,    i) = cell_ld( 1,  i);
		}
		// exchange cell_lds with proc

		for (j = 1; j <= row_block_size; j++) {
			for (i = 1; i <= col_block_size; i++) {
				ngb_ld(local_ngb, i, j, row_block_size ) =
				cell_ld(local_board, i-1, j-1, local_ld ) + cell_ld(local_board, i, j-1, local_ld ) + cell_ld(local_board, i+1, j-1, local_ld ) +
				cell_ld(local_board, i-1, j, local_ld ) + cell_ld(local_board, i+1, j, local_ld ) +
				cell_ld(local_board, i-1, j+1, local_ld ) + cell_ld(local_board, i, j+1, local_ld ) + cell_ld(local_board, i+1, j+1, local_ld );
			}
		}

		num_alive = 0;
	// #pragma omp parallel for private(i,j) reduction(+:num_alive)
		for (j = 1; j <= row_block_size; j++) {
			for (i = 1; i <= col_block_size; i++) {
				if ( (ngb_ld(local_ngb, i, j, row_block_size ) < 2) || (ngb_ld(local_ngb, i, j, row_block_size ) > 3) ) {
					cell_ld(local_board,i, j, local_ld) = 0;
				}
				else {
					if ((ngb_ld(local_ngb, i, j, row_block_size )) == 3)
						cell_ld(local_board,i, j, local_ld) = 1;
				}
				if (cell_ld(local_board,i, j, local_ld) == 1) {
					num_alive ++;
				}
			}
		}
/**/
	}

	for (int i = 0; i < col_block_size; ++i)
	{
		memcpy(tmp_board + row_block_size * i, local_board + local_ld * (i+1) + 1, row_block_size* sizeof(int));
	}

	// MPI_Reduce(num_alive);
	MPI_Gatherv(tmp_board,  col_block_size * row_block_size, MPI_INT, board + ldboard + 1, counts, disps, blocktype, 0, MPI_COMM_WORLD);
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

