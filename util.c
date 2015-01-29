#include "util.h"

int maxloop = 10; // number of iterations
int print = 0; // a boolean to print outputs
int n = 4;

double mytimer(void)
{
	struct timeval tp;
	gettimeofday( &tp, NULL );
	return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *board, int ldboard, int loop)
{
	if (N > 80) {
		fprintf(stderr, "Warning: Board might not hold on terminal\n");
		sleep(3);
	}
	int i,j;
	printf("loop %d\n", loop);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			if ( cell( i, j) == 1)
				printf("X");
			else
				printf(" ");
		}
		printf("\n");
	}
}

// same as output_board, but with more arguments 
void output_block(int M, int N, int *local_board, int ldboard, int loop)
{
	if (N > 80) {
		fprintf(stderr, "Warning: Board might not hold on terminal\n");
		sleep(3);
	}
	int i,j;
	printf("loop %d\n", loop);
	for (i=0; i<M; i++) {
		for (j=0; j<N; j++) {
			if ( cell_ld(local_board, i, j, ldboard ) == 1)
				printf("X");
			else
				printf(" ");
		}
		printf("\n");
	}
}

// same as output_board, but with more arguments 
void output_ngb(int M, int N, int *local_board, int ldboard, int loop)
{
	if (N > 80) {
		fprintf(stderr, "Warning: Board might not hold on terminal\n");
		sleep(3);
	}
	int i,j;
	printf("loop %d\n", loop);
	for (i=0; i<M; i++) {
		for (j=0; j<N; j++) {
			printf("%d ",cell_ld(local_board, i, j, ldboard ));
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

// read arguments from command line
int get_arg(int argc,char** argv,int* nb_row, int* nb_col)
{
	char c;
	while ((c = getopt (argc, argv, "t:r:c:n:s:p:")) != -1){
		switch (c) {
			case 't':
			maxloop = atoi(optarg);
			break;
			case 'r':
			if (nb_row != NULL)
				*nb_row = atoi(optarg);
			else return 1;
			break;
			case 'c':
			if (nb_col != NULL)
				*nb_col = atoi(optarg);
			else return 1;
			break;
			case 'n':
			n = atoi(optarg);
			break;
			case 's':
			BS = atoi(optarg);
			break;
			case 'p':
			print = atoi(optarg);
			break;
			default:
			return 1;
		}
	}
	return 0;
}