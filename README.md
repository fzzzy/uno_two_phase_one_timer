Arduino Uno Two Phase Rectangle Wave Generator
==============================================

using only one timer, the 16 bit Timer1
---------------------------------------

Useful as an H Bridge driver, or for any other application
which requires a rectangle wave up to 100khz of up to just under
50% duty cycle and one or two channels.

This program uses two Bourns PEC12-4225F-N0024 rotary encoders,
one for controlling frequency and the other for controlling
duty cycle. Hook both C pins to ground, the A and B pins of
one to digital inputs 2 and 3, and the A and B pins of the other
to digital inputs 4 and 5. To change the direction the pot is
turned to increase the frequency or duty cycle, swap the
A and B pins for that pot.

The outputs are on digital pins 9 and 10. The two outputs are
always 180 degrees apart, and will never overlap. This limits
the maximum duty cycle of each channel to just under 50%.