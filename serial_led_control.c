/*
 * Eduardo Berg <eb28@rice.edu>
 * Logan Lawrence <lcl5@rice.edu>
 * Nathaniel Morris <nam6@rice.edu>
 *
 * Code for APA102 serial LED control module.
 */
#include <msp430g2553.h>
#include <serial_led_control.h>

/*
 * Define the number of bytes of storage required to hold the control bytes.
 */
#define NUM_SERIAL_ARRAY_BYTES ((NUM_SERIAL_LEDS << 2) + 8)

/*
 * Array to hold the control bytes for the N LEDs in the LED chain.
 */
static unsigned char led_control_bytes[NUM_SERIAL_ARRAY_BYTES];

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
                         unsigned char b_val) {
    if ((led_idx + 1) > NUM_SERIAL_LEDS) {
        // Invalid index, return failure.
        return 0;
    }

    // Calculate index in array where data is stored.
    unsigned int arr_idx = (led_idx << 2) + 4;

    // Set the 4 bytes in the array corresponding to the specified LED.
    led_control_bytes[arr_idx] = 0xE0 | global_val;
    led_control_bytes[arr_idx + 1] = b_val;
    led_control_bytes[arr_idx + 2] = g_val;
    led_control_bytes[arr_idx + 3] = r_val;

    return 1;
}

/*
 * Convenience function to set all LEDs to off. Does not send the serial
 * commands to the LEDs.
 */
void clear_serial_leds() {
    unsigned int i;

    for (i = 4; i < NUM_SERIAL_ARRAY_BYTES - 4; i += 4) {
        led_control_bytes[i] = 0xE0; // Global brightness value
        led_control_bytes[i + 1] = 0x00; // RGB values:
        led_control_bytes[i + 2] = 0x00;
        led_control_bytes[i + 3] = 0x00;
    }
}

/*
 * Send the LED control commands to the LEDs via SPI.
 *
 * Synchronous, waits until LED commands are completely sent before returning.
 */
void send_serial_led_commands() {
    unsigned int i;
    unsigned char j;

    // Iterate through each byte in the led_control_bytes array
    for (i = 0; i < NUM_SERIAL_ARRAY_BYTES; i++) {
        unsigned char b = led_control_bytes[i];

        for (j = 0x80; j != 0; j = j >> 1) {
            // Send clock low.
            P1OUT &= ~BIT5;
            if (b & j) {
                // Send data high.
                P1OUT |= BIT7;
            } else {
                // Send data low.
                P1OUT &= ~BIT7;
            }
            // Send clock high.
            P1OUT |= BIT5;
        }
    }
}

/*
 * Perform all the required initial setup for this module:
 *     Setup pin P1.7 as SIMO data line.
 *     Setup pin P1.5 as data clock line.
 *     Enable global interrupts.
 */
void serial_led_control_setup() {
    unsigned int i;

    // Disable interrupts while setting up serial LED control:
    __disable_interrupt();

    // Initialize data storage for serial LED control:
    for (i = 0; i < NUM_SERIAL_ARRAY_BYTES; i++) {
        if (i < 4) {
            led_control_bytes[i] = 0x00; // Start frame
        } else if (NUM_SERIAL_ARRAY_BYTES - i <= 4) {
            led_control_bytes[i] = 0xFF; // End frame
        } else if (i & 0x03) {
            led_control_bytes[i] = 0x00; // RGB value
        } else {
            led_control_bytes[i] = 0xE0; // Global brightness value
        }
    }

    // Configure I/O pins for output:
    P1DIR |= BIT5 | BIT7;
    // Set data to low, clock to high:
    P1OUT &= ~BIT7;
    P1OUT |= BIT5;

    // Globally enable all interrupts:
    __enable_interrupt();

    // Serial LEDs aren't ready as fast as the MSP430 is, so delay here for
    // about 0.05 seconds, the LEDs should be ready at this point.
    __delay_cycles(50000);
}
