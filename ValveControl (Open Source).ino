 /**       
 *       !!!Work in Progress!!! Code is unfinished as of now
 *       RTC circuits and the Wifi support is yet to be added
 *       
 *         IMPORTANT The warning sequnces are not self-reseting 
 *       when they notice changes, the reset button must be pressed! 
 *            The open / close cycles are self-reseting
 */



#include <WiFi.h>       // Includes the Wifi Library

// The lights and buzzer
#define Redlight 2       // Red, Green and Blue lights outputs defined here
#define Greenlight 3
#define Bluelight 4
#define Wifilight 5      // The Wifi is connected / ON light  
#define Buzzer 6         // Defines the buzzer (summer)

// The sensors and the valve
#define SCIn 9           // The Arduinos input from the sensorcontrol (recives both warnings for water leakage and sensor problems
#define MoistureSensor 8    // The moisture sensor senses if there is a leak and gives a signal if detects it. (Sends signal over the 433 Mhz band)
#define PressureSensor A1         // The Pressure sensor (potentiometer)
#define Watervalve 0     // This valve controlls the waterflow in the house's system. (I testbänken är den här reläet)

// The 433 Mhz reciver and Wifi contacts (Wifi still yet to come)
#define rfReceivePin A2        //RF Receiver pin = Analog pin 0

#define IsPhoneConnected 11 //I only used this to "simulate" a phone being connected to the Arduino. Replace this with the code to monitor if the phone is connected and in the house

int Pressure = 0;
int RuntimeCounter = 0;

// This part is for the 433 Mhz reciver
unsigned int data = 0;                     // variable used to store received data

int MaxValue = 204;    //lower threshold value, adjust this to get the right treshold for the transmitter

//This is for the Wifi Shield
char ssid[] = "WriteTheNameOfTheNetworkHere";     //  your network SSID (name)
char pass[] = "PasswordGoesHere";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


void setup() {
  // put your setup code here, to run once:

pinMode(Redlight, OUTPUT);    // Sets the outputs for the RGB Light, Red - Green - Blue
pinMode(Greenlight, OUTPUT);
pinMode(Bluelight, OUTPUT);
pinMode(Wifilight, OUTPUT);
pinMode(Buzzer, OUTPUT);   // The buzzer that sounds if problems occur

pinMode(SCIn, INPUT);         // Input from the sensor control
pinMode(MoistureSensor, INPUT);     //This will be connected with the 433 Mhz reciver
pinMode(PressureSensor, INPUT);

pinMode(IsPhoneConnected, INPUT);     //Simulates a connected phone, remove this when making the wifi connection parts  

digitalWrite(Redlight, HIGH);    // This tests the whole RGB range and the buzzer with 1 second increments
delay(1000);
digitalWrite(Redlight, LOW);
delay(1000);
digitalWrite(Greenlight, HIGH);
delay(1000);
digitalWrite(Greenlight, LOW);
delay(1000);
digitalWrite(Bluelight, HIGH);
delay(1000);
digitalWrite(Bluelight, LOW);
delay(1000);
digitalWrite(Buzzer, HIGH);
delay(1000);
digitalWrite(Buzzer, LOW);

status = WiFi.begin(ssid, pass);  //Initializes the WIFI connection
}

// The segments below are the different functions called on to do light loops or open / close actions
void Open() {           // The sequnce of commands to run when wanting to open the valve
  digitalWrite(Redlight, LOW);         // Turns of the red "closed" light
  digitalWrite(Watervalve, HIGH);       // The command to open the valve
  digitalWrite(Greenlight, HIGH);        // The green light blink sequnce, runs for 1 second
  delay(250);
  digitalWrite(Greenlight, LOW);
  delay(250);
  digitalWrite(Greenlight, HIGH);
  delay(250);
  digitalWrite(Greenlight, LOW);
  delay(250);
  digitalWrite(Watervalve, LOW);
  digitalWrite(Greenlight, HIGH);
}

void Close() {            // Call this code to close the valve
  digitalWrite(Greenlight, LOW);        // Turns off the green "open" light
  digitalWrite(Watervalve, HIGH);       // The command to close the valve
  digitalWrite(Redlight, HIGH);        // The red light blink sequnce, runs for 1 second
  delay(250);
  digitalWrite(Redlight, LOW);
  delay(250);
  digitalWrite(Redlight, HIGH);
  delay(250);
  digitalWrite(Redlight, LOW);
  delay(250);
  digitalWrite(Watervalve, LOW);
  digitalWrite(Redlight, HIGH);
}

void Pressure_Warning() {     // This function will be called if the Pressure sensor gives an alert
  digitalWrite(Redlight, HIGH);      // Turns the red light on for half a second
  digitalWrite(Buzzer, HIGH);    // Sounds the buzzer for 0.5 seconds, quiet 2 seconds
  delay(500);
  digitalWrite(Redlight, LOW);
  digitalWrite(Buzzer, LOW);
  delay(2000);
  digitalWrite(Bluelight, HIGH);
  // Turns the blue light on for half a second
  digitalWrite(Buzzer, HIGH);
  delay(500);
  digitalWrite(Bluelight, LOW);
  digitalWrite(Buzzer, LOW);
  delay(2000);
}

void Sensor_Failure() {         // This function is called on if the senor fails
  digitalWrite(Redlight, HIGH);
  delay(2000);
  digitalWrite(Redlight, LOW);
  delay(2000); 
}

void Moisture_Warning() {
  digitalWrite(Buzzer, HIGH);    // Sounds the buzzer for 0.5 seconds, quiet 2 seconds
  delay(500);
  digitalWrite(Buzzer, LOW);
  delay(2000);
  digitalWrite(Buzzer, HIGH);
  delay(500);
  digitalWrite(Buzzer, LOW);
  delay(2000);  
}

void loop() {
  // put your main code here, to run repeatedly:

/*
 * This is where the code for the Wifi and the RTC (RealTimeClock)goes.
 * 
 */

float data=analogRead(rfReceivePin);    //listen for data on Analog pin 0
// These 2 "if's" are only for testing of the 433mhz reciver
if(data>MaxValue){
  digitalWrite(Greenlight, LOW);   //If a LOW signal is received, turn LED OFF
}

if(data<MaxValue){
  digitalWrite(Greenlight, HIGH);   //If a HIGH signal is received, turn LED ON
}



// This code checks the moisture sensor, and activates if the sensor returns a positive
if (MoistureSensor == HIGH ) {       // Checks the sensors feedback, this would be the signal from the 433mhz reciver
   for (RuntimeCounter = 0; RuntimeCounter < 1; RuntimeCounter++) {         // This limits the time this "if" can run to 1 time. If this would loop, then the valve would open and close repeatedly
      digitalWrite (Watervalve, HIGH);        // This closes the valve
      delay(1000);
      digitalWrite (Watervalve, HIGH);
   }
Moisture_Warning();
}

// The code below checks if the phone has left the building
/* bool PhoneInHouse = digitalRead (IsPhoneConnected);   //I only used this to show how we would code the function that checks if the phone is in the house, and activates if it leaves the house
if (PhoneInHouse == 0) {      // Activates if the phone leaves the house.
  delay(3000000);           // Waits for 5 minutes
          if (PhoneInHouse == 0){         //Checks again after 5 minutes to see if the phone is out of the house
              Close();
              delay(2000);
              int Pressure = analogRead(PressureSensor);      //This creates a integer called "Pressure" and inserts the value from the Pressure Sensor into it
               /*  The highest pressure from the pressure sensor is 1023
                *  682 is 2/3 of 1023, which means that if this number is reached, the pressure sensor is messuring a pressure loss of 30% or higher
                *  That means there is a leak somewhere in the system
                *  204 = 1v;
          }
}*/

// The code below is for the different fail scenarios, like pressure sensor failure
if (Pressure <682) {            //This checks the integer "Pressure", and activates if the value is 682 or lower
  for (RuntimeCounter = 0; RuntimeCounter < 1; RuntimeCounter++) {     // This limits the time this "if" can run to 1 time. If this would loop, then the valve would open and close repeatedly
    digitalWrite (Watervalve, HIGH);       // This closes the valve  
    delay(1000);
    digitalWrite (Watervalve, LOW);
  }
Pressure_Warning();        // Calls for the red/blue warning light loop
}

if (digitalRead(SCIn) == HIGH ) {    // Monitors the status of the sensor, activates if a fail message is recived
  for (RuntimeCounter = 0; RuntimeCounter < 1; RuntimeCounter++) {         // This limits the time this "if" can run to 1 time. If this would loop, then the valve would open and close repeatedly
    digitalWrite (Watervalve, HIGH);        // This closes the valve
    delay(1000);
    digitalWrite (Watervalve, LOW);
  }
Sensor_Failure();        // Calls for the red warning light loop
}
}
