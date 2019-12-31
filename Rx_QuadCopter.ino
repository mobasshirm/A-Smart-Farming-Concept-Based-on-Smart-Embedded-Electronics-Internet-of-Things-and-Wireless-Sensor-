#include <Servo.h>                                //Servo library for operating the ESC (Mechanism of servos and ESC are similar, both are needed to be fed with PWM signal)
#include <SPI.h>                                  //Serial Peripheral Interface header
#include <nRF24L01.h>                             //nRF module headers
#include <RF24.h>

RF24 radio(7,8);                                  //CE, CSN pin of nRF module

Servo ESC1;                                       //Naming the servo objects for four BLDCs
Servo ESC2;
Servo ESC3;
Servo ESC4;

const byte address[6] = "00015";                  //Address of the nRF module

int sig[5];

void setup()
{
  ESC1.attach(2);                                 //Defining pins for each BLDCs
  ESC2.attach(3);
  ESC3.attach(4);
  ESC4.attach(5);
  //Radio initialization codes for nRF module
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop()
{
  if(radio.available())
  {
    radio.read(&sig, sizeof(sig));
    //Serial.println(sig);

    int x1_value = sig[0];
    int y1_value = sig[1];
    int y2_value = sig[2];
    int buttonstate1 = sig[3]; 
    int buttonstate2 = sig[4];

    if(sig == sig[1])
    {
      //Quadcopter Up
      if(y1_value <= 50)
      {
        int throttle_bldc_1 = y1_value;                         //Value of -y axis of left joystick
        throttle_bldc_1 = map(throttle_bldc_1, 50, 0, 0, 180);  //Mapping of the values
        ESC1.write(throttle_bldc_1);                            //Writing the value to the ESC of BLDC motor

        int throttle_bldc_2 = y1_value;                         
        throttle_bldc_2 = map(throttle_bldc_2, 50, 0, 0, 180);  
        ESC2.write(throttle_bldc_2);                            

        int throttle_bldc_3 = y1_value;                         
        throttle_bldc_3 = map(throttle_bldc_3, 50, 0, 0, 180);  
        ESC3.write(throttle_bldc_3);                            

        int throttle_bldc_4 = y1_value;
        throttle_bldc_4 = map(throttle_bldc_4, 50, 0, 0, 180);
        ESC4.write(throttle_bldc_4);
       }
       
       //Quadcopter Down
       else
       {
         int throttle_bldc_1 = y1_value;                            //Value of +y axis of left joystick
         throttle_bldc_1 = map(throttle_bldc_1, 700, 1023, 180, 0); //Mapping of the values
         ESC1.write(throttle_bldc_1);                               //Writing the value to the ESC of BLDC motor

         int throttle_bldc_2 = y1_value;
         throttle_bldc_2 = map(throttle_bldc_2, 700, 1023, 180, 0);
         ESC2.write(throttle_bldc_2);

         int throttle_bldc_3 = y1_value;
         throttle_bldc_3 = map(throttle_bldc_3, 700, 1023, 180, 0);
         ESC3.write(throttle_bldc_3);

         int throttle_bldc_4 = y1_value;
         throttle_bldc_4 = map(throttle_bldc_4, 700, 1023, 180, 0);
         ESC4.write(throttle_bldc_4);
       }
    }

    if(sig == sig[0])
    {
      //Quadcopter Left
      if(x1_value <= 50)
      {
        int throttle_bldc_1 = x1_value;                              //Value of -x axis of left joystick
        throttle_bldc_1 = map(throttle_bldc_1, 50, 0, 0, 120);       //Mapping of the values
        ESC1.write(throttle_bldc_1);                                 //Writing the value to the ESC of BLDC motor

        int throttle_bldc_2 = x1_value;
        throttle_bldc_2 = map(throttle_bldc_2, 50, 0, 0, 180);
        ESC2.write(throttle_bldc_2);

        int throttle_bldc_3 = x1_value;
        throttle_bldc_3 = map(throttle_bldc_3, 50, 0, 0, 120);
        ESC3.write(throttle_bldc_3);

        int throttle_bldc_4 = x1_value;
        throttle_bldc_4 = map(throttle_bldc_4, 50, 0, 0, 60);
        ESC4.write(throttle_bldc_4);
      }
      //Quadcopter Right
      else
      {
        int throttle_bldc_1 = x1_value;                              //Value of +x axis of left joystick
        throttle_bldc_1 = map(throttle_bldc_1, 700, 1023, 0, 120);   //Mapping of the values
        ESC1.write(throttle_bldc_1);                                 //Writing the value to the ESC of BLDC motor

        int throttle_bldc_2 = x1_value;
        throttle_bldc_2 = map(throttle_bldc_2, 700, 1023, 0, 60);
        ESC2.write(throttle_bldc_2);

        int throttle_bldc_3 = x1_value;
        throttle_bldc_3 = map(throttle_bldc_3, 700, 1023, 0, 120);
        ESC3.write(throttle_bldc_3);

        int throttle_bldc_4 = x1_value;
        throttle_bldc_4 = map(throttle_bldc_4, 700, 1023, 0, 60);
        ESC4.write(throttle_bldc_4);
      }
    }

    if(sig == sig[2])
    {
      //Quadcopter move forward
      if(y2_value <= 50)
      {
        int throttle_bldc_1 = y2_value;                       //Value of -y axis of right joystick
        throttle_bldc_1 = map(throttle_bldc_1, 50, 0, 0, 60); //Mapping of the values
        ESC1.write(throttle_bldc_1);                          //Writing the value to the ESC of BLDC motor

        int throttle_bldc_2 = y2_value;
        throttle_bldc_2 = map(throttle_bldc_2, 50, 0, 0, 120);
        ESC2.write(throttle_bldc_2);

        int throttle_bldc_3 = y2_value;
        throttle_bldc_3 = map(throttle_bldc_3, 50, 0, 0, 180);
        ESC3.write(throttle_bldc_3);

        int throttle_bldc_4 = y2_value;
        throttle_bldc_4 = map(throttle_bldc_4, 50, 0, 0, 120);
        ESC4.write(throttle_bldc_4);
      }
      //Quadcopter move backward
      else
      {
        int throttle_bldc_1 = y2_value;                             //Value of +y axis of right joystick
        throttle_bldc_1 = map(throttle_bldc_1, 700, 1023, 0, 180);  //Mapping of the values
        ESC1.write(throttle_bldc_1);                                //Writing the value to the ESC of BLDC motor

        int throttle_bldc_2 = y2_value;
        throttle_bldc_2 = map(throttle_bldc_2, 700, 1023, 0, 120);
        ESC2.write(throttle_bldc_2);

        int throttle_bldc_3 = y2_value;
        throttle_bldc_3 = map(throttle_bldc_3, 700, 1023, 0, 60);
        ESC3.write(throttle_bldc_3);

        int throttle_bldc_4 = y2_value;
        throttle_bldc_4 = map(throttle_bldc_4, 700, 1023, 0, 120);
        ESC4.write(throttle_bldc_4);
      }
    }

    //Spray System
    if(sig == sig[3])
    {
      if(buttonstate1 == HIGH)
      {
        //Sprayer mechanism on
      }
      else
      {
        //Sprayer mechanism off
      }
    }

    //Bucket
    if(sig == sig[4])
    {
      if(buttonstate2 == HIGH)
      {
        //Bucket is opened
      }
      else
      {
        //Bucket is closed
      }
    }
    
  }

}

