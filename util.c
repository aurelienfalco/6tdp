#include "util.h"

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


int get_arg(int argc,char** argv,int* nb_row, int* nb_col)
{
	BS = 4096;
	maxloop = 10;
	char c;
	while ((c = getopt (argc, argv, "t:x:y:s:")) != -1){
		switch (c) {
			case 't':
			maxloop = atoi(optarg);
			break;
			case 'x':
			if (nb_row != NULL)
				*nb_row = atoi(optarg);
			else return 1;
			break;
			case 'y':
			if (nb_col != NULL)
				*nb_col = atoi(optarg);
			else return 1;
			break;
			case 's':
			BS = atoi(optarg);
			break;
			default:
			return 1;
		}
	}
	return 0;
}