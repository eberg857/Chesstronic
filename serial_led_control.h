/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Header file for APA102 serial LED control module.
 */
#ifndef CHESS_SERIAL_LED_CONTROL
#define CHESS_SERIAL_LED_CONTROL

/*
 * Defines the number of LEDs serially controlled by this module.
 */
#define NUM_SERIAL_LEDS 64

/*
 * Perform all the required initial setup for this module:
 *     Setup pin P1.7 as SIMO data line.
 *     Setup pin P1.5 as data clock line.
 *     Enable global interrupts.
 */
void serial_led_control_setup();

/*
 * Set the global brightness and RGB value of the specified LED. Note: this
 * does not send the command to the LED yet, use send_led_commands() for that.
 *
 * Parameters:
 *     led_idx - which LED in the chain to control (0-indexed)
 *     global_val - a 5-bit value controlling the brightness of the specified
 *                  LED (the left-most 3 bits of the char are ignored).
 *     r_val - a byte value controlling the red value of the LED
 *     g_val - a byte value controlling the green value of the LED
 *     b_val - a byte value controlling the blue value of the LED
 *
 * Returns:
 *     1 if the function was successful, 0 if there was an error.
 */
int set_serial_led_color(unsigned int led_idx, unsigned char global_val,
                         unsigned char r_val, unsigned char g_val,
                         unsigned char b_val);

/*
 * Convenience function to set all LEDs to off. Does not send the serial
 * commands to the LEDs.
 */
void clear_serial_leds();

/*
 * Send the LED control commands to the LEDs via SPI.
 *
 * Synchronous, waits until LED commands are completely sent before returning.
 */
void send_serial_led_commands();

#endif /* CHESS_SERIAL_LED_CONTROL */
