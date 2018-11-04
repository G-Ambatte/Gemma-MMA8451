#include <TinyWireM_MMA8451.h>
//#include <Adafruit_NeoPixel.h>

unsigned long now = 0;
unsigned long now_flash = 0;
int dlyAmt = 250;
int flashDelay = 100;

int ledPin = 1;
//int pixelCount = 16;

Adafruit_MMA8451 mma = Adafruit_MMA8451();
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelCount, ledPin, NEO_GRB + NEO_KHZ800);

const int seqLength = 7;
int seq[seqLength] = {1, 3, 1, 3, 1, 3, 2};

int currentState = 0;
const int accelLength = 10;
int accelState[accelLength] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Create state buffer for LED states
// First part holds pin state (HIGH or LOW)
// Second part holds duration (<1000ms)
const int pinStateLength = 20;
int pinStates[2][pinStateLength] = {};

void setup() {
  // Configure the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Blank the pinStates array to knoew defaults
  for (int i = 0; i < pinStateLength; i++) {
    pinStates[0][i] = 0;
    pinStates[1][i] = 0;
  }

  // Flash the LED five times to indicate start up
  flash(5, 100);
  // Wait before proceeding
  delay(dlyAmt);

  // Check for accelerometer connection
  while (!mma.begin(0x1D)) {
    // If no accelerometer at default address, flash 10 times
    flash(10, 100);
    // Wait before trying again
    delay(dlyAmt);
  }

  // Initialise Pixels library
  //  pixels.begin();

  // Set start time
  now = millis();
  now_flash = millis();
}

void loop() {
  // If the timer has expired
  if (millis() > now + dlyAmt) {
    // Reset the timer
    now = millis();

    // Read the current accelerometer state
    mma.read();

    /* Get a new sensor event */
    sensors_event_t event;
    mma.getEvent(&event);

    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;

    // Get absolute values of X, Y, and Z
    x = abs(x);
    y = abs(y);
    z = abs(z);

    // Set state by largest acceleration:
    // = 1 if X has largest acceleration
    // = 2 if Y has largest acceleration
    // = 3 if Z has largest acceleration
    // State definition is arbitrary and whatever we set it to
    // However the more states we have, the more likely we are to hit incorrect states and fail to execute a sequence

    if (max3(x, y, z) == x) {
      currentState = 1;
    };
    if (max3(x, y, z) == y) {
      currentState = 2;
    };
    if (max3(x, y, z) == z) {
      currentState = 3;
    };

    // If state has changed
    if ( currentState != accelState[0] ) {
      // Update the previous states
      // For each item in the accelState array, starting at the second to last item
      // and moving back through the list to the first item
      for (int i = accelLength - 1; i >= 0; i--) {
        // Set the next item in the array to the value of this item
        accelState[i + 1] = accelState[i];
      }
      // Update the first item in the array to the current state
      accelState[0] = currentState;

      // Set the LED to flash the current state number
      if ( pinStates[1][0] == 0 ) { updatePinStates(currentState, 250); };

      flashDelay = 100;
      
      // Compare the latest part of the accelStates array to the seq array
      // Flash if sequence is correct
      if ( checkSequence(seq, 5) ) {
        updatePinStates(4, 500);
//        currentState = 4;
//        flashDelay = 250;
      }
      if ( checkSequence(seq, 6) ) {
        updatePinStates(6, 250);
//        currentState = 6;
//        flashDelay = 150;
      }
      if ( checkSequence(seq, 7) ) {
        updatePinStates(8, 100);
//        currentState = 8;
//        flashDelay = 75;
      }
      
    }

//    flash(currentState, flashDelay);
  }
  checkFlash();
}

void flash(int flashCount, int dly) {
  for ( int i = 0; i < flashCount; i++ ) {
    digitalWrite(ledPin, LOW);
    delay(dly);
    digitalWrite(ledPin, HIGH);
    delay(dly);
  }
}

float max3(float a, float b, float c) {
  return max(max(a, b), c);
}

boolean checkSequence(int *array_a, int array_len) {
  // Compare array elements. If different, return false
  for ( int i = 0; i < array_len; i++ ) {
    if ( array_a[i] != accelState[array_len - i - 1] ) {  // As accelState is ordered newest to oldest, reverse iteration order for check
      return false;
    }
  }
  return true;
}

void checkFlash(void) {
  // Check for LED timer expiry
  if ((pinStates[1][0] > 0) && (millis() > now_flash + pinStates[1][0])) {
    // Advance all states in the buffer
    for (int i = 0 ; i < pinStateLength - 1; i++) {
      pinStates[0][i] = pinStates[0][i+1];
      pinStates[1][i] = pinStates[1][i+1];
    }
    // Set the last state to defaults
    pinStates[0][pinStateLength - 1] = 0;
    pinStates[1][pinStateLength - 1] = 0;

    // Update the pin with the new state
    if ( pinStates[0][0] == 0 ) {
      digitalWrite(ledPin, HIGH);
//      flash(1,250);
    } else {
      digitalWrite(ledPin, LOW);
//      flash(2,100);
    }
    if ( pinStates[0][1] == 0 ) {
      digitalWrite(ledPin, LOW);
    }
    now_flash = millis();
//    flash(1,250);
  }
}

void updatePinStates(int count, int duration) {
  for (int i = 0; i < count; i++) {
        pinStates[0][(i*2)] = 1;
        pinStates[1][(i*2)] = duration;
        pinStates[0][(i*2)+1] = 0;
        pinStates[1][(i*2)+1] = duration;
  }  
}



