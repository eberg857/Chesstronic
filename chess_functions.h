/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Generic chess game functions.
 */
#ifndef CHESS_FUNCTIONS
#define CHESS_FUNCTIONS

/**
 * Reset the current board back to starting chess orientation.
 * MAKE SURE TO CALL THIS WHEN INITIALIZING BOARD
 */
void reset_board();

/**
 * THIS METHOD IS INEFFICIENT
 *
 * Takes in current player side, and returns whether the player is in checkmate or not.
 * Return 1 if in checkmate , 0 if not, -1 if error
 */
int in_checkmate(int side);

/**
 * Takes in current player side, and returns whether the player is in check or not.
 * Return 1 if in check, 0 if not, -1 if error
 */
int in_check(int side);

/**
 * Calculates the possible moves of a piece given the position pressed. It does this by altering the state
 * of the current board, where a 1XX value means a move square and 2XX value means a take square
 *
 * REMEMBER TO REVERT TO ORIGINAL UNCALCULATED BOARD USING revert_board() IF THE USER DOESNT CHOOSE TO
 * MOVE SAID PIECE (you dont need to do this if num moves is 0)
 *
 * Returns: num of possible moves
 */
int calculate_moves(int x_pos, int y_pos, int side);

/**
 * Revert board to pre-calculated move state.
 *
 * NOTE: make sure to call this after using calculate_moves(),
 * plus it's fine to call it more often than not because it wont actually modify that values of any pieces
 */
void revert_board();

/**
 * Returns the piece id associated with the input position.
 *
 * Returns: piece id at location
 */
int get_piece_at_pos(int x_pos, int y_pos);

/**
 * Request that the piece given by parameter at the original location be moved to the new location.
 *
 * Returns: 1 if the move was successful, 0 if not
 */
int send_move(int orig_x_pos, int orig_y_pos, int new_x_pos, int new_ypos, int side); 

#endif /* CHESS_FUNCTIONS */
