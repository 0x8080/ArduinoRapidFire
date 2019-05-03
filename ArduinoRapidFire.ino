#define USBCON

enum {  // Will probably need at some point
    STATE_INPUT  = 0x0,
    STATE_OUTPUT = 0x1
  };

struct IO { // Define our pinout here
    uint16_t trigPin : 5, trigState : 1, trigVal : 10;
  };

IO io;

int main() {
  // Inital Setup
  init();
#if defined(USBCON)
  USBDevice.attach();
#endif
  Serial.begin(9600);

  // Start assigning our pins
  io.trigPin = A0;
  
  for(;;) {
    pinMode(io.trigPin, INPUT);
    io.trigVal = analogRead(io.trigPin);
    Serial.println(io.trigVal);

    if(io.trigVal > 250) {
      pinMode(io.trigPin, OUTPUT);
      digitalWrite(io.trigPin, HIGH);
      Serial.println("Fire!");
      delay(500);
    }
  }
}
