// T daccord avec mes commentaires? Je sais pas trop ou je peux rajouter

#include <Arduino.h>
#include <WiFiNINA.h> //Including this library to allow the use of the Arduino Uno Wifi
#include <Wire.h> //Including this library to communicate with I2C devices
#include <BH1750.h> //Including the library of a light sensor
#include "secrets.h" //Use this file to store all of the private credentials and connection details
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

#define RX 10
#define TX 11

int LDRsensorPin = A0;  // LDR
int potentio = A1; // Potentiometer
int led=3; // PWM pin

char ssid[] = SECRET_SSID;    //  your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;     //Creates a client that can connect to to a specified internet IP address and port.

unsigned long myChannelNumber = SECRET_CH_ID; //Unsigned long variables are extended size variables for number storage
const char * myWriteAPIKey = SECRET_WRITE_APIKEY; // Write API key

int number = 0;
String myStatus = "";
String getFields = "";

BH1750 lightMeter; // We create an object from BH1750 class

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
 
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion(); //Returns the firmware version running on the module as a string
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
   
  ThingSpeak.begin(client);  //Initialize ThingSpeak
   
  Wire.begin(); //Initiate the Wire library and join the I2C bus

  //to use one shot, high resolution mode
  lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2);
 
  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  PWMLightVaristor(); // Varying potentiometer to chang PWM
  getFields = getLDRSensorData() ; //Return the LDR sensor data
  lightMeter.begin(); //Initializes beightness measurments
  float lux2 = lightMeter.readLightLevel(); //Read lux value
  Serial.println((String)"Light: " + lux2 + " lx");
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){ //Wifi.status() Return the connection status and WL_CONNECTED assigned when connected to a WiFi network
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);    
    }
    Serial.println("\nConnected.");
  }
 
// set the fields with the values
  ThingSpeak.setField(1, getFields);
  ThingSpeak.setField(2, lux2);
  //ThingSpeak.setField(3, number3);
  //ThingSpeak.setField(4, number4);
 
  // set the status
  ThingSpeak.setStatus(myStatus);
 
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
 
  delay(20000); // Wait 20 seconds to update the channel again
}

//==========get LDR data  (converting LDR analog vakues to LUX values)=============
String getLDRSensorData(){
  const double k = 5.0/1024;
  const double luxFactor = 500000;
  const double R2 = 10000;
  const double LowLightLimit = 200;
  const double B = 1.3*pow(10.0,7);
  const double m = -1.4;
  int sensorvalue=analogRead(LDRsensorPin);
  Serial.println(sensorvalue);
  delay(1);
  double V2 = k*sensorvalue;
  double R1 = (5.0/V2 - 1)*R2;
  double lux = B*pow(R1,m);

 // If the value of the light sensor is superior or equal to 600, the led will be turn on, otherwise, it will be shut down
  if (sensorvalue>=600) {
    digitalWrite(LED_BUILTIN,HIGH);
    }else{
    digitalWrite(LED_BUILTIN,LOW);
    }
  Serial.println(lux);
  return  String(lux);
   //return random(1000); // Replace with
}

//================== Adjust light brightness using PWM ============
void PWMLightVaristor() {
   // defining constants for for lux calculator
  int POT_sensorValue,y;
  // read the value from the sensor:
  POT_sensorValue = analogRead(potentio);
  Serial.println("sensorValue ==>>" + String(POT_sensorValue));
  y=POT_sensorValue;
  y=map(y,0,1023,0,255) ;  // convert the scale 0 - 1023 to 0 - 255
  Serial.println("PWM value ==>>" + String(y));
  analogWrite(led,y); // send appropriate PWM signal
  delay(500);              
}