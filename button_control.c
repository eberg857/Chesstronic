/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Code for debouncing button presses on 64 buttons in an 8x8 grid.
 */
#include <msp430g2553.h>
#include <button_control.h>

/*
 * This two byte value holds the button press states. The lower byte holds
 * information for port 2, the upper byte for port 3.
 *
 * Port 2 is columns, port 3 is rows.
 */
unsigned int button_state = 0x0000;

/*
 * This array keeps track of how many WDT interrupts the button has been
 * pressed/released. The ports and pins are in numeric order.
 */
unsigned char debounce_count[16];

/*
 * Holds the value currently in button_state as two unsigned integers,
 * indicating the x and y position of the active button press. If no button
 * is currently being pressed, both values will be -1. If two buttons are
 * being pressed at the same time, both values will be -2.
 */
int active_button_x = -1;
int active_button_y = -1;

/*
 * Updates the active button integers above.
 */
static int update_active_button() {
    unsigned char row, col;
    int col_val, row_val;

    if (!button_state) {
        // No buttons currently pressed.
        active_button_x = -1;
        active_button_y = -1;
    }
    col = button_state & 0x00FF;
    row = (button_state & 0xFF00) >> 8;
    switch (col) {
        case 0x80: col_val = 7; break;
        case 0x40: col_val = 6; break;
        case 0x20: col_val = 5; break;
        case 0x10: col_val = 4; break;
        case 0x08: col_val = 3; break;
        case 0x04: col_val = 2; break;
        case 0x02: col_val = 1; break;
        case 0x01: col_val = 0; break;
        default: col_val = -2;
    }
    switch (row) {
        case 0x80: row_val = 7; break;
        case 0x40: row_val = 6; break;
        case 0x20: row_val = 5; break;
        case 0x10: row_val = 4; break;
        case 0x08: row_val = 3; break;
        case 0x04: row_val = 2; break;
        case 0x02: row_val = 1; break;
        case 0x01: row_val = 0; break;
        default: row_val = -2;
    }
    if ((row_val == -2 || col_val == -2)) {
        // Invalid button state (e.g. multiple buttons pressed).
        col_val = -2;
        row_val = -2;
    }
    if (active_button_x == row_val && active_button_y == col_val) {
        // Button state not changed.
        return 0;
    } else {
        // Button state changed.
        active_button_x = row_val;
        active_button_y = col_val;
        return 1;
    }
}

/*
 * Convert an x and y coordinate to an LED id (for use in serial_led_control).
 */
int get_led_id(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) {
        return -1;
    }

    if (x & 0x01) {
        return (x << 3) + (7 - y);
    } else {
        return (x << 3) + y;
    }
}

/*
 * WDT+ interrupt vector. This interrupt handles polling all the input pins
 * and debouncing button presses/releases.
 */
#pragma vector=WDT_VECTOR
__interrupt void wdt_interrupt (void) {
    unsigned int i, input_mask, debounce_mask;

    // Clear WDT+ interrupt flag
    IFG1 &= ~WDTIFG;

    // Check all P2 button lines.
    input_mask = 0x01;
    debounce_mask = 0x01;
    for (i = 0; i < 8; i++) {
        if (((P2IN & input_mask) && (button_state & debounce_mask)) ||
                (!(P2IN & input_mask) && !(button_state & debounce_mask))) {
            // Button debounce in progress, increment counter
            debounce_count[i]++;
        } else if (debounce_count[i]) {
            // Button has bounced, restart count
            debounce_count[i] = 0;
        }
        // Check if debounce complete
        if (debounce_count[i] == BUTTON_DEBOUNCE_TIME) {
            button_state ^= debounce_mask;
            debounce_count[i] = 0;
            if (update_active_button()) {
                __bic_SR_register_on_exit(LPM0_bits);
            }
        }
        input_mask = input_mask << 1;
        debounce_mask = debounce_mask << 1;
    }
    // Check all P3 button lines.
    input_mask = 0x01;
    for (i = 0; i < 8; i++) {
        if (((P3IN & input_mask) && (button_state & debounce_mask)) ||
                (!(P3IN & input_mask) && !(button_state & debounce_mask))) {
            // Button debounce in progress, increment counter
            debounce_count[i + 8]++;
        } else if (debounce_count[i + 8]) {
            // Button has bounced, restart count
            debounce_count[i + 8] = 0;
        }
        // Check if debounce complete
        if (debounce_count[i + 8] == BUTTON_DEBOUNCE_TIME) {
            button_state ^= debounce_mask;
            debounce_count[i + 8] = 0;
            if (update_active_button()) {
                __bic_SR_register_on_exit(LPM0_bits);
            }
        }
        input_mask = input_mask << 1;
        debounce_mask = debounce_mask << 1;
    }
}

/*
 * Perform all the required initial setup for this module:
 *     Setup all pins on P2 and P3 for button inputs.
 *     Setup WDT+ as an interrupt timer.
 *     Enable global interrupts.
 */
void button_control_setup() {
    // Disable interrupts while setting up buttons:
    __disable_interrupt();

    // Setup pins as input pins, initially high-to-low transition:
    P2SEL = 0;
    P3SEL = 0;
    P2SEL2 = 0;
    P3SEL2 = 0;
    P2DIR = 0;
    P3DIR = 0;

    // Enable pull-up resistors on the button pins:
    P2REN = 0xFF;
    P3REN = 0xFF;
    P2OUT = 0xFF;
    P3OUT = 0xFF;

    // Set WDT+ module to interrupt every 1ms (main clock assumed to be 8MHz):
    WDTCTL = WDT_MDLY_8;
    IE1 |= WDTIE;

    // Enable global interrupts:
    __enable_interrupt();
}
