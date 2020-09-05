#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

//genset and mains
const int mainsIn = A0;
//const int genIN = 12;

//relayPins
//const int realayPin9 = 9;
//const int realayPin10 = 10;
//For voltgage reading, use A0 and A1 directly in the code with analogRead()

const int voltDigi = 10;
//int mainState = 0;
//int GenState = 0;


// Rotary encoder declarations
static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
// Button reading, including debounce without delay function declarations
const byte buttonPin = 4; // this is the Arduino pin we are connecting the push button to
byte oldButtonState = HIGH;  // assume switch open because of pull-up resistor
const unsigned long debounceTime = 10;  // milliseconds
unsigned long buttonPressTime;  // when the switch last changed state
boolean buttonPressed = 0; // a flag variable
// Menu and submenu/setting declarations
byte Mode = 0;   // This is which menu mode we are in at any given time (top level or one of the submenus)
const byte modeMax = 3; // This is the number of submenus/settings you want
//unsigned int setting1 = 0;  // a variable which holds the value we set
byte setting2 = 0;  // a variable which holds the value we set
byte setting3 = 0;  // a variable which holds the value we set

//To break a loop and make timers run once
boolean abi = true;

void setup() {
  //Rotary encoder section of setup
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(0, PinA, RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  // button section of setup
  pinMode (buttonPin, INPUT_PULLUP); // setup the button pin

  pinMode(mainsIn, INPUT);
  analogWrite(mainsIn, LOW);

  pinMode(voltDigi, INPUT);
  digitalWrite(voltDigi, LOW);

  //Gen and Mains
  //pinMode(manisIN, INPUT);
  //pinMode(genIN, INPUT);

  //RelayPins
  //pinMode(relayPin9, OUTPUT);
  //pinMode(relayPin10, OUTPUT);

  lcd.init();
  //lcd.backlight();
  // DEBUGGING section of setup
  Serial.begin(9600);
}

void loop() {
  rotaryMenu();
  // carry out other loop code here

  //Read Voltage from A0
//  mainState = digitalRead(voltDigi);
  int voltValue = analogRead(mainsIn);
  float voltage = voltValue * (5.0 / 1023.0);

//  Serial.print(mainState);  
  Serial.println(voltage);

  
// IMPORTANT CODE TO CONTROL THE TIMER(S)
 if (voltage > 0.2) {
  for (;voltage > 0.2 && abi == true;) {
    Serial.println("HIGH");
    stopTimer(setting3);
    break;
  }
 } else {
  abi = true;
 }
  
//  abi = true;

//  if (voltage < 0.2 ) {   
//    Serial.print("askjdhaskdjsha");
//  } else {    
//    Serial.println("HIGH");
////    delay(1000);
//    stopTimer(setting3);   
//  }
    //LineSwitcher();
}
  // CHANGE THIS ONE (WON'T WORK AS EXPECTED)
  //void LineSwitcher() {
  //  // read the state of the pushbutton value:
  //  digitalWrite(genIN, LOW);
  //  MainsState = digitalRead(mainsIN);
  //
  //  // check if the MainsSupplybutton is pressed.
  //  // if it is, the MainsSupplyState is HIGH:
  //  if (MainsState == HIGH) {
  //    // turn Relay on:
  //    digitalWrite(RelayPin10, HIGH);
  //    digitalWrite(RelayPin9, HIGH);
  //    delay(5000);
  //  } else {
  //    // turn Relay off:
  //    digitalWrite(RelayPin9, LOW);
  //    digitalWrite(RelayPin10, LOW);
  //  }
  //}

  void rotaryMenu() { //This handles the bulk of the menu functions without needing to install/include/compile a menu library
    //DEBUGGING: Rotary encoder update display if turned
    if (oldEncPos != encoderPos) { // DEBUGGING
      lcd.backlight();
      //    Serial.println(encoderPos);
      lcd.clear();
      //    {
      //     x = encoderPos;
      //      displayMenu();
      //    }
      //    if (encoderPos == 1) {
      //      Serial.println("Status");
      //      return;
      //    }
      //    if (encoderPos == 2) {
      //      Serial.println("Start");
      //      return;
      //    }
      //    if (encoderPos == 3) {
      //      Serial.println("Stop");
      //      return;
      //    }
      lcd.print(encoderPos);
      oldEncPos = encoderPos;
    }
    // Button reading with non-delay() debounce
    byte buttonState = digitalRead (buttonPin);
    if (buttonState != oldButtonState) {
      if (millis () - buttonPressTime >= debounceTime) { // debounce
        buttonPressTime = millis ();  // when we closed the switch
        oldButtonState =  buttonState;  // remember for next time
        if (buttonState == LOW) {
          Serial.println ("Button closed");
          buttonPressed = 1;
        }
        else {
          Serial.println ("Button opened");
          buttonPressed = 0;
        }
      }  // end if debounce time up
    } // end of state change

    //Main menu section
    if (Mode == 0) {
      if (encoderPos > (modeMax + 10)) encoderPos = modeMax; // check we haven't gone out of bounds below 0 and correct if we have
      else if (encoderPos > modeMax) encoderPos = 0; // check we haven't gone out of bounds above modeMax and correct if we have

      if (buttonPressed) {
        Mode = encoderPos; // set the Mode to the current value of input if button has been pressed
        Serial.print("Mode selected: ");
        lcd.clear();
        lcd.print(" Mode :");
        lcd.print(Mode);
        Serial.println(Mode);
        buttonPressed = 0; // reset the button status so one press results in one action
        if (Mode == 1) {
          Serial.println("Mode 1");
          lcd.clear();
          lcd.print("Status");
          delay(1000);
          //encoderPos = setting1;
        }
        if (Mode == 2) {
          Serial.println("Mode 2");
          lcd.clear();
          lcd.print("Set Start Timer");
          delay(1000);
          encoderPos = setting2;
        }
        if (Mode == 3) {
          Serial.println("Mode 3");
          lcd.clear();
          lcd.print("Set Stop Timer");
          delay(1000);
          encoderPos = setting3;
        }
      }
    }
    if (Mode == 1 && buttonPressed) {
      // setting1 = encoderPos; // record whatever value your encoder has been turned to, to setting 3
      // setAdmin(1,setting1);
      //code to do other things with setting1 here, perhaps update display
      if (encoderPos > (modeMax + 10)) encoderPos = 0; // check we haven't gone out of bounds below 0 and correct if we have
      else if (encoderPos > modeMax) encoderPos = 0; // check we haven't gone out of bounds above modeMax and correct if we have
      displayStatus();

    }
    if (Mode == 2 && buttonPressed) {
      setting2 = encoderPos; // record whatever value your encoder has been turned to, to setting 2
      setAdmin(2, setting2);
      //code to do other things with setting2 here, perhaps update display

    }
    if (Mode == 3 && buttonPressed) {
      setting3 = encoderPos; // record whatever value your encoder has been turned to, to setting 3
      setAdmin(3, setting3);
      //code to do other things with setting3 here, perhaps update display
      
    }
  }

  void startTimer(unsigned int setStart) {
    while (setStart > 0) {
      lcd.home();
      lcd.backlight();
      lcd.clear();
      lcd.print(setStart);
      setStart--;
      delay(1000);
    }
    delay(1000);
    lcd.clear();
    lcd.print("MAIN MENU");
    encoderPos = 0; // reorientate the menu index - optional as we have overflow check code elsewhere
    buttonPressed = 0; // reset the button status so one press results in one action
    Mode = 0; // go back to top level of menu, now that we've set values
    oldEncPos = 0;
    delay(1000);
    lcd.noBacklight();
    delay(1000);
    lcd.clear();
    Serial.println("Main Menu"); //DEBUGGING
  }

  void stopTimer(unsigned int setStop) {
    while (setStop > 0) {
      lcd.home();
      lcd.backlight();
      lcd.clear();
      lcd.print(setStop);
      setStop--;
      delay(1000);
    }
    delay(1000);
    lcd.clear();
    lcd.print("MAIN MENU");
    encoderPos = 0; // reorientate the menu index - optional as we have overflow check code elsewhere
    buttonPressed = 0; // reset the button status so one press results in one action
    Mode = 0; // go back to top level of menu, now that we've set values
    oldEncPos = 0;
    abi = false;
    delay(1000);
    lcd.noBacklight();
    delay(1000);
    lcd.clear(); 
    Serial.println("Main Menu"); //DEBUGGING
  }

  void displayStatus () {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.backlight();
    lcd.print("Automatic Genset");
    lcd.setCursor(5, 1);
    lcd.print("Panel");
    delay(3000);
    /* if ((MainsState == HIGH) && (GenState == LOW)) {
       MainsDisplay();
      }else
      if ((GenState == HIGH) && (MainsState == LOW)){
       GenDisplay();
      } */
    GenDisplay();
    delay(1000);
    lcd.clear();
    lcd.print("MAIN MENU");
    encoderPos = 0; // reorientate the menu index - optional as we have overflow check code elsewhere
    buttonPressed = 0; // reset the button status so one press results in one action
    Mode = 0; // go back to top level of menu, now that we've set values
    oldEncPos = 0;
    delay(1000);
    lcd.noBacklight();
    delay(1000);
    lcd.clear();
    Serial.println("Main Menu"); //DEBUGGING
  }

  void MainsDisplay() {
    lcd.clear();
    lcd.home();
    lcd.print(" Mains Voltage");
    //int mainsVal = analogRead(A0);
    int mainsVal = 960;               //Just for E.g.
    float mainsVolt = mainsVal * (220.0 / 1023.0);
    lcd.setCursor(3, 1);
    lcd.print(mainsVolt);
    lcd.print(" V");
    // delay(10000);
  }

  void GenDisplay() {
    lcd.clear();
    lcd.home();
    lcd.print("Genset  Voltage");
    //int genVal = analogRead(A1);
    int genVal = 890;               //Just for E.g.
    float genVolt = genVal * (220.0 / 1023.0);
    lcd.setCursor(3, 1);
    lcd.print(genVolt);
    lcd.print(" V");
    // delay(10000);
    //  Serial.print/ln("Main Menu"); //DEBUGGING
  }

  // Carry out common activities each time a setting is changed
  void setAdmin(byte name, unsigned int setting) {
    Serial.print("Setting "); //DEBUGGING
    Serial.print(name); //DEBUGGING
    Serial.print(" = "); //DEBUGGING
    Serial.println(setting);//DEBUGGING
    lcd.clear();
    lcd.print("Set ");
    lcd.print(name);
    lcd.print("=");
    lcd.print(setting);
    delay(1000);
    lcd.clear();
    lcd.print("MAIN MENU");
    encoderPos = 0; // reorientate the menu index - optional as we have overflow check code elsewhere
    buttonPressed = 0; // reset the button status so one press results in one action
    Mode = 0; // go back to top level of menu, now that we've set values
    oldEncPos = 0;
    delay(1000);
    lcd.noBacklight();
    delay(1000);
    lcd.clear();
    Serial.println("Main Menu"); //DEBUGGING
  }

  //Rotary encoder interrupt service routine for one encoder pin
  void PinA() {
    cli(); //stop interrupts happening before we read pin values
    reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
    if (reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      encoderPos --; //decrement the encoder's position count
      bFlag = 0; //reset flags for the next turn
      aFlag = 0; //reset flags for the next turn
    }
    else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
    sei(); //restart interrupts
  }

  //Rotary encoder interrupt service routine for the other encoder pin
  void PinB() {
    cli(); //stop interrupts happening before we read pin values
    reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
    if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      encoderPos ++; //increment the encoder's position count
      bFlag = 0; //reset flags for the next turn
      aFlag = 0; //reset flags for the next turn
    }
    else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
    sei(); //restart interrupts
  }
  // end of sketch!
