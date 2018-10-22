# Milestone 1: Communicating with Will Byers
This softwere is inteneded to be used with a MSP430F5529 launchpad microprocessor.  This software uses takes in a byte packet over UART, in order to light up an RGB LED.  In addition, it can be hooked up in series with other microprocessors and light up multiple LEDs.  This configuration can be used to make a picture, or communicate in some manor, similar to that of Will Byers and his mother.

# Use
The pins 3.4 and 3.3 on the launchpad are uesd for recieving and transmitting UART data respectively.  They can use the launchpad's ez-FET to communicate from a computer to the RXD pin on the processor. The RXD pin can be hooked up to the 3.4 pin on the processor to transfer bytes from the computer to the board.

The first byte of data transfered is the amount of incoming bytes.  The next 3 bytes are for the PWM of each RGB of the LED.  If the incomming bytes are 0x03 0x00 0x00 0xFF, then the blue LED is 100% on, and the red and green are 0% on.  Any bytes after the first 3 are transmitted from the 3.3 pin, as well as the first byte being the size of the new transmitted packet.
