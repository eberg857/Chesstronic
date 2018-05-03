/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Generic chess game functions.
 */
#include <chess_functions.h>

/*
 * Helper function declarations.
 */
int checkDiagonal(int x_pos, int y_pos);
int checkRows(int x_pos, int y_pos);
int checkColumns(int x_pos, int y_pos);
int checkKnight(int x_pos, int y_pos);
int verify_move(int orig_x_pos, int orig_y_pos, int new_x_pos, int new_y_pos,
                int piece, int side);
int calculate_diagonal_moves(int x_pos, int y_pos, int side, int piece_num,
                             int king_flag);
int calculate_vertical_moves(int x_pos, int y_pos, int side, int piece_num,
                             int king_flag);
int calculate_horizontal_moves(int x_pos, int y_pos, int side, int piece_num,
                               int king_flag);

/**
 *
 *	TODO: 
 *  (1) En pessant
 *  (2) Pawn promotion
 *  (3) Castling
 *  (4) Verifying checkmate in a less resource consuming way
 *
 */
//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//~//


/* State of the current game */
unsigned char currentboard[8][8];

char w_kingSideCastle = 1;
char w_queenSideCastle = 1;
char b_kingSideCastle = 1;
char b_queenSideCastle = 1;

/**
 * Reset the current board back to starting chess orientation.
 * MAKE SURE TO CALL THIS WHEN INITIALIZING BOARD
 */
void reset_board() {
    unsigned int i;
    unsigned int j;

	/* Set all of middle squares to unoccupied */
	for (i = 2; i < 6; i++) {
		for (j = 0; j < 8; j++) {
			currentboard[i][j] = 0;
		}
	}

	/* Set up pawns for both sides */
	for (j = 0; j < 8; j++) {
		currentboard[1][j] = 1;
		currentboard[6][j] = 11;
	}

	/* Set up other pieces */
	for (j = 0; j < 4; j++) {
		currentboard[0][j] = j + 2;
		currentboard[7][j] = j + 12;

		currentboard[0][7 - j] = j + 2;
		currentboard[7][7 - j] = j + 12;
		if (j == 3) {
			currentboard[0][j] = 6;
			currentboard[7][j] = 16;
		}
	}
}

/**
 * THIS METHOD IS INEFFICIENT
 *
 * Takes in current player side, and returns whether the player is in checkmate
 * or not.
 * Return 1 if in checkmate , 0 if not, -1 if error
 */
int in_checkmate(int side) {
    unsigned int i;
    unsigned int j;

	// go through every piece on board
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (currentboard[i][j] == 0) continue;

			// if the piece is on players side
			if ((side == 0 && currentboard[i][j] < 10) ||
			        (side == 1 && currentboard[i][j] > 10)) {

				// if the player has any possible move, they're not in checkmate
				if (calculate_moves(i, j, side) > 0) {
				    revert_board();
				    return 0;
				}
				revert_board();
			}
		}
	}

	return 1;
}

/**
 * Takes in current player side, and returns whether the player is in check or
 * not.
 * Return 1 if in check, 0 if not, -1 if error
 */
int in_check(int side) {
    unsigned int i;
    unsigned int j;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (currentboard[i][j] == (side * 10) + 6) {
				if (checkDiagonal(i, j) || checkRows(i, j) ||
				        checkColumns(i, j) || checkKnight(i, j)) return 1;
				return 0;
			}
		}
	}

	// SHOULD NEVER GET TO HERE
	return -1;
}

/**
 * Calculates the possible moves of a piece given the position pressed. It does
 * this by altering the state of the current board, where a 1XX value means a
 * move square and 2XX value means a take square
 *
 * REMEMBER TO REVERT TO ORIGINAL UNCALCULATED BOARD USING revert_board()
 * IF THE USER DOESNT CHOOSE TO MOVE SAID PIECE (you dont need to do this if
 * num moves is 0)
 *
 * Returns: num of possible moves
 */
int calculate_moves(int x_pos, int y_pos, int side) {
	int temp_piece = 0;
	int moves = 0;

	int piece = currentboard[x_pos][y_pos];
	if (piece == 0 || ((side == 1) && (piece <= 10))
	        || ((side == 0) &&(piece >= 10))) {
		return 0;
	}

	// get kind of piece
	piece = piece % 10;

	int adv_pawn = x_pos + 1 - (2 * side);
    int adv2_pawn = x_pos + 2*(1 - (2 * side));

	// update tempboard with possible moves
	switch(piece) {
		case 1: /* PAWN */

			// pawn moves up single space
			if ((adv_pawn >= 0) && (adv_pawn < 8) && (currentboard[adv_pawn][y_pos] == 0)) {

				currentboard[adv_pawn][y_pos] = ((side * 10) + piece);
				currentboard[x_pos][y_pos] = 0;
				
				int check = in_check(side);
				currentboard[x_pos][y_pos] = ((side * 10) + piece);
				currentboard[adv_pawn][y_pos] = 0;

				if (!check) {
					currentboard[adv_pawn][y_pos] += 100;
				}

			}

			// pawn takes to right
			if ((adv_pawn >= 0) && (adv_pawn < 8) && (y_pos + 1 < 8) &&
			        ((side == 0  && currentboard[adv_pawn][y_pos + 1] > 10) ||
			         (side == 1 && currentboard[adv_pawn][y_pos + 1] < 10 && currentboard[adv_pawn][y_pos + 1] != 0))) {

				temp_piece = currentboard[adv_pawn][y_pos + 1];
				currentboard[adv_pawn][y_pos + 1] = ((side * 10) + piece);
				currentboard[x_pos][y_pos] = 0;
				
				int check = in_check(side);
				currentboard[x_pos][y_pos] = ((side * 10) + piece);
				currentboard[adv_pawn][y_pos + 1] = temp_piece;

				if (!check) {
					currentboard[adv_pawn][y_pos + 1] += 200;
				}

			}

			// pawn takes to left
			if ((adv_pawn >= 0) && (adv_pawn < 8) && (y_pos - 1 >= 0) &&
			        ((side == 0  && currentboard[adv_pawn][y_pos - 1] > 10) ||
			         (side == 1 && currentboard[adv_pawn][y_pos - 1] < 10 && currentboard[adv_pawn][y_pos - 1] != 0))) {

				temp_piece = currentboard[adv_pawn][y_pos - 1];
				currentboard[adv_pawn][y_pos - 1] = ((side * 10) + piece);
				currentboard[x_pos][y_pos] = 0;
				
				int check = in_check(side);
				currentboard[x_pos][y_pos] = ((side * 10) + piece);
				currentboard[adv_pawn][y_pos - 1] = temp_piece;

				if (!check) {
					currentboard[adv_pawn][y_pos - 1] += 200;
				}

			}

			// pawn hasn't been moved yet, can do double space
			if ((adv2_pawn >= 0) && (adv2_pawn < 8) && (currentboard[adv2_pawn][y_pos] == 0) 
				&& (((side == 0) && (x_pos == 1)) || ((side == 1) && (x_pos == 6)))) {

				currentboard[adv2_pawn][y_pos] = ((side * 10) + piece);
				currentboard[x_pos][y_pos] = 0;
				
				int check = in_check(side);
				currentboard[x_pos][y_pos] = ((side * 10) + piece);
				currentboard[adv2_pawn][y_pos] = 0;

				if (!check) {
					currentboard[adv2_pawn][y_pos] += 100;
				}
			}
			
			break;
		case 2: /* ROOK */
			moves += calculate_vertical_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 0);
			moves += calculate_horizontal_moves(x_pos, y_pos, side,
			                                    ((side * 10) + piece), 0);
			break;
		case 3: /* KNIGHT */
			if (x_pos + 1 < 8 && y_pos + 2 < 8) {
				moves += verify_move(x_pos, y_pos, x_pos + 1, y_pos + 2,
				                     ((side * 10) + piece), side);
			}

			if (x_pos + 2 < 8 && y_pos + 1 < 8) {
				moves += verify_move(x_pos, y_pos, x_pos + 2, y_pos + 1,
				                     ((side * 10) + piece), side);
			}

			if (x_pos + 1 < 8 && y_pos - 2 >= 0) {
				moves += verify_move(x_pos, y_pos, x_pos + 1, y_pos - 2,
				                     ((side * 10) + piece), side);
			}

			if (x_pos + 2 < 8 && y_pos - 1 >= 0) {
				moves += verify_move(x_pos, y_pos, x_pos + 2, y_pos - 1,
				                     ((side * 10) + piece), side);
			}

			if (x_pos - 1 >= 0 && y_pos + 2 < 8) {
				moves += verify_move(x_pos, y_pos, x_pos - 1, y_pos + 2,
				                     ((side * 10) + piece), side);
			}

			if (x_pos - 2 >= 0 && y_pos + 1 < 8) {
				moves += verify_move(x_pos, y_pos, x_pos - 2, y_pos + 1,
				                     ((side * 10) + piece), side);
			}

			if (x_pos - 1 >= 0 && y_pos - 2 >= 0) {
				moves += verify_move(x_pos, y_pos, x_pos - 1, y_pos - 2,
				                     ((side * 10) + piece), side);
			}

			if (x_pos - 2 >= 0 && y_pos - 1 >= 0) {
				moves += verify_move(x_pos, y_pos, x_pos - 2, y_pos - 1,
				                     ((side * 10) + piece), side);
			}
			break;
		case 4: /* BISHOP */
			moves += calculate_diagonal_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 0);
			break;
		case 5: /* QUEEN */
			moves += calculate_vertical_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 0);
			moves += calculate_horizontal_moves(x_pos, y_pos, side,
			                                    ((side * 10) + piece), 0);
			moves += calculate_diagonal_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 0);
			break;
		case 6: /* KING */
		    if (side == 0 && x_pos == 0 && y_pos == 3 && w_kingSideCastle == 1) {
                if (currentboard[0][1] == 0 && currentboard[0][2] == 0) {
                    // modify the board
                    currentboard[0][2] = 6;
                    currentboard[0][3] = 0;

                    // if in check, move was invalid
                    if (!in_check(side)) {
                        currentboard[0][1] = 6;
                        currentboard[0][2] = 0;
                        if (!in_check(side)) {
                            currentboard[0][1] = 0;
                            currentboard[0][1] += 100;
                            currentboard[0][3] = 6;
                        } else {
                            currentboard[0][1] = 0;
                            currentboard[0][3] = 6;
                        }

                    } else {
                        currentboard[0][2] = 0;
                        currentboard[0][3] = 6;
                    }
                }
            }
		    if (side == 1 && x_pos == 7 && y_pos == 3 && b_kingSideCastle == 1) {
		        if (currentboard[7][1] == 0 && currentboard[7][2] == 0) {
		            // modify the board
                    currentboard[7][2] = 16;
                    currentboard[7][3] = 0;

                    // if in check, move was invalid
                    if (!in_check(side)) {
                        currentboard[7][1] = 16;
                        currentboard[7][2] = 0;
                        if (!in_check(side)) {
                            currentboard[7][1] = 0;
                            currentboard[7][3] = 16;
                            currentboard[7][1] += 100;
                        } else {
                            currentboard[7][1] = 0;
                            currentboard[7][3] = 16;
                        }
                    } else {
                        currentboard[7][2] = 0;
                        currentboard[7][3] = 16;
                    }
		        }
		    }
		    if (side == 0 && x_pos == 0 && y_pos == 3 && w_queenSideCastle == 1) {
                if (currentboard[0][4] == 0 && currentboard[0][5] == 0 && currentboard[0][6] == 0) {
                    // modify the board
                    currentboard[0][4] = 6;
                    currentboard[0][3] = 0;

                    // if in check, move was invalid
                    if (!in_check(side)) {
                        // modify the board
                       currentboard[0][5] = 6;
                       currentboard[0][4] = 0;

                       // if in check, move was invalid
                       if (!in_check(side)) {
                           currentboard[0][6] = 6;
                           currentboard[0][5] = 0;
                           if (!in_check(side)) {
                               // revert board
                               currentboard[0][6] = 0;
                               currentboard[0][3] = 6;
                               currentboard[0][5] += 100;
                           } else {
                               currentboard[0][6] = 0;
                               currentboard[0][3] = 6;
                           }
                       } else {
                           currentboard[0][5] = 0;
                           currentboard[0][3] = 6;
                       }

                    } else {
                        currentboard[0][4] = 0;
                        currentboard[0][3] = 6;
                    }
                }
            }
		    if (side == 1 && x_pos == 7 && y_pos == 3 && b_queenSideCastle == 1) {
                if (currentboard[7][4] == 0 && currentboard[7][5] == 0 && currentboard[7][6] == 0) {
                    // modify the board
                    currentboard[7][4] = 16;
                    currentboard[7][3] = 0;

                    // if in check, move was invalid
                    if (!in_check(side)) {
                        // modify the board
                       currentboard[7][5] = 16;
                       currentboard[7][4] = 0;

                       // if in check, move was invalid
                       if (!in_check(side)) {
                           currentboard[7][6] = 16;
                           currentboard[7][5] = 0;
                           if (!in_check(side)) {
                               currentboard[7][6] = 0;
                               currentboard[7][3] = 16;
                               currentboard[7][5] += 100;
                           } else {
                               currentboard[7][6] = 0;
                               currentboard[7][3] = 16;
                           }
                       } else {
                           currentboard[7][5] = 0;
                           currentboard[7][3] = 16;
                       }

                    } else {
                        currentboard[7][4] = 0;
                        currentboard[7][3] = 16;
                    }
                }
            }
			moves += calculate_vertical_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 1);
			moves += calculate_horizontal_moves(x_pos, y_pos, side,
			                                    ((side * 10) + piece), 1);
			moves += calculate_diagonal_moves(x_pos, y_pos, side,
			                                  ((side * 10) + piece), 1);
			break;
	}
	return moves;
}

/**
 * Revert board to pre-calculated move state. 
 * 
 * NOTE: make sure to call this after using calculate_moves(),
 * plus it's fine to call it more often than not because it wont actually modify
 * that values of any pieces.
 */
void revert_board() {
    unsigned int i;
    unsigned int j;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			currentboard[i][j] = currentboard[i][j] % 100;
		}
	}
}

/**
 * Returns the piece id associated with the input position.
 *
 * Returns: piece id at location
 */
int get_piece_at_pos(int x_pos, int y_pos) {
	return currentboard[x_pos][y_pos];
}

/**
 * Request that the piece given by parameter at the original location be moved to the new location.
 *
 * Returns: 1 if the move was successful, 0 if not
 */
int send_move(int orig_x_pos, int orig_y_pos, int new_x_pos, int new_y_pos, int side) {
	// error catching
	if (currentboard[new_x_pos][new_y_pos] < 100) return 0;

	int piece = currentboard[orig_x_pos][orig_y_pos];

	// save the last piece just in case
	int temp_piece = currentboard[new_x_pos][new_y_pos];

	// modify the board
	currentboard[new_x_pos][new_y_pos] = piece;
	currentboard[orig_x_pos][orig_y_pos] = 0;
	
	// are you in check after this move?
	int check = in_check(side);

	// if in check, move was invalid
	if (check) {
		// revert board
		currentboard[orig_x_pos][orig_y_pos] = piece;
		currentboard[new_x_pos][new_y_pos] = temp_piece;
		return 0;
	}

	if (piece % 10 == 6) {
	    if (side == 0 && w_kingSideCastle == 1 && new_y_pos == 1) {
	        currentboard[0][0] = 0;
	        currentboard[0][2] = 2;
	    }

	    if (side == 1 && b_kingSideCastle == 1 && new_y_pos == 1) {
            currentboard[7][0] = 0;
            currentboard[7][2] = 12;
        }

	    if (side == 0 && w_queenSideCastle == 1 && new_y_pos == 5) {
            currentboard[0][7] = 0;
            currentboard[0][4] = 2;
	    }

	    if (side == 1 && b_queenSideCastle == 1 && new_y_pos == 5) {
            currentboard[7][7] = 0;
            currentboard[7][4] = 12;
        }

	    if (side == 0) {
	        w_kingSideCastle = 0;
	        w_queenSideCastle = 0;
	    } else {
	        b_kingSideCastle = 0;
	        b_queenSideCastle = 0;
	    }
	}

	if (piece % 10 == 2) {
	    if (orig_y_pos == 0) {
	        if (side == 0) w_kingSideCastle = 0;
	        else b_kingSideCastle = 0;
	    }
	    else if (orig_y_pos == 7) {
	        if (side == 0) w_queenSideCastle = 0;
	        else b_queenSideCastle = 0;
	    }
	}

	if (piece % 10 == 1 && (new_x_pos == 0 || new_x_pos == 7)) {
	    currentboard[new_x_pos][new_y_pos] = 10 * side + 5;
	}

	return 1;
}



/////////////////////////////////////////////////////////////////////////////
/**																				
 *
 * BELOW IS LIST OF INTERNAL FUNCTIONS, THERE SHOULD BE NO NEED TO CALL THESE
 *
 */
/////////////////////////////////////////////////////////////////////////////





/**
 * 
 * Calculates whether the king is in check from diagonal.
 * Returns: 1 if in check, 0 if not
 */
int checkDiagonal(int x_pos, int y_pos) {
	int side = currentboard[x_pos][y_pos] < 10 ? 0 : 1;
	int i;
	int j;
	int new_x_pos;
	int new_y_pos;
	
	int refval;

	for (i = 0; i < 4; i++) { // outer loop for four diagonals
		for (j = 1; j < 8; j++) {// inner loop for iterating through diagonals
		    new_x_pos = x_pos + ((i & 0x01) ? 1 : -1) * j;
			new_y_pos = y_pos + ((i >= 2) ? 1 : -1) * j;

			if (new_x_pos < 0 || new_x_pos > 7) break;
			if (new_y_pos < 0 || new_y_pos > 7) break;

			refval = currentboard[new_x_pos][new_y_pos] % 100;

			// if empty space, continue
			if (refval == 0) continue;

			// if your own piece is on diagonal, check next diagonal
			if ((refval < 10 && side == 0) 
				|| (refval > 10 && side == 1)) break;

			// if their piece is on diagonal and not bishop or queen, check next diagonal
			if (refval != 10 * ((side + 1) % 2) + 4
				&& refval != 10 * ((side + 1) % 2) + 5
				&& !(refval == 10 * ((side + 1) % 2) + 1 && new_x_pos + 1 - (2 * ((side + 1) % 2)) == x_pos)) break;

			// only option left is that theres an enemy queen or bishop
			return 1;
		}
	}
	return 0;
}

/**
 * Calculates whether the king is in check from rows.
 * Returns: 1 if in check, 0 if not
 */
int checkRows(int x_pos, int y_pos) {
    int side = currentboard[x_pos][y_pos] < 10 ? 0 : 1;
	int i;
	int j;
	int new_x_pos;

	int refval;

	for (i = 0; i < 2; i++) { // outer loop for two rows
		for (j = 1; j < 8; j++) {// inner loop for iterating through rows
			new_x_pos = x_pos + ((2 * (i % 2) - 1) * j);

			if (new_x_pos < 0 || new_x_pos > 7) break;

			refval = currentboard[new_x_pos][y_pos] % 100;

			// if empty space, continue
			if (refval == 0) continue;

			// if your own piece is on row, check next row
			if (refval < 10 && side == 0) break;
			if (refval > 10 && side == 1) break;

			// if their piece is on row and not rook or queen, check next row
			if (refval != 10 * ((side + 1) % 2) + 2
				&& refval != 10 * ((side + 1) % 2) + 5) break;

			// only option left is that theres an enemy rock or queen
			return 1;
		}
	}
	return 0;
}

/**
 * Calculates whether the king is in check from columns.
 * Returns: 1 if in check, 0 if not
 */
int checkColumns(int x_pos, int y_pos) {
    int side = currentboard[x_pos][y_pos] < 10 ? 0 : 1;
	int i;
	int j;
	int new_y_pos;

	int refval;

	for (i = 0; i < 2; i++) { // outer loop for two rows
		for (j = 1; j < 8; j++) {// inner loop for iterating through rows
			new_y_pos = y_pos + ((2 * (i % 2) - 1) * j);

			if (new_y_pos < 0 || new_y_pos > 7) break;

			refval = currentboard[x_pos][new_y_pos] % 100;

			// if empty space, continue
			if (refval == 0) continue;

			// if your own piece is on row, check next row
			if (refval < 10 && side == 0) break;
			if (refval > 10 && side == 1) break;

			// if their piece is on row and not rook or queen, check next row
			if (refval != 10 * ((side + 1) % 2) + 2
				&& refval != 10 * ((side + 1) % 2) + 5) break;

			// only option left is that theres an enemy rock or queen
			return 1;
		}
	}
	return 0;
}

/**
 * Calculates whether the king is in check from knight.
 * Returns: 1 if in check, 0 if not
 */
int checkKnight(int x_pos, int y_pos) {
    int side = currentboard[x_pos][y_pos] < 10 ? 0 : 1;

	if (x_pos + 1 < 8 && y_pos + 2 < 8) {
		if (currentboard[x_pos + 1][y_pos + 2] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos + 2 < 8 && y_pos + 1 < 8) {
		if (currentboard[x_pos + 2][y_pos + 1] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos + 1 < 8 && y_pos - 2 >= 0) {
		if (currentboard[x_pos + 1][y_pos - 2] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos + 2 < 8 && y_pos - 1 >= 0) {
		if (currentboard[x_pos + 2][y_pos - 1] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos - 1 >= 0 && y_pos + 2 < 8) {
		if (currentboard[x_pos - 1][y_pos + 2] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos - 2 >= 0 && y_pos + 1 < 8) {
		if (currentboard[x_pos - 2][y_pos + 1] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos - 1 >= 0 && y_pos - 2 >= 0) {
		if (currentboard[x_pos - 1][y_pos - 2] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	if (x_pos - 2 >= 0 && y_pos - 1 >= 0) {
		if (currentboard[x_pos - 2][y_pos - 1] % 100 == 10 * ((side + 1) % 2) + 3) return 1;
	}

	return 0;
}

/**
 * When requesting to move to new position, verify the nature of the movement (take or move)
 * and then update board accordingly.
 *
 * Returns: num of moves
 */
int verify_move(int orig_x_pos, int orig_y_pos, int new_x_pos, int new_y_pos, int piece, int side) {

	// used for when taking a piece
	int temp_piece;

	// check if the desired space is unoccupied
	if (currentboard[new_x_pos][new_y_pos] == 0) {

		// modify the board
		currentboard[new_x_pos][new_y_pos] = piece;
		currentboard[orig_x_pos][orig_y_pos] = 0;
		
		// are you in check after this move?
		int check = in_check(side);

		// revert board
		currentboard[orig_x_pos][orig_y_pos] = piece;
		currentboard[new_x_pos][new_y_pos] = 0;

		// if not in check, make note
		if (!check) {
			currentboard[new_x_pos][new_y_pos] += 100;
			return 1;
		}

	} else if ((side == 0  && currentboard[new_x_pos][new_y_pos] > 10) || (side == 1 && currentboard[new_x_pos][new_y_pos] < 10)) { // there is a piece in desired space

		// save the value of the taken piece
		temp_piece = currentboard[new_x_pos][new_y_pos];

		// modify the board
		currentboard[new_x_pos][new_y_pos] = piece;
		currentboard[orig_x_pos][orig_y_pos] = 0;
		
		// are you in check after this move?
		int check = in_check(side);

		// revert board
		currentboard[orig_x_pos][orig_y_pos] = piece;
		currentboard[new_x_pos][new_y_pos] = temp_piece;

		// if not in check, make note
		if (!check) {
			currentboard[new_x_pos][new_y_pos] += 200;
			return 1;
		}
	}
	return 0;
}


/**
 * Calculate the possile moves a diagonally moving piece, modifying the current board
 *
 * Returns: number of moves
 */
int calculate_diagonal_moves(int x_pos, int y_pos, int side, int piece_num, int king_flag) {
	int i;
	int j;
	int new_x_pos;
	int new_y_pos;
	int moves = 0;
	int max_num = 8;

	if (king_flag) max_num = 2;

	for (i = 0; i < 4; i++) { // outer loop for four diagonals
		for (j = 1; j < max_num; j++) {// inner loop for iterating through diagonals
			new_x_pos = x_pos + ((2 * (i % 2) - 1) * j);
			new_y_pos = y_pos + ((2 * ((int) (i >= 2)) - 1) * j);

			if (new_x_pos < 0 || new_x_pos > 7) break;
			if (new_y_pos < 0 || new_y_pos > 7) break;

			// if empty space, verify and continue
			if (currentboard[new_x_pos][new_y_pos] == 0) {
				moves += verify_move(x_pos, y_pos, new_x_pos, new_y_pos, piece_num, side);
				continue;
			}

			// if your own piece is on diagonal, check next diagonal
			if (currentboard[new_x_pos][new_y_pos] < 10 && side == 0) break;
			if (currentboard[new_x_pos][new_y_pos] > 10 && side == 1) break;

			// if their piece is on diagonal, verify and move to next diagonal
			moves += verify_move(x_pos, y_pos, new_x_pos, new_y_pos, piece_num, side);
			break;
		}
	}
	return moves;
}

/**
 * Calculate the possile moves a vertically moving piece, modifying the current board
 *
 * Returns: number of moves
 */
int calculate_vertical_moves(int x_pos, int y_pos, int side, int piece_num, int king_flag) {
	int i;
	int j;
    int new_y_pos;
	int moves = 0;
    int max_num = 8;

	if (king_flag) max_num = 2;

	for (i = 0; i < 2; i++) { // outer loop for four rows
		for (j = 1; j < max_num; j++) {// inner loop for iterating through rows
			new_y_pos = y_pos + ((2 * (i % 2) - 1) * j);

			if (new_y_pos < 0 || new_y_pos > 7) break;

			// if empty space, continue
			if (currentboard[x_pos][new_y_pos] == 0) {
				moves += verify_move(x_pos, y_pos, x_pos, new_y_pos, piece_num, side);
				continue;
			}

			// if your own piece is on row, check next row
			if (currentboard[x_pos][new_y_pos] < 10 && side == 0) break;
			if (currentboard[x_pos][new_y_pos] > 10 && side == 1) break;

			// if their piece is on row, verify and move to next row
			moves += verify_move(x_pos, y_pos, x_pos, new_y_pos, piece_num, side);
			break;
		}
	}
	return moves;
}

/**
 * Calculate the possile moves a horizontally moving piece, modifying the current board
 *
 * Returns: number of moves
 */
int calculate_horizontal_moves(int x_pos, int y_pos, int side, int piece_num, int king_flag) {
	int i;
	int j;
	int new_x_pos;
	int moves = 0;
    int max_num = 8;

	if (king_flag) max_num = 2;

	for (i = 0; i < 2; i++) { // outer loop for four rows
		for (j = 1; j < max_num; j++) {// inner loop for iterating through rows
			new_x_pos = x_pos + ((2 * (i % 2) - 1) * j);

			if (new_x_pos < 0 || new_x_pos > 7) break;

			// if empty space, continue
			if (currentboard[new_x_pos][y_pos] == 0) {
				moves += verify_move(x_pos, y_pos, new_x_pos, y_pos, piece_num, side);
				continue;
			}

			// if your own piece is on row, check next row
			if (currentboard[new_x_pos][y_pos] < 10 && side == 0) break;
			if (currentboard[new_x_pos][y_pos] > 10 && side == 1) break;

			// if their piece is on row, verify and move to next row
			moves += verify_move(x_pos, y_pos, new_x_pos, y_pos, piece_num, side);
			break;
		}
	}
	return moves;
}
