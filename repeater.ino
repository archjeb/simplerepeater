// Version 0.1  - Initial code write up.
/*
 * Simple Repeater Controller
 *
 * Written by Jeremy Georges - 10-19-2015
 *
 * This is written with the Motorola Maxtrac radios in mind. COR has been brought out of the radio using the RX sense pin on internal board.
 * COR with low/ground actives the PTT.
 * Please see http://www.repeater-builder.com/motorola/maxtrac/maxtrac-rptr-int.html for details on how to bring a COR signal out of the radio for
 * interfacing purposes.
 *
 * Additionally, I have a link radio that RX/TX to/from the hub repeater site.
 * This assumes that the hub (main repeater) is providing CWID, etc.
 * This sample sketch has pin 6 reserved for future use to provide a CWID output if needed. This will be the next rev.

 */

#define VERSION "0.1"

#include <Time.h>

//Define the pins
#define PIN_COR 2   //INPUT for COR -  
#define PIN_COR_LINK 3 // For Link Radio - if used
#define PIN_PTT 4   //Output for PTT
#define PIN_PTT_LINK 5  // For link radio - if used
#define PIN_CWID 6  //Output for CWID - future use
#define PIN_FAN 7       // FAN enable output


//Define Values
#define FAN_TIME 30  //FAN runtime in seconds
#define PTT_TIMER 120 //Max PTT allowed in seconds
#define LINKED 1   // Set to 0 for a non linked repeater setup or 1 if its has a link radio
#define HANG_TIME 2000 // Hang time for PTT in milliseconds

//-----------------------BEGIN Variable setup -------------------------------

int FAN_STATE = 0; // Zero is fan is on, 1 is fan is on
extern time_t START_FAN_TIME = now(); //Initialize with current time.
time_t START_PTT_TIME = now();
time_t myt;
//-----------------------END Variable setup-------------------------------


void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing Pins");
  pinMode(PIN_COR, INPUT);    // COR
  digitalWrite(PIN_COR, HIGH); //Turn on pullup resistor.
  pinMode(PIN_COR_LINK, INPUT);
  digitalWrite(PIN_COR_LINK, HIGH); //Turn on pullup resistor.
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_PTT, OUTPUT);
  digitalWrite(PIN_PTT, HIGH); //Turn on pullup resistor. low to gnd enables PTT on Motorola Radios
  pinMode (PIN_CWID, OUTPUT);
  pinMode (PIN_PTT_LINK, OUTPUT);
  digitalWrite(PIN_PTT_LINK, HIGH); //Turn on pullup resistor. low to gnd enables PTT on Motorola Radios
  Serial.println("Finished initialization");
  Serial.print("Time: ");
  time_t t = now();
  
  Serial.println(t);

}



void loop()
{
  if (now() - START_FAN_TIME > FAN_TIME && FAN_STATE == 1) {  //Check to see if Fan Timer has expired. If so, then turn fan off.
    Serial.println("Turning FAN Off");
    digitalWrite(PIN_FAN, LOW);
    Serial.print("Time: ");
    Serial.println(now());
    Serial.print("Delta of Fan Start time: ");
    Serial.println(now() - START_FAN_TIME);
    FAN_STATE=0;  //Set fan state flag to 0
  }

  //loop through and check COR pins
  if (digitalRead(PIN_COR) == LOW) { //We have a COR active - go to PTT
    Serial.println("COR ACTIVE");
    START_PTT_TIME = now();
    // loop through and keep checking the COR PIN
    Serial.println("Setting PTT Pins ACTIVE on both Radios");
    Serial.println("Turning FAN On");
    FAN_STATE=1;  //Set fan state flag to 1
    while (true) {  // keep PTT active until PIN goes high; we'll check and if it does, we can just break out of this while loop
      digitalWrite(PIN_PTT, LOW);
      digitalWrite(PIN_PTT_LINK, LOW);
      digitalWrite(PIN_FAN, HIGH);
      if (digitalRead(PIN_COR) == HIGH) {
        delay(HANG_TIME); //delay then we'll check again to see if its really end of transmit or not
        if (digitalRead(PIN_COR) == HIGH) {
          Serial.println("COR Not ACTIVE, turning PTT off");
          //If we get here, then we know for sure that we need to stop PTT
          digitalWrite(PIN_PTT_LINK, HIGH);
          delay(2000); //wait a second for repeater link to come back up with roger beep
          digitalWrite(PIN_PTT, HIGH);
          START_FAN_TIME = now();  //Set Fan Start timer once we stop transmitting. That way it can cool after TX
          Serial.print("Setting START_FAN_TIME to: ");
          Serial.println(START_FAN_TIME);
          break; //break out of while loop now
        }
      }
      //check PTT timer to see if its exceeded. If so, then we need to stop transmitting, and we'll wait a few seconds before we allow it move on.
      if (now() - START_PTT_TIME > PTT_TIMER) {
        Serial.println("PTT Timer Exceeded");
        while (digitalRead(PIN_COR == LOW)) {    // We stay in loop and keep PTT off until COR floats high.
          // we need to stay here till the COR signal goes away.
          digitalWrite(PIN_PTT_LINK, HIGH);
          digitalWrite(PIN_PTT, HIGH);
        }
        delay(1000);  //just delay a second to be sure.
        START_FAN_TIME = now();  //Set Fan Start timer once we stop transmitting. That way it can cool after TX
        break;  //break out of the while loop so we go back to main loop to check COR PINs
      }


    }

  }

  if (digitalRead(PIN_COR_LINK) == LOW) {  //We have an incoming signal from our radio link.
    Serial.println("COR ON LINK RADIO ACTIVE");
    START_PTT_TIME = now();
    Serial.println("Activating PTT on Primary Radio");
    Serial.println("Turning FAN On");
    FAN_STATE=1;  //Set fan state flag to 1
    // loop through and keep checking the COR PIN
    while (true) {  // keep PTT active until PIN goes high; we'll check and if it does, we can just break out of this while loop
      digitalWrite(PIN_PTT, LOW);
      digitalWrite(PIN_FAN, HIGH);
      if (digitalRead(PIN_COR_LINK) == HIGH) {  // we won't worry about a hang timer here because the one from the hub radio/repeater already has one.
        Serial.println("COR On Radio Link Is Now Not ACTIVE, turning PTT off");
        digitalWrite(PIN_PTT, HIGH);
        START_FAN_TIME = now();  //Set Fan Start timer once we stop transmitting. That way it can cool after TX
        Serial.print("Setting START_FAN_TIME to: ");
        Serial.println(START_FAN_TIME);
        break; //break out of while loop now
      }
      //check PTT timer to see if its exceeded. If so, then we need to stop transmitting, and we'll wait a few seconds before we allow it move on.
      if (now() - START_PTT_TIME > PTT_TIMER) {
        Serial.println("PTT Timer Exceeded, turning PTT off");
        while (digitalRead(PIN_COR_LINK) == LOW) {    // We stay in loop and keep PTT off until COR floats high.
          // we need to stay here till the COR signal goes away.
          digitalWrite(PIN_PTT, HIGH);
          START_FAN_TIME = now();  //Set Fan Start timer once we stop transmitting. That way it can cool after TX
        }
        delay(1000);  //just delay a second to be sure.
        break;  //break out of the while loop so we go back to main loop to check COR PINs
      }


    }
  }


}
