// Note that this code has only been set up for XBOX 360 Gampads ONLY as of right now.
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

// Pins
int DPAD_IN[4] = {2, 3, 6, 5}; // DPAD Input Pins {UP, DOWN, LEFT RIGHT}
int ABXY_IN[4]= {9, 10, 8, 7}; // ABXY Input Pins {A, B, X, Y}
int R_TRIGGER_IN = A3,L_TRIGGER_IN = A2; // Trigger Input Pins
  int R_TRIGGER_OUT = 4; // Trigger Output Pins
int R_BUMPER_IN = 11, L_BUMPER_IN = 12; // Bumper Input Pins

int LED = 13; // On-Board LED

int R_TRIGGER_VAL, L_TRIGGER_VAL;
  int R_TRIGGER, L_TRIGGER;
bool L_BUMPER, R_BUMPER;
bool A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON;
bool DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT;

// Other Variables
int Debug_Max = 4;

int Debug = 0;

unsigned const int Profiles = 10;   // Number of Profiles (RapidFire)

unsigned int Profile = 0;  // Current Profile (RapidFire)

int DELAY[Profiles][2] = { {40, 30} , {90, 30} } ;   // Array containing delay timings for each profile (RapidFire)

void setup() 
  {
  // Setting up Pins
  for ( int i = 0; i < 4; i++)
    {
      pinMode(ABXY_IN[i], INPUT);
        pinMode(ABXY_IN[i], LOW); // Enables Pull-Down Resistor to prevent value from floating
    
      pinMode(DPAD_IN[i], INPUT);
        pinMode(DPAD_IN[i], LOW);
    }
    
  pinMode(R_BUMPER_IN, INPUT);
    pinMode(R_BUMPER_IN, LOW);
  pinMode(L_BUMPER_IN, INPUT);
    pinMode(L_BUMPER_IN, LOW);
        
  pinMode(R_TRIGGER_IN, INPUT);
    pinMode(R_TRIGGER_OUT, OUTPUT);
  pinMode(L_TRIGGER_IN, INPUT);
  
  // Initializing Serial Comunication
  Serial.begin(9600);

  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
    {
      u8g.setColorIndex(255);     // white
    }
    else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) 
      {
        u8g.setColorIndex(3);         // max intensity
      }
    else if ( u8g.getMode() == U8G_MODE_BW ) {
        u8g.setColorIndex(1);         // pixel on
      }
    else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
        u8g.setHiColorByRGB(255,255,255);
      }

      InitDisplay();
  }


void loop() 
  {
    GrabInputs(); // Reads and stores values from I/O
    
    RapidFire(R_TRIGGER, 30, R_TRIGGER_OUT); // Trigger, Min Threshold, Trigger Output
    
    SetDelay(L_TRIGGER, 30, DPAD_UP, DPAD_DOWN, DPAD_RIGHT, DPAD_LEFT, 5, 500, 0, 1); // Trigger Input, Delay_In_Up Button, Delay_In_Down Button, Delay_Out_Up Button, Delay_Out_Down Button, MinDelay, MaxDelay, Print via Serial, Print via IIC
    
    SetProfile(L_TRIGGER, 30, Y_BUTTON, X_BUTTON, 0, 1); // Debug, Trigger Input, Min Trigger Input Value, Button Input, Macro Next Profile, Macro Prev Profile, Print via Serial, Print via IIC

    SetDebug(L_TRIGGER, 30, R_BUMPER, L_BUMPER, 0, 1);

    if (Debug > 0)
      {
        PrintStats(0, 1);  
      }
  }

void InitDisplay()
  {
    u8g.firstPage();  
    do {
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr( 20, 20, "Welcome to");
      u8g.drawStr( 21, 40, "RapiDuino!");
    } while( u8g.nextPage() );
    delay(2000);
    PrintStats(0, 1);  
  }


void RapidFire(int TRIGGER_INPUT, unsigned int i, int TRIGGER_OUTPUT) {
    if (TRIGGER_INPUT > i)
    {
        digitalWrite(TRIGGER_OUTPUT, LOW); // Sends signal that the trigger is pressed
          delay(DELAY[Profile][0]);
          
        digitalWrite(TRIGGER_OUTPUT, HIGH); // Sends signal that the trigger is released
          delay(DELAY[Profile][1]);
     }
  }


void SetDelay(int TRIGGER_INPUT, unsigned int i, bool DELAY_IN_UP, bool DELAY_IN_DOWN, bool DELAY_OUT_UP, bool DELAY_OUT_DOWN, int MinDelay, int MaxDelay, bool PrintSerial, bool PrintIIC)
  {   
    if (TRIGGER_INPUT > i && DELAY_IN_UP == LOW && DELAY[Profile][0] < MaxDelay) // Checks for if the L-Trigger & DPAD-UP is being pressed and that DELAY_IN in below MaxDelay
      {
        DELAY[Profile][0] += 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(25);
      }
    
    if (TRIGGER_INPUT > i && DELAY_IN_DOWN == LOW && DELAY[Profile][0] > MinDelay) // Checks for if the L-Trigger & DPAD-DOWN is being pressed and that DELAY_IN in above MinDelay
      {
        DELAY[Profile][0] -= 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(25);
      }
      
    if (TRIGGER_INPUT > i && DELAY_OUT_UP == LOW && DELAY[Profile][1] < MaxDelay) // Checks for if the L-Trigger & DPAD-UP is being pressed and that DELAY_OUT in below MaxDelay
      {
        DELAY[Profile][1] += 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(25);
      }
  
    if (TRIGGER_INPUT > i && DELAY_OUT_DOWN == LOW && DELAY[Profile][1] > MinDelay) // Checks for if the L-Trigger & DPAD-DOWN is being pressed and that DELAY_IN in above MinValue
      {
        DELAY[Profile][1] -= 5;
          
        PrintStats(PrintSerial, PrintIIC);
          
        delay(25);
      }
 }
 

void SetProfile(int TRIGGER_INPUT, unsigned int i, bool PROFILE_NEXT, bool PROFILE_BACK, bool PrintSerial, bool PrintIIC)
  {
    if (PROFILE_NEXT == LOW && TRIGGER_INPUT > i && Profile <= Profiles)
      {
        Profile++;
        
        if (Profile == Profiles)
          {
            Profile = 0;
          }
        
        PrintStats(PrintSerial, PrintIIC);
         
        delay(100);
      }
      
    if (PROFILE_BACK == LOW && TRIGGER_INPUT > i && Profile >= 0)
      {
        if (Profile == 0)
          {
            Profile = Profiles;
            Profile--;
          }
          
        else if (Profile > 0)
          {
            Profile--;
          }
          
        PrintStats(PrintSerial, PrintIIC);
         
        delay(100);
      }
  }


void SetDebug(int TRIGGER_INPUT, unsigned int i, bool INC_DEBUG, bool DEC_DEBUG, bool PrintSerial, bool PrintIIC)
  {
    if (INC_DEBUG == LOW && TRIGGER_INPUT > i && Debug <= Debug_Max)
      {
        Debug++;
        
        if (Debug > Debug_Max)
          {
            Debug = 0;
          }
        
        PrintStats(PrintSerial, PrintIIC);
         
        delay(100);
      }
      
    if (DEC_DEBUG == LOW && TRIGGER_INPUT > i && Debug >= 0)
      {
        if (Debug == 0)
          {
            Debug = Debug_Max;
            Debug--;
          }
          
        else if (Debug > 0)
          {
            Debug--;
          }
          
        PrintStats(PrintSerial, PrintIIC);
         
        delay(100);
      }
  }

void PrintStats(bool PrintSerial, bool PrintIIC)
  {
    int RPM = 60000 / (DELAY[Profile][0] + DELAY[Profile][1]);
    
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

          Serial.print("\t");

        Serial.print("Rate of Fire: ");
          Serial.print(RPM);
          Serial.println("RPM");

        switch (Debug)
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
      }


    if (PrintIIC == 1)
      {
          u8g.firstPage();  
          do 
            {
              u8g.setFont(u8g_font_u8glib_4);
              u8g.setPrintPos(0, 5);
                u8g.print("Profile ");
                u8g.print(Profile);

              u8g.setPrintPos(2, 15);
                u8g.print("Delay In = ");
                u8g.print(DELAY[Profile][0]);
                u8g.print("ms");
                
              u8g.setPrintPos(2, 20);
                u8g.print("Delay Out = ");
                u8g.print(DELAY[Profile][1]);
                u8g.print("ms");
              
              u8g.setPrintPos(2, 25);
                u8g.print(RPM);
                u8g.print(" RPM");

              switch(Debug)
                {
                  case 0 : break;
                  
                  case 1 :  u8g.setPrintPos(64, 5);
                              u8g.print("DEBUG: ");
                              u8g.print(Debug);
                              
                            u8g.setPrintPos(66, 15);
                              u8g.print("Right Trigger: ");
                              u8g.print(R_TRIGGER);
                              
                            u8g.setPrintPos(66, 20);
                              u8g.print("Left Trigger: ");
                              u8g.print(L_TRIGGER);
                              
                            break;
                            
                  case 2 :  u8g.setPrintPos(64, 5);
                              u8g.print("DEBUG: ");
                              u8g.print(Debug);

                            u8g.setPrintPos(66, 15);
                              u8g.print("A: ");
                              u8g.print(A_BUTTON);

                            u8g.setPrintPos(66, 20);
                              u8g.print("B: ");
                              u8g.print(B_BUTTON);
                              
                            u8g.setPrintPos(66, 25);
                              u8g.print("X: ");
                              u8g.print(X_BUTTON);
                            
                            u8g.setPrintPos(66, 30);
                              u8g.print("Y: ");
                              u8g.print(Y_BUTTON);
                            
                            break;
                            
                  case 3 :  u8g.setPrintPos(64, 5);
                              u8g.print("DEBUG: ");
                              u8g.print(Debug);

                            u8g.setPrintPos(66, 15);
                              u8g.print("Right Bumper: ");
                              u8g.print(R_BUMPER);

                            u8g.setPrintPos(66, 20);
                              u8g.print("Left Bumper: ");
                              u8g.print(L_BUMPER);

                            break;
                  case 4 :  u8g.setPrintPos(64, 5);
                              u8g.print("DEBUG: ");
                              u8g.print(Debug);

                            u8g.setPrintPos(66, 15);
                              u8g.print("DPAD Up: ");
                              u8g.print(DPAD_UP);

                            u8g.setPrintPos(66, 20);
                              u8g.print("DPAD Down: ");
                              u8g.print(DPAD_DOWN);
                              
                            u8g.setPrintPos(66, 25);
                              u8g.print("DPAD Left: ");
                              u8g.print(DPAD_LEFT);
                            
                            u8g.setPrintPos(66, 30);
                              u8g.print("DPAD Right: ");
                              u8g.print(DPAD_RIGHT);

                            break;
                }
            } while( u8g.nextPage() );
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
    A_BUTTON = digitalRead(ABXY_IN[0]);
    B_BUTTON = digitalRead(ABXY_IN[1]);
    X_BUTTON = digitalRead(ABXY_IN[2]);
    Y_BUTTON = digitalRead(ABXY_IN[3]);
    
    // Reading values from DPAD
    DPAD_UP = digitalRead(DPAD_IN[0]); // Returns HIGH when not pressed and LOW when pressed
    DPAD_DOWN = digitalRead(DPAD_IN[1]); // Returns HIGH when not pressed and LOW when pressed
    DPAD_LEFT = digitalRead(DPAD_IN[2]); // Returns HIGH when not pressed and LOW when pressed
    DPAD_RIGHT = digitalRead(DPAD_IN[3]); // Returns HIGH when not pressed and LOW when pressed
  }
