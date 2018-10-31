#include <TinyWireM_MMA8451.h>
//#include <Adafruit_NeoPixel.h>

float now = 0;
int dlyAmt = 1000;
int flashDelay = 100;

int ledPin = 1;
//int pixelCount = 16;

Adafruit_MMA8451 mma = Adafruit_MMA8451();
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelCount, ledPin, NEO_GRB + NEO_KHZ800);

const int seqLength = 5;
int seq[seqLength] = {1, 2, 3, 2, 3};

int currentState = 0;
const int accelLength = 10;
int accelState[accelLength] = {0,0,0,0,0,0,0,0,0,0};

void setup() {
  // Configure the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Flash the LED five times to indicate start up
  flash(5);
  // Wait before proceeding
  delay(dlyAmt);

  // Check for accelerometer connection
  while (!mma.begin(0x1D)) {
    // If no accelerometer at default address, flash 10 times
    flash(10);
    // Wait before trying again
    delay(dlyAmt);
  }

  // Initialise Pixels library
//  pixels.begin();

  // Set start time
  now = millis();
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
    
    if (max3(x, y, z) == x) { currentState = 1; };
    if (max3(x, y, z) == y) { currentState = 2; };
    if (max3(x, y, z) == z) { currentState = 3; };

    // If state has changed
    if ( currentState != accelState[0] ) {
      // Update the previous states
      // For each item in the accelState array, starting at the second to last item
      // and moving back through the list to the first item
      for (int i = accelLength-1; i>=0; i--) {
        // Set the next item in the array to the value of this item
        accelState[i+1] = accelState[i];
      }
      // Update the first item in the array to the current state
      accelState[0] = currentState;

      // Compare the latest part of the accelStates array to the seq array
      if ( checkSequence(seq, 3) ) {
        // If they match, show success indication
//        magicPixelSeq();
        flash(5);
      }
      if ( checkSequence(seq, 4) ) {
        flash(6);
      }
      if ( checkSequence(seq, 5) ) {
        flash(8);
      }
    }
    
    flash(currentState);
//    updatePixels();
  }
}

void flash(int flashCount) {
    for ( int i = 0; i<flashCount; i++ ) {
      digitalWrite(ledPin, HIGH);
      delay(flashDelay);
      digitalWrite(ledPin, LOW);
      delay(flashDelay);
    }
}

float max3(float a, float b, float c) {
  return max(max(a,b),c);
}

boolean checkSequence(int *array_a, int array_len){
  // Compare array elements. If different, return false
  for ( int i = 0; i<array_len; i++ ) {
    if ( array_a[i] != accelState[array_len - i - 1] ) {  // As accelState is ordered newest to oldest, reverse iteration order for check 
      return false;
    }
  }
  return true;
}

//void updatePixels(void) {
//    for(int i=0; i<pixelCount ;i++){
//
//    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
//    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
//
//    pixels.show(); // This sends the updated pixel color to the hardware.
//  }
//}

void magicPixelSeq(void) {
  // Stub for visual effect on success
  flash(6);
  // updatePixels();
}


