#include "util.h"

int loop = 0, num_alive = 0;
int ldboard, ldnbngb;

int *board;
int *nbngb;
int BS = 4096; // size of the problem


void init(){
	num_alive = 0;

    /* Leading dimension of the board array */
	ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
	ldnbngb = BS;

	board = malloc( ldboard * ldboard * sizeof(int) );
	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

	num_alive = generate_initial_board( BS, &(cell(0, 0)), ldboard );
}