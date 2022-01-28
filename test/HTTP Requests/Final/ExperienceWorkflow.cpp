/*
    
*/

#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"

// Data wire is connected to the Arduino Uno Wifi Rev2
#define ONE_WIRE_BUS 7
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

#define POWER_PIN  8
#define SIGNAL_PIN A5

nt value = 0; // variable to store the sensor value

DeviceAddress sensor1 = { 0x28, 0xBC, 0xB9, 0x01, 0x00, 0x00, 0x00, 0x57 }; 
DeviceAddress sensor2 = { 0x28, 0xE9, 0xB4, 0x01, 0x00, 0x00, 0x00, 0x7E }; 
DeviceAddress sensor3 = { 0x28, 0xDD, 0xD0, 0x01, 0x00, 0x00, 0x00, 0x41 }; 
DeviceAddress sensor4 = { 0x28, 0x83, 0xE3, 0x01, 0x00, 0x00, 0x00, 0xB6 };

int value = 0; // variable to store the sensor value

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "op-dev.icam.fr";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);
int number5 = random(0,100);

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
    digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
  
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }

    String fv = WiFi.firmwareVersion();

    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }

    Serial.println("Connected to wifi");
    printWifiStatus();
    Serial.println("\nStarting connection to server...");

    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=57600&newStatus=0");
        client.println("Host: op-dev.icam.fr");
        client.println("Connection: close");
        client.println();
    }

    int x;
    int p;
    std::cout << "Would you enter the prediction manualy? Yes/No" << std::endl;
    cin >> x;
    if (cin == "Yes" or cin == "yes") {
        Serial.println("Entering manual prediction mode");
        p = 0;
    }
    else {
        Serial.println("Entering Water Level automatic prediction mode")
        p = 1;
    }
}

void KnowThePrediction() {
    int tho = -15257,64;
    int T;
    int Tinf = 25;
    int Tf;
    int t;

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
    
    Tf = number4;
    T = number2;

    t = tho * log10((T - Tinf) / (Tf - Tinf))
}

void loop() {
    KnowThePrediction();

    digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
    delay(10);                      // wait 10 milliseconds
    value = analogRead(SIGNAL_PIN); // read the analog value from sensor
    digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

    Serial.print("Sensor value: ");
    Serial.println(value);

    int r;
    std::cout << "What is you prediction? (Time in second)" << std::endl;
    cin >> r;

    if (p = 0)  {
        if (value <= 100)   {
            Serial.println("Water level is empty.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r);
            Serial.println("Predictions allowed !");
        }
        else if (value > 100 && value <= 300) {
            Serial.println("Water level is low.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r);
            Serial.println("Predictions allowed !");
        }
        else if (value > 300 && value <= 330)   {
            Serial.println("Water level is medium.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r);
            Serial.println("Predictions allowed !");
        }
        else if (value >= 330)  {
            Serial.println("Water level is high.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r);
            Serial.println("Predictions allowed !");
        }
        Serial.println("EEOM sented to server successfully !");
    }

    if (p = 1)  {
        if (value <= 100)   {
            Serial.println("Water level is empty.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        else if (value > 100 && value <= 300) {
            Serial.println("Water level is low.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        else if (value > 300 && value <= 330)   {
            Serial.println("Water level is medium.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        else if (value >= 330)  {
            Serial.println("Water level is high.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        Serial.println("EEOM sented to server successfully !");
    }

    int r2;
    std::cout << "What is you prediction? (Time in second)" << std::endl;
    cin >> r2;

    if (p = 0)  {
        if (value <= 100)   {
            Serial.println("Water level is empty.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=57600&newStatus=2");
            Serial.println("Predictions forbiden !");
            Serial.println("Experience Finished !");
        }
        else if (value > 100 && value <= 300) {
            Serial.println("Water level is low.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r2);
            Serial.println("Predictions allowed !");
        }
        else if (value > 300 && value <= 330)   {
            Serial.println("Water level is medium.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r2);
            Serial.println("Predictions allowed !");
        }
        else if (value >= 330)  {
            Serial.println("Water level is high.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + r2);
            Serial.println("Predictions allowed !");
        }
        Serial.println("EEOM sented to server successfully !");
    }
    
    if (p = 1)  {
        if (value <= 100)   {
            Serial.println("Water level is empty.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=57600&newStatus=2");
            Serial.println("Predictions forbiden !");
            Serial.println("Experience Finished !");
        }
        else if (value > 100 && value <= 300) {
            Serial.println("Water level is low.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        else if (value > 300 && value <= 330)   {
            Serial.println("Water level is medium.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        else if (value >= 330)  {
            Serial.println("Water level is high.");
            client.println("GET /~icebox/createPrediction.php?idexperience=15&secretkey=fa49&prediction=" + t);
            Serial.println("Predictions allowed !");
        }
        Serial.println("EEOM sented to server successfully !");
    }

    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
        char c = client.read();
        Serial.write(c);
        }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();
        // do nothing forevermore:
        while (true);
    }
    delay(1000);
}