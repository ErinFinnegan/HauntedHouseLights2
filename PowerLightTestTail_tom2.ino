/*
 Debounce
 
 Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
 press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  There's
 a minimum delay between toggles to debounce the circuit (i.e. to ignore
 noise).
 
 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground
 
 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 
 created 21 November 2006
 by David A. Mellis
 modified 30 Aug 2011
 by Limor Fried
 modified 28 Dec 2012
 by Mike Walters
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Debounce
 
 Added Flickering by Tom Arthur Oct 30
 Changed to interupts to work with flickering
 
 */



/*
////////////////////////////////////////////////////////////////////////////////////////////
 
Mostly by Tom Arthur, 10/30/2014

 IMPORTANT NOTE:  You gotta plug the power tail into pin 13 (blue) and ground (white) for this.  
 The LED won't work if you do this.
 
 Channel 1: brightness
 Ch. 2:  Strobe
 
 ////////////////////////////////////////////////////////////////////////////////////////////
 */



//#include <DmxInterrupt.h>
#include <DmxSimple.h>

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 3;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin
const int PowerSwitchTail = 8;

int stateArray[] = {
  0, 1, 2};

int currentState = -1;  //the current state hasn't been set yet

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int dim = -1; // current dim value

int inByte = 0;    

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long debouncing_time = 500; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;
boolean fadeLoop = true;

void setup() {
  // put your setup code here, to run once:
  //1 red
  //2 green
  // 3 blue
  //4 shutter strobe
  DmxSimple.usePin(3);
  DmxSimple.maxChannel(3);

//  pinMode(2, INPUT);
//  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, debounceInterrupt, RISING);   

  pinMode(ledPin, OUTPUT);
  pinMode(PowerSwitchTail, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);
  
  //the lights have an address on the menu.  You can set the address on the screen.  The channel that you choose is...
  //  So 2 x lights with 5 channels each, the first 1 is 1-5, and the second one begins at 6, so whatever the channels were
  // from that ight go up to 6.  You can see and set the light channels from the main menu on the light by hitting return (on the smaller light)
  // and on the back of the hanging type lights.

  // Set lights to default state if there is any issue getting connected to MAX
  // this prevents you from being stuck in the dark. 
  DmxSimple.write(5,200 ); //amber
  DmxSimple.write(4,200 ); //blue
  DmxSimple.write(3,200 ); //green
  DmxSimple.write(2,200 ); //red


  Serial.begin(9600);
  establishContact();

}

void loop() {

  if (buttonState == 1){
    buttonState = 0;
    if (currentState == -1) {
      IntroScene();  
    } 
    else if (currentState == 0) { 
      BlackLightScene (); 

    } 
    else if (currentState == 1)    {
      PortalScene();
    } 
    else if (currentState == 2) {
      IntroScene();  
    } 
    else {
      IntroScene();  
    }
  }

}


void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print("A");   // send an initial string
    delay(300);
  }
}

void IntroScene(){
  Serial.print(0);
  currentState = 0;
  DmxSimple.write(1, 0);
 
  DmxSimple.write(5,0 ); //amber
  DmxSimple.write(4,0 ); //blue
  DmxSimple.write(3,0 ); //green
  DmxSimple.write(2,0 ); //red
  amberFadeIn(0);
  digitalWrite(PowerSwitchTail, LOW);
    

  fadeLoop = true; 
  AmberFlicker();

}

void BlackLightScene (){
  Serial.print(1); 
  digitalWrite(PowerSwitchTail, HIGH);
  dramaticFlash();
  currentState = 1;

}

void PortalScene() {
  whiteFadeIn(0);
//  DmxSimple.write(1, 220);
  currentState = 2;
  Serial.print(2);
  fadeLoop = true; 
//  WhiteFlicker();
}

void AmberFlicker() {
  while (fadeLoop == true){
    int nextDim = random (190, 255);  
    while (nextDim != dim && fadeLoop == true) {
      if (dim > nextDim){
        dim--;
      } 
      else if (dim < nextDim){
        dim++;
      }
      DmxSimple.write(5,dim); 
    }

    delay (random(50, 300));
  } 
  
}

//void WhiteFlicker() {
//  while (fadeLoop == true){
//    int nextDim = random (100, 255);  
//    while (nextDim != dim && fadeLoop == true) {
//      if (dim > nextDim){
//        dim--;
//      } 
//      else if (dim < nextDim){
//        dim++;
//      }
//      DmxSimple.write(1,dim); 
//      delay(10);
//    }
//
//    delay (random(50, 300));
//  } 
//  
//}

void amberFadeIn(int startVal) {
  int finalBright = 255;
  dim = 0;
  while (dim != 255) {
      if (dim > finalBright){
        dim--;
      } 
      else if (dim < finalBright){
        dim++;
      }
      DmxSimple.write(5,dim); 
      delay(2);
    }
  
}

void amberFadeOut(int startVal) {
  
  
}

void whiteFadeIn(int startVal) {
  int finalBright = 255;
  dim = 0;
  while (dim != finalBright) {
      if (dim > finalBright){
        dim--;
      } 
      else if (dim < finalBright){
        dim= dim + 1;
      }
      DmxSimple.write(1,dim); 
    }
}

void whiteFadeOut(int startVal) {
  int finalBright = 0;
  dim = 0;
  while (dim != finalBright) {
      if (dim > finalBright){
        dim--;
      } 
      else if (dim < finalBright){
        dim++;
      }
      DmxSimple.write(5,dim); 
      delay(2);
    }
}

// makes a flash to startle audience
void dramaticFlash() {
    DmxSimple.write(4,255); //blue
    DmxSimple.write(3,255); //green
    DmxSimple.write(2,255); //red
   delay(200); // change this to adjust the length of the flash
    DmxSimple.write(5,0); //blue
    DmxSimple.write(4,0); //blue
    DmxSimple.write(3,0); //green
    DmxSimple.write(2,0); //red
  
}

// handles interupt and prevents retriggering
// tells loop that status has changed
void debounceInterrupt() {

  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
      fadeLoop = false; 
      buttonState = 1;
      last_micros = micros();

  }

}




