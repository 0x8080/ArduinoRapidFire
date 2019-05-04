// Allow Serial to be enable / disabled
#ifdef DEBUG
  #define SERIAL_BEGIN(baud) Serial.begin(9600)
  #define SERIAL_PRINTLN(str) Serial.println(str)
#else
  #define SERIAL_BEGIN(baud)
  #define SERIAL_PRINTLN(str)
#endif

enum {  // Will probably need at some point
    STATE_INPUT  = 0x0,
    STATE_OUTPUT = 0x1
  };

struct IO { // Store pin map, state, and values here
    uint16_t trigPin : 5, trigState : 1, trigVal : 10;

    IO() :trigPin(A0), trigState(STATE_INPUT), trigVal(0) {}
  };

IO io;

int main() {
  // Inital Setup
  init();
#ifdef USBCON
  USBDevice.attach();
#endif
  SERIAL_BEGIN(9600);

  // Start assigning our pins
  io.trigPin = A0;
  
  for(;;) {
    pinMode(io.trigPin, INPUT);
    io.trigVal = analogRead(io.trigPin);
    SERIAL_PRINTLN(io.trigVal);

    if(io.trigVal > 250) {
      pinMode(io.trigPin, OUTPUT);
      digitalWrite(io.trigPin, HIGH);
      SERIAL_PRINTLN("Fire!");
      delay(500);
    }
  }
}
