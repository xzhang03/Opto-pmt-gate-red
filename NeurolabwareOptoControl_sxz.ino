
// Simple e optogenetics pulse generator DLR 10/30/17

#define CPMS 3108    // how many counts in 1ms (have to measure for each board)

const byte ledPin = 13;
const byte interruptPin = 2;
const byte trigPin = 8;
const byte gatePin = 9;
const byte mlPin = 6;
const byte ttlWait = 10;
const byte redtrigPin = 4; // Pin for Red LED trigger

unsigned long int ttlTime = 0;
bool ttlOn = false;

volatile byte pulse = 0; // number of pulses to deliver
volatile byte width = 10; // width of pulses in msec
volatile byte margin = 2; // offset margin in ms
volatile byte pulseCount = 0; // pulses left to deliver

volatile byte command, val;
volatile int i;

void setup() {
  pinMode(redtrigPin, OUTPUT); // Define red LED pin as output
  pinMode(ledPin, OUTPUT);
  // pinMode(trigPin, OUTPUT);
  pinMode(gatePin, OUTPUT);
  pinMode(mlPin, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  
  digitalWrite(ledPin, LOW);
  digitalWrite(gatePin, LOW);
  // digitalWrite(trigPin, LOW);
  digitalWrite(redtrigPin, LOW); // Set red LED pin as low
  Serial.begin(38400);
  attachInterrupt(digitalPinToInterrupt(interruptPin), trig, FALLING);
}

void loop() {
  if (Serial.available() >= 2) {
    command = Serial.read();
    val = Serial.read();
    switch (command) {        // parse commands...
      case 0:
        pulse = val;
        break;
      case 1:
        width = val;
        break;
      case 2:
        digitalWrite(trigPin, HIGH);
        break;
      case 3:
        digitalWrite(trigPin, LOW);
        break;
      case 4:
        margin = val;
        break;
      case 5:
        digitalWrite(ledPin, HIGH);
        break;
      case 6:
        digitalWrite(ledPin, LOW);
        break;
      default:
      case 255:
        pulseCount = pulse;
        break;
    }
  }
  
  if (millis() - ttlTime > ttlWait) {
    if (ttlOn && !digitalRead(mlPin)) {
      ttlOn = false;
      ttlTime = millis();
    }
    else if (!ttlOn && digitalRead(mlPin)) {
      pulseCount = pulse;
      ttlOn = true;
      ttlTime = millis();
    }
  }
}

// Note that some Arduino boards cannot use delayMicroseconds() in an interrupt routine
// If this is this case for yorus, you have to switch the digitalMicroseconds line with
// the for loop, which will provide the delay.  You will have to calibrate this in terms
// the number of counts per msec required.  This is then the value you can use to define
// the CPMS constant above.

void trig() {
  if (pulseCount > 0) { // note that delayMicroseconds() does not work in ISR
    digitalWrite(gatePin, HIGH);
    // digitalWrite(trigPin, HIGH);
    digitalWrite(redtrigPin, HIGH); // Write red LED pin high
    delayMicroseconds(1000*width);
    //for (i = 0; i < width * CPMS; i++); // delay
    // digitalWrite(trigPin, LOW);
    digitalWrite(redtrigPin, LOW);  // Write red LED pin low
    delayMicroseconds(1000*margin);
    //for (i = 0; i < margin * CPMS; i++); // delay
    digitalWrite(gatePin, LOW);
    pulseCount--;
  }
}
