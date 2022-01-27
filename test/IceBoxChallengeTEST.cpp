#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// Data wire is connected to the Arduino Uno Wifi Rev2
#define ONE_WIRE_BUS 7
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

#define POWER_PIN  7
#define SIGNAL_PIN A5

int value = 0; // variable to store the sensor value

DeviceAddress sensor1 = { 0x28, 0xBC, 0xB9, 0x01, 0x00, 0x00, 0x00, 0x57 }; 
DeviceAddress sensor2 = { 0x28, 0xE9, 0xB4, 0x01, 0x00, 0x00, 0x00, 0x7E }; 
DeviceAddress sensor3 = { 0x28, 0xDD, 0xD0, 0x01, 0x00, 0x00, 0x00, 0x41 }; 
DeviceAddress sensor4 = { 0x28, 0x83, 0xE3, 0x01, 0x00, 0x00, 0x00, 0xB6 };

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);
int number5 = random(0,100);
String myStatus = "";

void setup() {
  Serial.begin(115200);  // Initialize serial

  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF

  sensors.begin();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  Serial.print("Sensor 1(*C): ");
  Serial.println(sensors.getTempC(sensor1)); 
  number1 = sensors.getTempC(sensor1); 
 
  Serial.print("Sensor 2(*C): ");
  Serial.println(sensors.getTempC(sensor2)); 
  number2 = sensors.getTempC(sensor2);  
  
  Serial.print("Sensor 3(*C): ");
  Serial.println(sensors.getTempC(sensor3));    
  number3 = sensors.getTempC(sensor3); 

  Serial.print("Sensor 4(*C): ");
  Serial.println(sensors.getTempC(sensor4));  
  number4 = sensors.getTempC(sensor4); 

  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Sensor value: ");
  Serial.println(value);
  number5 = value;

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
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
  ThingSpeak.setField(1, number1);
  ThingSpeak.setField(2, number2);
  ThingSpeak.setField(3, number3);
  ThingSpeak.setField(4, number4);
  ThingSpeak.setField(5, number5);

  // figure out the status message
  if(number1 > number2){
    myStatus = String("field1 is greater than field2"); 
  }
  else if(number1 < number2){
    myStatus = String("field1 is less than field2");
  }
  else{
    myStatus = String("field1 equals field2");
  }
  
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
  
  // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);
  
  delay(20000); // Wait 20 seconds to update the channel again
}
