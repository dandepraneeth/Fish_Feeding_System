#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Cloudchip.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<Servo.h>


#define WIFI_SSID    "WIFI"          //Enter Your WIFI Username
#define WIFI_PASSWORD "Password"      //Enter WIFI Passsword
#define TOKEN     "TOKEN"
#define ONE_WIRE_BUS D1 //Dallas temperature sensor pin 
#define senseInput D7 //Turbidity sensor pin
#define depth A0
int angle = 0;
//int depth=A0;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass the oneWire refe
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
Cloudchip cloudchip;
Servo servo;


//Set to A0 as Analog Read
int senseRawValue; //Some variable
float senseTurbidity; //Some floating variable


void setup(){
  Serial.begin(115200);
  cloudchip.WiFiInit(WIFI_SSID ,WIFI_PASSWORD);
  cloudchip.Credentials(TOKEN);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  servo.attach(D2);
  sensors.begin();
  pinMode(senseInput,INPUT);
  pinMode(depth,INPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D8,OUTPUT);
  digitalWrite(D5,HIGH);// We used as 3.3v (VCC) because of shortage of vcc
  digitalWrite(D8,HIGH);// We used as 3.3v (VCC) because of shortage of vcc
}

void loop() {

//*****************************temperature**************************
  sensors.requestTemperatures(); 
Serial.println("Temperature is: ");
Serial.println(sensors.getTempCByIndex(0));
//**********************************************************************

//**************************time***************************************
  timeClient.update();
  delay(1000);
   cloudchip.run();
  String TerminalCommands = cloudchip.Terminal();
  Serial.println(" ");
  Serial.println("Termianl command = "+TerminalCommands);
  Serial.println(" ");
  Serial.println(timeClient.getFormattedTime());
  Serial.println(" ");
  if(TerminalCommands.equals(timeClient.getFormattedTime()))
  {
    for(angle = 0; angle < 180; angle++)  
  {                                  
    servo.write(angle);               
    delay(50);
    Serial.println("feed");                   
  }
  for(angle = 180; angle < 1; angle++)  
  {                                  
    servo.write(angle);               
    delay(15);             
    Serial.println("not feeding");      
  }
    Serial.println("rotate");
  }
  
  //********************************Water depth level sensor****************
  int level=analogRead(depth);
if(sensors.getTempCByIndex(0)>=28)
{
  digitalWrite(D3,LOW);
  Serial.println("fan is on");
}
if(sensors.getTempCByIndex(0)<27.99)
{
  digitalWrite(D3,HIGH);
  Serial.println("fan is off");
}
if(level<=50)
{
  Serial.println("empty");
  digitalWrite(D4,LOW);
}
if(level>50&&level<=120)
{
  digitalWrite(D4,LOW);
  Serial.println("low water level");
}
if(level>120&&level<330)
{
  digitalWrite(D4,HIGH);
  Serial.println("high water level");
}
//*************************************************************

//*******************************Turbidity***************************
  
  senseRawValue = analogRead(senseInput); //Read input raw value fromt the sensor
  senseTurbidity = senseRawValue * (5.0 / 1024.0); //Convert analog data from 0 -1024 to voltage 0 - 5v;
  Serial.println("TURBIDITY VALUE > "); //Print the output data to the serial
  Serial.println(senseTurbidity);
 
if(senseTurbidity<2.0)
{
  digitalWrite(D6,HIGH);
  Serial.println("**turbidity sensor pump is on**");
}
if(senseTurbidity>=2.1)
{
  digitalWrite(D6,LOW);
  Serial.println("**turbidity sensor pump is off**");
}
 //******************************************************************
}
