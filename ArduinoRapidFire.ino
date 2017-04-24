// Note that this code has only been set up for XBOX 360 Gampads ONLY as of right now.

#include <SPI.h>
#include <Wire.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Pins
int DPAD_UP_IN = 2, DPAD_DOWN_IN = 3, DPAD_LEFT_IN = 6, DPAD_RIGHT_IN = 5; // DPAD Input Pins
int A_BUTTON_IN= 9, B_BUTTON_IN = 10, X_BUTTON_IN = 8, Y_BUTTON_IN = 7; // ABXY Input Pins
  int A_BUTTON_OUT = A0, B_BUTTON_OUT = A1, X_BUTTON_OUT = A6, Y_BUTTON_OUT = A7; // ABXY Output Pins

int R_TRIGGER_IN = A3,L_TRIGGER_IN = A2; // Trigger Input Pins
  int R_TRIGGER_OUT = 4; // Trigger Output Pins

int R_BUMPER_IN = 11, L_BUMPER_IN = 12; // Bumper Input Pins

int LED = 13; // On-Board LED
    
// Input Values from L & R Triggers
int R_TRIGGER_VAL, L_TRIGGER_VAL;
  int R_TRIGGER, L_TRIGGER;
  
// Input Values from Bumpers
int L_BUMPER, R_BUMPER;
  
// Input values from ABXY
int A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON;

// Input values from DPAD
int DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT;

// Other Variables
const int Profiles = 2;   // Number of Profiles (RapidFire)
const int Macro_Profiles = 2; // Number of Profiles (Macro)

int Profile = 0;  // Current Profile (RapidFire)
int Macro_Profile = 0; // Current Profile (Macro)

int DELAY[Profiles][2] = { {40, 30} , {90, 30} } ;   // Array containing delay timings for each profile (RapidFire)
int MACRO_DELAY[Macro_Profiles][2] = { {30, 30} , {30, 30} }; // Array containing delay timings for each profile (Macro)



void setup() 
  {
  // Setting up Pins
  pinMode(A_BUTTON_IN, INPUT);
    pinMode(A_BUTTON_IN, LOW); // Enables Pull-Down Resistor to prevent value from floating
      pinMode(A_BUTTON_OUT, OUTPUT);
  pinMode(B_BUTTON_IN, INPUT);
    pinMode(B_BUTTON_IN, LOW);
      pinMode(B_BUTTON_OUT, OUTPUT);
  pinMode(X_BUTTON_IN, INPUT);
    pinMode(X_BUTTON_IN, LOW);
      pinMode(X_BUTTON_OUT, OUTPUT);
  pinMode(Y_BUTTON_IN, INPUT);
    pinMode(Y_BUTTON_IN, LOW);
      pinMode(Y_BUTTON_OUT, OUTPUT);
  
  pinMode(DPAD_UP_IN, INPUT);
    pinMode(DPAD_UP_IN, LOW);
  pinMode(DPAD_DOWN_IN, INPUT);
    pinMode(DPAD_DOWN_IN, LOW);
  pinMode(DPAD_LEFT_IN, INPUT);
    pinMode(DPAD_LEFT_IN, LOW);    
  pinMode(DPAD_RIGHT_IN, INPUT);
    pinMode(DPAD_RIGHT_IN, LOW);
    
  pinMode(R_BUMPER_IN, INPUT);
    pinMode(R_BUMPER_IN, LOW);
  pinMode(L_BUMPER_IN, INPUT);
    pinMode(L_BUMPER_IN, LOW);
        
  pinMode(R_TRIGGER_IN, INPUT);
    pinMode(R_TRIGGER_OUT, OUTPUT);
  pinMode(L_TRIGGER_IN, INPUT);
  
  // Initializing Serial Comunication
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);
  display.clearDisplay();
  }



void loop() 
  {
    GrabInputs(); // Reads and stores values from I/O
    
    RapidFire(R_TRIGGER, 30, R_TRIGGER_OUT); // Trigger, Min Threshold, Trigger Output
    
    SetDelay(L_TRIGGER, 30, DPAD_UP, DPAD_DOWN, DPAD_RIGHT, DPAD_LEFT, 10, 300, 1, 0); // Trigger Input, Delay_In_Up Button, Delay_In_Down Button, Delay_Out_Up Button, Delay_Out_Down Button, MinDelay, MaxDelay, Print via Serial, Print via IIC
    
    SetProfile(L_TRIGGER, 30, Y_BUTTON, X_BUTTON, R_BUMPER, L_BUMPER, 1, 0); // Trigger Input, Min Trigger Input Value, Button Input, Macro Next Profile, Macro Prev Profile, Print via Serial, Print via IIC
    
    PrintDebug(0, 300); // 0 = Off 1 = Triggers 2 = ABXY 3 = Bumpers 4 = DPAD, Refresh Delay
  }



void RapidFire(int TRIGGER_INPUT, int i, int TRIGGER_OUTPUT)
  { 
    if (TRIGGER_INPUT > i)
    {
      digitalWrite(TRIGGER_OUTPUT, LOW); // Sends signal that the trigger is pressed
        delay(DELAY[Profile][0]);
        
      digitalWrite(TRIGGER_OUTPUT, HIGH); // Sends signal that the trigger is released
        delay(DELAY[Profile][1]);
    }
  }



void SetDelay(int TRIGGER_INPUT, int i, int DELAY_IN_UP, int DELAY_IN_DOWN, int DELAY_OUT_UP, int DELAY_OUT_DOWN, int MinDelay, int MaxDelay, bool PrintSerial, bool PrintIIC)
  {   
    if (TRIGGER_INPUT > i && DELAY_IN_UP == LOW && DELAY[Profile][0] < MaxDelay) // Checks for if the L-Trigger & DPAD-UP is being pressed and that DELAY_IN in below MaxDelay
      {
        DELAY[Profile][0] += 10;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(300);
      }
    
    if (TRIGGER_INPUT > i && DELAY_IN_DOWN == LOW && DELAY[Profile][0] > MinDelay) // Checks for if the L-Trigger & DPAD-DOWN is being pressed and that DELAY_IN in above MinDelay
      {
        DELAY[Profile][0] -= 10;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(300);
      }
      
    if (TRIGGER_INPUT > i && DELAY_OUT_UP == LOW && DELAY[Profile][1] < MaxDelay) // Checks for if the L-Trigger & DPAD-UP is being pressed and that DELAY_OUT in below MaxDelay
      {
        DELAY[Profile][1] += 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(300);
      }
  
    if (TRIGGER_INPUT > i && DELAY_OUT_DOWN == LOW && DELAY[Profile][1] > MinDelay) // Checks for if the L-Trigger & DPAD-DOWN is being pressed and that DELAY_IN in above MinValue
      {
        DELAY[Profile][1] -= 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(300);
      }
 }
 
 

void SetProfile(int TRIGGER_INPUT, int i, int PROFILE_NEXT, int PROFILE_BACK, int MACRO_PROFILE_NEXT, int MACRO_PROFILE_BACK, bool PrintSerial, bool PrintIIC)
  {
    if (PROFILE_NEXT == LOW && TRIGGER_INPUT > i && Profile <= Profiles)
      {
        Profile++;
        
        if (Profile == Profiles)
          {
            Profile = 0;
          }
        
        PrintStats(PrintSerial, PrintIIC);
         
        delay(300);
      }
      
    if (PROFILE_BACK == LOW && TRIGGER_INPUT > i && Profile > 0)
      {
        Profile--;
        
        PrintStats(PrintSerial, PrintIIC);
         
        delay(300);
      }
      
    if (TRIGGER_INPUT > i && MACRO_PROFILE_NEXT == LOW && Macro_Profile < Macro_Profiles)
      {
        Macro_Profile++;
        
        if (Macro_Profile == Macro_Profiles)
          {
            Macro_Profile = 0;
          }
        
        PrintMacroStats(PrintSerial, PrintIIC);
        
        delay(300);
      }
      
    if (TRIGGER_INPUT > i && MACRO_PROFILE_BACK == LOW && Macro_Profile > 0)
      {
        Macro_Profile--;
        
        PrintMacroStats(1, 0);
        
        delay(300);
      }
  }
  


void PrintStats(bool PrintSerial, bool PrintIIC)
  {  
    if (PrintSerial == 1)
      {
        Serial.print("Profile ");
          Serial.println(Profile);
          
          Serial.print("\t");
          
        Serial.print("Delay_In = ");
          Serial.println(DELAY[Profile][0]);
          
          Serial.print("\t");
          
        Serial.print("Delay_Out = ");
          Serial.println(DELAY[Profile][1]);
      }

    if (PrintIIC == 1)
      {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
    
        display.print("Profile ");
          display.println(Profile);
          
          display.print("\t");
          
        display.print("Delay_In = ");
          display.println(DELAY[Profile][0]);
          
          display.print("\t");
          
        display.print("Delay_Out = ");
          display.println(DELAY[Profile][1]);
        
        display.display();
      } 
  }



void PrintDebug(int i, int REFRESH_DELAY)
{
  switch (i)
    {
      case 1 : Serial.print("Right Trigger = ");
                Serial.println(R_TRIGGER);         
               Serial.print("Left Trigger = ");
                Serial.println(L_TRIGGER);      
               Serial.println();
               break;
                 
      case 2 : Serial.print("A Button = ");
                 Serial.println(A_BUTTON);
               Serial.print("B Button = ");
                 Serial.println(B_BUTTON);
               Serial.print("X Button = ");
                 Serial.println(X_BUTTON);
               Serial.print("Y Button = ");
                 Serial.println(Y_BUTTON);
               Serial.println();
               break;

      case 3 : Serial.print("R Bumper = ");
                Serial.println(R_BUMPER);
               Serial.print("L Bumper = ");
                Serial.println(L_BUMPER);
               Serial.println();
               break;

      case 4 : Serial.print("DPAD UP = ");
                Serial.println(DPAD_UP);
               Serial.print("DPAD DOWN = ");
                Serial.println(DPAD_DOWN);
               Serial.print("DPAD LEFT = ");
                Serial.println(DPAD_LEFT);
               Serial.print("DPAD RIGHT = ");
                Serial.println(DPAD_RIGHT);
               Serial.println();
               break;
    }
    
  if(i > 0)
    {
       delay(REFRESH_DELAY);
    }
}



void GrabInputs()
  {
    // Reading value from L & R Triggers
    R_TRIGGER_VAL = analogRead(R_TRIGGER_IN);
    R_TRIGGER = map(R_TRIGGER_VAL, 0, 1023, 0, 255); // Maps values from R-Trigger to a value between 0 and 255
    
    L_TRIGGER_VAL = analogRead(L_TRIGGER_IN);
    L_TRIGGER = map(L_TRIGGER_VAL, 0, 1023, 0, 255);  // Maps values from L-Trigger to a value between 0 and 255
    
    // Reading Values from L & R Bumpers
    R_BUMPER = digitalRead(R_BUMPER_IN);
    L_BUMPER = digitalRead(L_BUMPER_IN);
      
    // Reading values from ABXY
    A_BUTTON = digitalRead(A_BUTTON_IN);
    B_BUTTON = digitalRead(B_BUTTON_IN);
    X_BUTTON = digitalRead(X_BUTTON_IN);
    Y_BUTTON = digitalRead(Y_BUTTON_IN);
    
    // Reading values from DPAD
    DPAD_UP = digitalRead(DPAD_UP_IN); // Returns HIGH when not pressed and LOW when pressed
    DPAD_DOWN = digitalRead(DPAD_DOWN_IN); // Returns HIGH when not pressed and LOW when pressed
    DPAD_LEFT = digitalRead(DPAD_LEFT_IN); // Returns HIGH when not pressed and LOW when pressed
    DPAD_RIGHT = digitalRead(DPAD_RIGHT_IN); // Returns HIGH when not pressed and LOW when pressed
  }



void Macro(int TRIGGER_INPUT, int i, int BUTTON_INPUT, int BUTTON_OUTPUT, int NUM_LOOPS)
  {
    int LOOP = 0;  
    
    if (TRIGGER_INPUT > i && BUTTON_INPUT == LOW)
      {
        if (NUM_LOOPS == -1)
        {
          ButtonOutput(BUTTON_OUTPUT);
        }
        
        if (NUM_LOOPS >= 1 && LOOP <= NUM_LOOPS)
        {
          ButtonOutput(BUTTON_OUTPUT);
          
          LOOP++;
        }
      }
  }



void PrintMacroStats(bool PrintSerial, bool PrintIIC)
  {
    if (PrintSerial == 1)
      {
        Serial.print("Macro Profile ");
          Serial.println(Macro_Profile);
          
          Serial.print("\t");
          
        Serial.print("Delay_In = ");
          Serial.println(MACRO_DELAY[Macro_Profile][0]);
          
          Serial.print("\t");
          
        Serial.print("Delay_Out = ");
          Serial.println(MACRO_DELAY[Macro_Profile][1]);
      }
      
    if (PrintIIC == 1)
      {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
    
        display.print("Macro Profile ");
          display.println(Macro_Profile);
          
          display.print("\t");
          
        display.print("Delay_In = ");
          display.println(MACRO_DELAY[Macro_Profile][0]);
          
          display.print("\t");
          
        display.print("Delay_Out = ");
          display.println(MACRO_DELAY[Macro_Profile][1]);
        
        display.display();
      }
  }



void ButtonOutput(int BUTTON_OUTPUT)
  {
    digitalWrite(BUTTON_OUTPUT, HIGH);
      delay(MACRO_DELAY[Macro_Profile][0]);
    digitalWrite(BUTTON_OUTPUT, LOW);
      delay(MACRO_DELAY[Macro_Profile][1]);
  }
