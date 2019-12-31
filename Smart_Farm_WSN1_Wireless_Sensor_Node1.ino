//*******************************************  WSN-1  *************************************************//

#include <DHT.h>                               //DHT Sensor Header
#include <Wire.h>                              //Wire header
#include <Adafruit_BMP085.h>                   //Adafruit Barometric Pressure Sensor BMP085/180 Header
#include <SPI.h>                               //Serial Peripheral Interface Header
#include <nRF24L01.h>                          //nRF24L01 Wireless Transceiver Module Headers
#include <RF24.h>
#include <RF24Network.h>

#define DHT11_data_pin 2                       //DHT11 sensor data input pin
#define DHTTYPE DHT11                          //Setting the DHT Sensor Type

RF24 radio(10, 11);                            //CE, CSN pin of nRF24L01 Module
RF24Network network(radio);                    //Include the radio in the network
const uint16_t node_WSN01 = 01;                //Address of this node (in Octal format)
const uint16_t CN = 00;                        //Address of the central node
const unsigned long interval = 10;             //How often to transmit data to the central node
unsigned long last_sent;                       //Time of the last transmission

const int pir_sensor = 3;                      //PIR sensor data input pin
const int buzzer = 4;                          //Buzzer output signal pin
const int water_sensing_pin = A1;              //Water sensor data input pin
const int moisture_value_probe = A2;           //Soil moisture sensor data input pin
const int moisture_level = 250;
const int pH_sensor_pin = A3;                  //pH sensor data input pin
int pH_val = 0;                                //Initial pH Value
unsigned long int avg_val;                     //Avarage pH value
int bufr[10];                                  //Buffer to store pH sensor readings
int temporary;                                 //Temporary pH readings store
float sig[8];

//MQ-135 Gas Sensor (CO2)
int mq_data_pin = A0;                            //define which analog input channel you are going to use
int Rl_value = 1;                                //define the load resistance on the board, in kilo ohms
float R0_clear_air_factor = 4.5;                 //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,

int calibration_sample_times = 50;               //define how many samples you are going to take in the calibration phase
int calibration_sample_interval = 500;           //define the time interal(in milisecond) between each samples in the
                                                 //cablibration phase
int read_sample_interval = 50;                   //define how many samples you are going to take in normal operation
int read_sample_times = 5;                       //define the time interal(in milisecond) between each samples in normal operation

int CO2 = 0;

float CO2_curve[3] = {1.6,0.18,-0.55};            //two points are taken from the curve. 
                                                  //with these two points, a line is formed which is "approximately equivalent"
                                                  //to the original curve.
                                                  //data format:{ x, y, slope}; point1: (x1, y1), point2: (x2, y2) of the corresponding curve

float Ro = 10;                                    //Ro is initialized to 10 kilo ohms


DHT dht(DHT11_data_pin, DHTTYPE);
Adafruit_BMP085 bmp;

void setup() 
{
  pinMode(pir_sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin())
  {
  Serial.println("BMP180 sensor not found");
  while (1) {}
  }

  Serial.print("Calibrating...\n");                
  Ro = MQ_calibration(mq_data_pin);               //Calibrating the sensor. Please make sure the sensor is in clean air 
                                                  //when you perform the calibration                    
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");

//Initialization code for nRF24L01 Module
  SPI.begin();
  radio.begin();
  network.begin(90, node_WSN01);                  //(Channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
}

void loop() 
{
  float temparature = dht.readTemperature();

  float humidity = dht.readHumidity();

  int pir_sensor_val = digitalRead(pir_sensor);
  if (pir_sensor_val == HIGH) 
  {
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    digitalWrite(buzzer, LOW);
  }

  float pressure = bmp.readPressure();

  float water_sense_data = analogRead(water_sensing_pin);

  //if(water_sense_data >= 1)
  //{
    //Serial.println("Rainfall Detected...");
  //}
  //else
  //{
    //Serial.println("No Trace of Rainfall...");
  //}

  float moisture = analogRead(moisture_value_probe);
  float moisture_percentage = (100 - ((moisture/1023)*100));

  for(int i=0;i<10;i++) 
  { 
    bufr[i]=analogRead(pH_sensor_pin);
    delay(10);
  }
  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)
    {
      if(bufr[i]>bufr[j])
      {
        temporary=bufr[i];
        bufr[i]=bufr[j];
        bufr[j]=temporary;
      }
    }
   }
   avg_val=0;
   for(int i=2;i<8;i++)
   {
     avg_val+=bufr[i]; 
   }
   
   float pH_vol=(float)avg_val*5.0/1024/6;
   float pH_value = -5.70 * pH_vol + 21.34;
   delay(20);

   sig[0]=temparature;
   sig[1]=humidity;
   sig[2]=pressure;
   sig[3]=water_sense_data;
   sig[4]=moisture_percentage;
   sig[5]=pH_value;
   sig[6]=MQ_determine_gas_percentage(MQ_data_read(mq_data_pin)/Ro, CO2);
   sig[7]=pir_sensor_val;
   
   network.update();
   unsigned long now = millis();

   if (now - last_sent >= interval)
   {
     last_sent = now;
     RF24NetworkHeader header(CN); 
     network.write(header, &sig, sizeof(sig));
     delay(20);
   }
   
}

//MQ Resistance Calculation
float MQ_resistance_calculation(int raw_adc)
{
  return ( ((float)Rl_value*(1023-raw_adc)/raw_adc));
}

//MQ Calibration
float MQ_calibration(int mq_data_pin)
{
  int i;
  float val = 0;

  for (i = 0; i<calibration_sample_times; i++) 
  {            //take multiple samples
    val += MQ_resistance_calculation(analogRead(mq_data_pin));
    delay(calibration_sample_interval);
  }
  val = val/calibration_sample_times;                   //calculate the average value

  val = val/R0_clear_air_factor;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 

  return val; 
}

//MQ Read
float MQ_data_read(int mq_data_pin)
{
  int i;
  float rs = 0;

  for (i = 0; i<read_sample_times; i++) 
  {
    rs += MQ_resistance_calculation(analogRead(mq_data_pin));
    delay(read_sample_interval);
  }

  rs = rs/read_sample_times;

  return rs;  
}

// MQ Get Gas Percentage
float MQ_determine_gas_percentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == CO2 ) 
  {
     return MQ_determine_percentage(rs_ro_ratio, CO2_curve);
  } 
  else
  {
    return 0;
  }

  return 0;
}

//MQ Get Percentage 

float  MQ_determine_percentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
