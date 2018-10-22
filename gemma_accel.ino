#include <TinyWireM_MMA8451.h>
#include <Adafruit_NeoPixel.h>

float now = 0;
int dlyAmt = 1000;
int flashDelay = 100;

int ledPin = 1;
int pixelCount = 16;

Adafruit_MMA8451 mma = Adafruit_MMA8451();
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelCount, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
  // Configure the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Flash the LED five times to indicate start up
  flash(5);
  // Wait before proceeding
  delay(dlyAmt);

  // Check for accelerometer connection
  while (!mma.begin()) {
    // If no accelerometer, flash 10 times
    flash(10);
    // Wait before trying again
    delay(dlyAmt);
  }

  // Initialise Pixels library
  pixels.begin();

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

    // Set the number of flashes to 0
    int flashCount = 0;
    // Set number of flashes by largest acceleration:
    // 1 flash if X has largest acceleration
    // 2 flashes if Y has largest acceleration
    // 3 flashes if Z has largest acceleration
    
    if (max3(x, y, z) == x) { flashCount = 1; };
    if (max3(x, y, z) == y) { flashCount = 2; };
    if (max3(x, y, z) == z) { flashCount = 3; };

    flash(flashCount);
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

void updatePixels(void) {
    for(int i=0; i<pixelCount ;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

