/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Final Project: Chess Board
 */
#include <msp430g2553.h>
#include <serial_led_control.h>
#include <button_control.h>
#include <chess_functions.h>

void show_possible_moves();

/*
 * Chess board initialization and main code loop.
 */
int main(void) {
    // Disable WDT+ timer:
    WDTCTL = WDTPW + WDTHOLD;

    // Setup main clock:
    DCOCTL = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;

    // Run setup code:
    serial_led_control_setup();
    button_control_setup();
    reset_board();
    send_serial_led_commands();

    int last_button_x = -1;
    int last_button_y = -1;
    int last_x_pos = -1;
    int last_y_pos = -1;
    int side = 0;
    int state = 0;

    int led_display_counter;

    int button_x = -1;
    int button_y = -1;

    int i = 0;
    int j = 0;
    int display_flip = 0;

    // Main code loop:
    while (1) {
        button_x = active_button_x;
        button_y = active_button_y;

        // if (button_id == active_button_id) {
        //     // Enter low power mode unless button state has already changed.
        //     __bis_SR_register(LPM0_bits);
        // }

        // button_id = active_button_id;

        if (state == 2 || state == 3) {
            if (led_display_counter > 10000) {
                led_display_counter = 0;

                if (display_flip == 0) {
                    for (i = 6 * (state - 2); i < 2 +  6 * (state - 2); i++) {
                        for (j = 0; j < 8; j++) {
                            set_serial_led_color(get_led_id(i, j), 1, 0, 255, 0);
                        }
                    }
                } else {
                    clear_serial_leds();
                }
                display_flip = (display_flip + 1)%2;

                send_serial_led_commands();
            }
            led_display_counter++;
        }


        // ADD SOME PERIODIC CHECKMATE CHECK

        if ((button_x != last_button_x || button_y != last_button_y) && button_x >= 0) {
            if (state == 0) {
                if ((get_piece_at_pos(button_x, button_y) % 100 != 0)
                    && !((side == 0) && ((get_piece_at_pos(button_x, button_y) % 100) > 10))
                    && !((side == 1) && ((get_piece_at_pos(button_x, button_y) % 100) < 10))) { 
                    calculate_moves(button_x, button_y, side);
                    clear_serial_leds();
                    send_serial_led_commands();
                    set_serial_led_color(get_led_id(button_x, button_y), 16, 0, 0, 255);
                    show_possible_moves();
                    last_x_pos = button_x;
                    last_y_pos = button_y;
                    state = 1;
                }
            } else if (state == 1) {
                if (button_x == last_x_pos && button_y == last_y_pos) {
                    revert_board();
                    state = 0;
                    clear_serial_leds();
                    send_serial_led_commands();
                } else if (send_move(last_x_pos, last_y_pos, button_x, button_y, side)) {
                    revert_board();
                    state = 0;
                    side = (side + 1) % 2;
                    clear_serial_leds();
                    send_serial_led_commands();
                    set_serial_led_color(get_led_id(button_x, button_y), 16, 0, 0, 255);
                    send_serial_led_commands();

                    if (in_checkmate(side)) {
                        state = 2 + ((side + 1) % 2);
                    }
                }
            } else if (state == 2 || state == 3) {
                clear_serial_leds();
                send_serial_led_commands();
                state = 0;
                WDTCTL = 0;
            }
        }

        last_button_x = button_x;
        last_button_y = button_y;

        // After wake by button press:
        // if (button_id >= 0 && button_id < 64) {

            
            // Highlight pressed LED.
            // set_serial_led_color(button_id, 1, 0, 0xFF, 0);
            // send_serial_led_commands();
            // set_serial_led_color(button_id, 0, 0, 0, 0);
        // } else {
            // Un-highlight pressed LED.


            // send_serial_led_commands();
        // }

        if (state != 2 && state != 3) __bis_SR_register(LPM0_bits);
    }
}

static volatile int test;
void show_possible_moves() {
    int i;
    int j;
    int temp_piece;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            temp_piece = get_piece_at_pos(i, j);
            test = temp_piece;
            if (temp_piece >= 100 && temp_piece < 200) {
                set_serial_led_color(get_led_id(i, j), 16,
                         0, 255, 0);
            } else if (temp_piece >= 200) {
                set_serial_led_color(get_led_id(i, j), 16,
                         255, 0, 0);
            }
        }
    }
    send_serial_led_commands();
}

