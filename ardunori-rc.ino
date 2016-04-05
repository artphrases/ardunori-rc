/* 
This code serves as a basis to switch the blocks/bars of a Yamaha Tenori-On automatically.
Please have a look here https://youtu.be/Xg3ltSjQTvw for a visual demonstration and further explanation.
*/

// Actual number of pins for the POC
const int pinCount = 4;
// Midi Click counter
int counter = 0;

//HEX Array with 16 values for the 16 possible blocks of the Tenori-On
byte hexArray[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

const int buttonPin[] = {53,51,49,47}; // Button pins 
const int output_redLed[] = {45,44,43,42}; // Pins for the RGB LED red light
const int ledPinBlue[] = {52,50,48,46}; // Pins for the RGB LED blue light

// Variables will change:
int ledStateBlue[] = {LOW,LOW,LOW,LOW};         // the current state of the output pin
int buttonState;                               // the current reading from the input pin
int lastButtonState[] = {LOW,LOW,LOW,LOW};    // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int thisPin = 0;
int thisPin2 = 0;

byte midi_start = 0xfa;
byte midi_stop = 0xfc;
byte midi_clock = 0xf8;
byte midi_continue = 0xfb;
int play_flag = 0;
byte data;

void setup() {
    
    Serial.begin(31250);

    for (int thisPin = 0; thisPin < pinCount; thisPin++) { 
      pinMode(output_redLed[thisPin], OUTPUT);
      pinMode(buttonPin[thisPin], INPUT);
      pinMode(ledPinBlue[thisPin], OUTPUT);
      digitalWrite(ledPinBlue[thisPin], ledStateBlue[thisPin]);
    }

// Sends SysEx Command to set Remote Mode without initializing/clear block 

    Serial.write(0xF0);               // SysEx start
    Serial.write(0x43);               // Manufacturer 0
    Serial.write(0x73);               // Model 1
    Serial.write(0x01);               // MIDI channel
    Serial.write(0x33);               // Data
    Serial.write(0x01);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x02);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0xF7);               // SysEx End
}

void loop() {

    showPressed();
    
    if(Serial.available() > 0) {
      data = Serial.read();
      if(data == midi_start) {
        play_flag = 1;
        thisPin = 0;
        }

      else if(data == midi_continue) {
        play_flag = 1;
        }
      
      else if(data == midi_stop) { 
        play_flag = 0;
        counter = 0;
          for (int thisPin = 0; thisPin < pinCount; thisPin++) {
          digitalWrite(output_redLed[thisPin], LOW);
          }
        }

      else if((data == midi_clock) && (play_flag == 1)) {
        showPlay();
        }
      }
}

// Function to ligth and move the red RGB ligth from bar to bar
void showPlay() {
    
    if(counter == 95) {      
      counter = 0;
      digitalWrite(output_redLed[thisPin], LOW);
        if(thisPin == pinCount - 1 || ledStateBlue[thisPin + 1] == LOW) {
          thisPin = 0;
        }
        else {
          thisPin++; 
        }
    }
            
    else if(counter < 95) {
      counter++;
      while (ledStateBlue[thisPin] == LOW) {
        thisPin++; 
      }
      if (counter == 1) {
        sendingSysEx(thisPin); 
      }
      digitalWrite(output_redLed[thisPin], HIGH);
    }
    
    else {
      counter++;
    }
}

void showPressed() {

  //The blue LEDS will be switched on or off
 for (int thisPin2 = 0; thisPin2 < pinCount; thisPin2++) {
    
    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin[thisPin2]);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
      if (reading != lastButtonState[thisPin2]) {
      // reset the debouncing timer
      lastDebounceTime = millis();
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState) { 
        buttonState = reading;

          // only toggle the LED if the new button state is HIGH
          if (buttonState == HIGH) {
          ledStateBlue[thisPin2] = !ledStateBlue[thisPin2];
          }
        }
      }

      // set the LED:
      digitalWrite(ledPinBlue[thisPin2], ledStateBlue[thisPin2]);

      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState[thisPin2] = reading;
    }
}

// SysEx Command Function to send the changes from one block to another
void sendingSysEx(int thisPin)
  
  {

    Serial.write(0xF0);               // SysEx start
    Serial.write(0x43);               // Manufacturer 0
    Serial.write(0x73);               // Model 1
    Serial.write(0x01);               // MIDI channel
    Serial.write(0x33);               // Data
    Serial.write(0x01);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x0F);               // Data
    Serial.write(hexArray[thisPin]);  // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0x00);               // Data
    Serial.write(0xF7);               // SysEx End
    
  }






