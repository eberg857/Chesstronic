/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Header file for debouncing button presses on 64 buttons in an 8x8 grid.
 */
#ifndef CHESS_BUTTON_CONTROL
#define CHESS_BUTTON_CONTROL

/*
 * This defines the minimum amount of time for which a button press is
 * considered real, in milliseconds.
 */
#define BUTTON_DEBOUNCE_TIME 10

/*
 * This two byte value holds the button press states. The lower byte holds
 * information for port 2, the upper byte for port 3.
 *
 * Port 2 is columns, port 3 is rows.
 */
extern unsigned int button_state;

/*
 * Holds the value currently in button_state as two unsigned integers,
 * indicating the x and y position of the active button press. If no button
 * is currently being pressed, both values will be -1. If two buttons are
 * being pressed at the same time, both values will be -2.
 */
extern int active_button_x;
extern int active_button_y;

/*
 * Convert an x and y coordinate to an LED id (for use in serial_led_control).
 */
int get_led_id(int x, int y);

/*
 * Perform all the required initial setup for this module:
 *     Setup all pins on P2 and P3 for button inputs.
 *     Setup WDT+ as an interrupt timer.
 *     Enable global interrupts.
 */
void button_control_setup();

#endif /* CHESS_BUTTON_CONTROL */
