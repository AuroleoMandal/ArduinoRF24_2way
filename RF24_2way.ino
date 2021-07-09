#include <SPI.h>            //Serial Peripheral Interface library
#include <nRF24L01.h>       //https://github.com/nRF24/RF24
#include <RF24.h>           //to access the RF24 class

#define CE_pin 7    //define Chip Enable (CE) and Chip Select Not (CSN) pins here
#define CSN_pin 8   

RF24 radio(CE_pin, CSN_pin);  //RF24 object declaration

int node = 0; //default node value (can be changed to 1 for the other node)

const byte addresses[][6] = {"11111", "22222"};   // 2 addresses for communication pipelines between node 0->1 and 1->0

void setup() 
{ 
  Serial.begin(9600);
  node = serialModifyRole(node);  
  //Use the Arduino IDE serial monitor to change node identity 
  //(Disable this and change default value in declaration if communication model is used in a project without an interface)
  
  radio.begin();                        
  radio.setPALevel(RF24_PA_MIN);              //set power level (MIN, LOW, HIGH, MAX)

  if(node==0)                                   //Reading and writing address for node 0 set here
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  if(node==1)                                   //Reading and writing address for node 1 set here
  {
    radio.openReadingPipe(1, addresses[0]);
    radio.openWritingPipe(addresses[1]);
  }
  conn_troubleshoot();
}

void loop() 
{

  if(node == 0)     //behaviour for node 0
  {
      radio.stopListening();
      node0Transmission();
      delay (5);

      radio.startListening();
      char msg[32]="";
    if (radio.available()) 
    { 
      radio.read(&msg, sizeof(msg));
      Serial.println(msg);
      delay (5);
    }
  }

    if(node == 1)     //behaviour for node 1
  {
    radio.startListening();
    int valueReceived;
    if (radio.available()) 
    { 
      radio.read(&valueReceived, sizeof(valueReceived));
      Serial.println(valueReceived);
      delay (5);
    
      radio.stopListening();
      node1Transmission();
      delay (5);
    }
  }
  delay(100);
}

//--------------------------------------------------------------------------------------------------
void node0Transmission()    //what node 0 will transmit (also modify what node 1 will recieve)
{
  int valueSent = 200;
  radio.write(&valueSent, sizeof(valueSent));
}

void node1Transmission()    //what node 1 will transmit (also modify what node 0 will recieve)
{
  char msg[] = "Received";
  radio.write(&msg, sizeof(msg));
}
//--------------------------------------------------------------------------------------------------

int serialModifyRole(int defaultNodeVal)    //modify node using serial monitor
{
  int response;
  Serial.println("Which node will it be ? (Give one 0 and the other 1)");
  while(Serial.available() == 0){}
  response = Serial.parseInt();
  if (response == 0)
  {
    Serial.println("This is node 0, all transmissions can be modified in node0Transmission()");
    return 0;
  }
  else if (response == 1)
  {
    Serial.println("This is node 1, all transmissions can be modified in node1Transmission()");
    return 1;
  }
  else
  {
    Serial.print("Input not valid, using default node = ");
    Serial.println(node);
    return defaultNodeVal;
  }
  delay(1000);
}

void conn_troubleshoot()     //use to troubleshoot connections
{
  bool isConnected=0;
  isConnected = radio.isChipConnected();
  if(isConnected == 1)
    Serial.println(" (Connections Okay)");
  else
    Serial.println(" (Check your connections");
}