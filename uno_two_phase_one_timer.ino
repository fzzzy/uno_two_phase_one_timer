/* Arduino Uno Two Phase Rectangle Wave Generator
   using only one timer, the 16 bit Timer1

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
*/

// Frequency counter
volatile int count_a = 0;
volatile int new_a = 500;
// Duty cycle counter
volatile int count_b = 0;
volatile int new_b = 100;
// Rotary knobs state
volatile int old_state = 0;
volatile int state = 0;

// Used to limit the frequency of timer updates to 30 times per sec
int old_time = 0;

void setup() {
  // Pin 9 is one phase
  pinMode(9, OUTPUT);
  // Pin 10 is the other phase, 180 degrees apart from the first
  pinMode(10, OUTPUT);

  // Pins 2, 3, 4, 5 used for rotary encoders
  // Calling digitalWrite(x, HIGH) on the pin activates the internal
  // pull-up resistor, defaulting that pin to high. Connecting the pin
  // to ground through the C pin of the rotary encoder causes the input
  // to go low.
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);
  pinMode(5, INPUT);
  digitalWrite(5, HIGH);

  noInterrupts();
  // ICR1 is TOP in Frequency and Phase correct PWM
  ICR1 = new_a;
  // Compare match register for first phase
  OCR1A = new_b;
  // Compare match register for second phase
  OCR1B = new_a - new_b;
  // Port A normal | Port B Inverted
  TCCR1A = _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);
  // WGM13 Frequency and Phase correct PWM | CS10 No prescaler
  TCCR1B = _BV(WGM13) | _BV(CS10);

  // Pin change interrupt control register
  PCICR |= 0b100;
  
  // Pin change mask registers decide which pins are enabled as triggers
  PCMSK2 |= (_BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21));
  interrupts(); 
}

// Interrupt handler for rotary encoders
ISR(PCINT2_vect) {
    state = PIND;
    int changes = state ^ old_state;
    // If bit 2, update the frequency
    if (bitRead(changes, 2)) {
      (bitRead(state, 2) ^ bitRead(old_state, 3)) ? new_a-- : new_a++;
    }
    // If bit 4, update the duty cycle
    if (bitRead(changes, 4)) {
      (bitRead(state, 4) ^ bitRead(old_state, 5)) ? new_b-- : new_b++;
    }
    // Store the state to compare against next time
    old_state = state;
}

void loop() {
  int time = millis();
  // Limit frequency of timer updates to 30 per second
  if (time - old_time < 33) {
    return;
  }
  old_time = time;
  // Check to see if the rotary encoders changed since last time
  if (new_a != count_a || new_b != count_b) {
    // Don't let the frequency get too high
    if (new_a < 80) {
      new_a = 80;
    }
    count_a = new_a;
    count_b = new_b;
    // Calculate the duty as a percentage,
    // multiply by 64 to lower the resolution,
    // then multiply by the frequency cycles
    int trigger = ((count_b / 65536.0) * 64) * count_a;
    // Don't let the duty cycle get too short
    if (trigger < 2) {
      trigger = 2;
    }
    // Don't let the duty cycle get too long
    if (trigger > count_a / 2 - 5) {
      trigger = count_a / 2 - 5;
    }
    noInterrupts();
    // Update TOP to set the frequency
    ICR1 = count_a;
    // Set compare match for the first phase
    OCR1A = trigger;
    // Set compare match for the second phase
    OCR1B = count_a - trigger;
    interrupts();
  }
}



