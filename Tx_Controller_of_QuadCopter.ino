//TX
#include <SPI.h>                                  //Serial Peripheral Interface header
#include <nRF24L01.h>                             //nRF module headers
#include <RF24.h>

RF24 radio(7,8);                                  //CE, CSN pin of nRF module

int jstk1_x1_dir = A0;                            //Pin to detect x value for left joystick
int jstk1_y1_dir = A1;                            //Pin to detect y value for left joystick
int jstk2_y2_dir = A3;                            //Pin to detect y value for left joystick
int button1 = 2;                                  //Pin to detect signal for left push button
int button2 = 3;                                  //Pin to detect signal for right push button
int buttonstate1 = 0;                             //Initial state of left button
int buttonstate2 = 0;                             //Initial state of right button

const byte address[6] = "00015";                  //Address of the nRF module

int sig[5];
void setup()
{
  Serial.begin(9600);                             
  //Radio initialization codes for nRF module
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  //Pin state
  pinMode(jstk1_x1_dir, INPUT);
  pinMode(jstk1_y1_dir, INPUT);
  pinMode(jstk2_y2_dir, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
}

void loop()
{
  //Variables to store data/signals from each pin
  int x1_value = analogRead(jstk1_x1_dir);
  int y1_value = analogRead(jstk1_y1_dir);
  int y2_value = analogRead(jstk2_y2_dir);
  int buttonstate1 = digitalRead(button1); 
  int buttonstate2 = digitalRead(button2);

  sig[0] = x1_value;
  sig[1] = y1_value;
  sig[2] = y2_value;
  sig[3] = buttonstate1;
  sig[4] = buttonstate2;
  
  radio.write(&sig, sizeof(sig));
  delay(20); 

}
