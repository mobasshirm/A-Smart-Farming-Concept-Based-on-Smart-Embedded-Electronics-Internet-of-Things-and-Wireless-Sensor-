#include <SPI.h>                       //Serial Peripheral Interface Header
#include <nRF24L01.h>                  //nRF module headers
#include <RF24.h>
#include <RF24Network.h>                      
#include <SoftwareSerial.h>            //Software serial header
#include <Wire.h>                      //Wire connectivity header
#include <ESP8266WiFi.h>               //ESP8266 headers
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

RF24 radio(6, 7); // CE, CSN
RF24Network network(radio);            // Include the radio in the network
const uint16_t CN = 00;                // Address of this node in Octal format ( 04,031, etc)
const uint16_t node_WSN01 = 01;        // Address of the other node in Octal format
const uint16_t node_WSN02 = 02;

SoftwareSerial SIM900(18, 19);

const int relay_pin = 5;

float sig[16];

float temparature_WSN1 = 0;
float humidity_WSN1 = 0;
float pressure_WSN1 = 0;
float water_sense_data_WSN1 = 0;
float moisture_percentage_WSN1 = 0;
float pH_value_WSN1 = 0;
float CO2_WSN1 = 0;
float pir_sensor_val_WSN1 = 0;

float temparature_WSN2 = 0;
float humidity_WSN2 = 0;
float pressure_WSN2 = 0;
float water_sense_data_WSN2 = 0;
float moisture_percentage_WSN2 = 0;
float pH_value_WSN2 = 0;
float CO2_WSN2 = 0;
float pir_sensor_val_WSN2 = 0;

const char* ssid = "ESP8266 Access Point"; // The name of the Wi-Fi network that will be created
const char* password = "thereisnospoon";   // The password required to connect to it, leave blank for an open network
const char* ssid_AP     = "SSID";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password_AP = "PASSWORD";     // The password of the Wi-Fi network

const char* host = "script.google.com";
const int https_port = 443;
String script_id = "AKfycbxcxmAp1Xpc-OON2mJ9NYxWZwbUaWOG4NEs_ln2DJJnyGnIt64";

ESP8266WebServer server(80);
WiFiClientSecure client;

void setup() 
{
  SPI.begin();
  radio.begin();
  network.begin(90, CN);                  //(Channel, Node address)
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  pinMode(relay_pin, OUTPUT);

  // Arduino communicates with SIM900A GSM module at a baud rate of 19200
  SIM900.begin(19200);
  // Network synchronization time for GSM module
  delay(20000);

  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection to the Access Point Has Established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() 
{
  network.update();
  // Receiving Signals
  while ( network.available() ) 
  {
    RF24NetworkHeader header;

    //Receiving signals from Wireless Sensor Nodes
    network.read(header, &sig, sizeof(sig));
    
    //Wireless Sensor Node 1
    if (header.from_node == node_WSN01)
    {
      float temparature_WSN1 = sig[0];
      float humidity_WSN1 = sig[1];
      float pressure_WSN1 = sig[2];
      float water_sense_data_WSN1 = sig[3];
      float moisture_percentage_WSN1 = sig[4];
      float pH_value_WSN1 = sig[5];
      float CO2_WSN1 = sig[6];
      float pir_sensor_val_WSN1 = sig[7];    
    }

    //Wireless Sensor Node 2
    if (header.from_node == node_WSN02)
    {
      float temparature_WSN2 = sig[0];
      float humidity_WSN2 = sig[1];
      float pressure_WSN2 = sig[2];
      float water_sense_data_WSN2 = sig[3];
      float moisture_percentage_WSN2 = sig[4];
      float pH_value_WSN2 = sig[5];
      float CO2_WSN2 = sig[6];
      float pir_sensor_val_WSN2 = sig[7];
    }
  }

  //WSN-1
  
  //PIR Motion Detection
  if(pir_sensor_val_WSN1 == HIGH)
  {
    //Command to select SMS mode
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);

    //X's are needed to be replaced with desired receiver's mobile number
    // Use international mobile number format
    SIM900.println("AT + CMGS = \"+XXXXXXXXXXXX\""); 
    delay(100);
  
    // Desired message that is to be sent
    SIM900.println("Alert !!! Motion has been detected near farm field"); 
    delay(100);

    // End AT command with a ^Z, ASCII code 26
    SIM900.println((char)26); 
    delay(100);
    SIM900.println();
    // Give module time to send SMS
    delay(5000);
  }
  else
  {
    
  }

  //Rainfall
  float rainfall_threshold = 400;
  if(water_sense_data_WSN1 >= rainfall_threshold )
  {
    //Command to select SMS mode
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);

    //X's are needed to be replaced with desired receiver's mobile number
    // Use international mobile number format
    SIM900.println("AT + CMGS = \"+XXXXXXXXXXXX\""); 
    delay(100);
  
    // Desired message that is to be sent
    SIM900.println("Rainfall Detected"); 
    delay(100);

    // End AT command with a ^Z, ASCII code 26
    SIM900.println((char)26); 
    delay(100);
    SIM900.println();
    // Give module time to send SMS
    delay(5000);
  }
  else
  {
    
  }

  //Soil Moisture
  float soil_noisture_threshold = 15;
  if(moisture_percentage_WSN1 <= soil_noisture_threshold)
  {
    digitalWrite(relay_pin, HIGH);
  }
  else
  {
    digitalWrite(relay_pin, LOW);
  }

  //pH
  float pH_threshold_acidic = 5;
  float pH_threshold_alkaline = 9;

  //Acidic
  if(pH_value_WSN1 <= pH_threshold_acidic)
  {
    //Command to select SMS mode
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);

    //X's are needed to be replaced with desired receiver's mobile number
    // Use international mobile number format
    SIM900.println("AT + CMGS = \"+XXXXXXXXXXXX\""); 
    delay(100);
  
    // Desired message that is to be sent
    SIM900.println("The soil is acidic, take necessary actions to make it suitable for plants"); 
    delay(100);

    // End AT command with a ^Z, ASCII code 26
    SIM900.println((char)26); 
    delay(100);
    SIM900.println();
    // Give module time to send SMS
    delay(5000);
  }
  //Alkaline
  if(pH_value_WSN1 >= pH_threshold_alkaline)
  {
    //Command to select SMS mode
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);

    //X's are needed to be replaced with desired receiver's mobile number
    // Use international mobile number format
    SIM900.println("AT + CMGS = \"+XXXXXXXXXXXX\""); 
    delay(100);
  
    // Desired message that is to be sent
    SIM900.println("The soil pH is alkaline, take necessary actions to make it suitable for plants"); 
    delay(100);

    // End AT command with a ^Z, ASCII code 26
    SIM900.println((char)26); 
    delay(100);
    SIM900.println();
    // Give module time to send SMS
    delay(5000);
  }
  else
  {
    
  }

  server.handleClient();
}

//Make ESP8266 to work as a HTTP server (AP)
void handle_OnConnect() 
{
  float temparature_WSN1;
  float humidity_WSN1;
  float pressure_WSN1;
  float moisture_percentage_WSN1;
  float pH_value_WSN1;
  float CO2_WSN1;

  float temparature_WSN2;
  float humidity_WSN2;
  float pressure_WSN2;
  float moisture_percentage_WSN2;
  float pH_value_WSN2;
  float CO2_WSN2;

  server.send(200, "text/html", SendHTML(temparature_WSN1,humidity_WSN1,CO2_WSN1,pressure_WSN1,pH_value_WSN1,moisture_percentage_WSN1,temparature_WSN2,humidity_WSN2,CO2_WSN2,pressure_WSN2,pH_value_WSN2,moisture_percentage_WSN2)); 
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

//Processing the HTML page which is to be shown in the ESP8266 HTTP Server (AP)
String SendHTML(float temparature_WSN1,float humidity_WSN1,float CO2_WSN1,float pressure_WSN1,float pH_value_WSN1,float moisture_percentage_WSN1,float temparature_WSN2,float humidity_WSN2,float CO2_WSN2,float pressure_WSN2,float pH_value_WSN2,float moisture_percentage_WSN2)
{
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";

  ptr += "<head>";

  ptr += "<title>Main Page</title>";
  ptr += "<br>";

  ptr += "<h1><center>Wireless Sensor Nodes Data</center></h1>";
  ptr += "</head>";

  ptr += "<body >";

  ptr += "<br>";
  ptr += "<br>";
  ptr += "<br>";
  
  ptr += "<center>";
  ptr += "<p><b>WSN-1</b></p>";
  ptr += "<center>"; 
  ptr += "<br>";
  ptr += "<br>";
  
  ptr += "<p>";
  ptr += "<b>Temperature : ";
  ptr += temparature_WSN1;
  ptr += "&deg; C</b></p>";

  ptr += "<p>";
  ptr += "<b>Humidity : ";
  ptr += humidity_WSN1;
  ptr += "%</b></p>";

  ptr += "<p>";
  ptr += "<b>CO<sub>2</sub> : ";
  ptr += CO2_WSN1;
  ptr += "ppm</b></p>";

  ptr += "<p>";
  ptr += "<b>Barometric Pressure : ";
  ptr += pressure_WSN1;
  ptr += "hPa</b></p>";

  ptr += "<p>";
  ptr += "<b>pH : ";
  ptr += pH_value_WSN1;
  ptr += "</b></p>";

  ptr += "<p>";
  ptr += "<b>Soil Moisture : ";
  ptr += moisture_percentage_WSN1;
  ptr += "%</b></p>";

  
  ptr += "<center>";
  ptr += "<p><b>WSN-2</b></p>";
  ptr += "<center>";
  ptr += "<br>";
  ptr += "<br>";

  ptr += "<p>";
  ptr += "<b>Temperature : ";
  ptr += temparature_WSN2;
  ptr += "&deg; C</b></p>";

  ptr += "<p>";
  ptr += "<b>Humidity : ";
  ptr += humidity_WSN2;
  ptr += "%</b></p>";

  ptr += "<p>";
  ptr += "<b>CO<sub>2</sub> : ";
  ptr += CO2_WSN2;
  ptr += "ppm</b></p>";

  ptr += "<p>";
  ptr += "<b>Barometric Pressure : ";
  ptr += pressure_WSN2;
  ptr += "hPa</b></p>";

  ptr += "<p>";
  ptr += "<b>pH : ";
  ptr += pH_value_WSN2;
  ptr += "</b></p>";

  ptr += "<p>";
  ptr += "<b>Soil Moisture : ";
  ptr += moisture_percentage_WSN2;
  ptr += "%</b></p>";

  
  ptr += "</center>";


  ptr += "</body>";

  ptr += "</html>";
  return ptr;
}

//Transitting data to the cloud after connecting with the nearby Wi-Fi network through an Access Point
void sendData(float temparature_WSN1,float humidity_WSN1,float CO2_WSN1,float pressure_WSN1,float pH_value_WSN1,float moisture_percentage_WSN1,float temparature_WSN2,float humidity_WSN2,float CO2_WSN2,float pressure_WSN2,float pH_value_WSN2,float moisture_percentage_WSN2, int val3)
{
  int a1=val3;
  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client.connect(host, https_port)) 
  {                                                                //Connecting to the IP address and port specified
    Serial.println("Connection Failed");
    a1=1;
  } 
  //float temparature_WSN1;
  //float humidity_WSN1;
  //float pressure_WSN1;
  //float moisture_percentage_WSN1;
  //float pH_value_WSN1;
  //float CO2_WSN1;

  //float temparature_WSN2;
  //float humidity_WSN2;
  //float pressure_WSN2;
  //float moisture_percentage_WSN2;
  //float pH_value_WSN2;
  //float CO2_WSN2;
  
  String url = "/macros/s/" + script_id + "/exec?output1=" + temparature_WSN1 + "&output2=" + humidity_WSN1 + "&output3=" + CO2_WSN1 + "&output4=" + pressure_WSN1 + "&output5=" + pH_value_WSN1 + "&output6=" + moisture_percentage_WSN1 + "&output7=" + temparature_WSN1 + "&output8=" + humidity_WSN1 + "&output9=" + CO2_WSN1 + "&output10=" + pressure_WSN1 + "&output11=" + pH_value_WSN1 + "&output12=" + moisture_percentage_WSN1;
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");                            //Printing data to the cloud server with which the client is connected to

  if(a1==0)
  {
    Serial.println("Connection Request is Sent");
  } 
  else 
  {
     Serial.println("Connection Request Failed");
  }
  while (client.connected()) 
  {
    String line = client.readStringUntil('\n');                           //Stream to string untill described character comes
    if (line == "\r") 
    {
      Serial.println("Headers Successfully Received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  //Serial.println(line);
  if (line.startsWith("{\"state\":\"success\"")) 
  {
    Serial.println("ESP8266 to cloud connection successfull");
  } 
  else 
  {
    Serial.println("ESP8266 to cloud connection has failed");
  }

  Serial.println("Terminating Connection");
}
