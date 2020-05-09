#include <WiFiClient.h>
#include <Wire.h>
#include <Servo.h>
#include <IRremote.h>
#include "ultrasunete.h"
#include "control_motoare.h"


#define SLAVE_ADDRESS 0x08
//#define ANSWERSIZE 1  // lungime raspuns asteptat de la slave
int received_from_WiFi = 0;  // initializat cu 0 (comanda de stop)

// stare initiala robot: 1 = urmarire linie, 2 = ocolire obstacole, 3 = telecomandat IR, 4 = telecomandat WiFi
byte stare_robot = 1;

// servomotor
Servo servomotor;
int pinServo = 12;

// pin infrarosu
int pinIR = 9;
IRrecv irrecv(pinIR);  // initializare library
decode_results results;  // in results primeste rezulatele

// builtin led state
byte builtin_LED_prevState = 0;

// pini comanda motoare
int motor_a1 = 5;
int motor_a2 = 7;
int motor_b1 = 6;
int motor_b2 = 8;

// pini modul ultrasunete
int trig = 11;
int echo = 10;

// pini modul urmarire linie
int S1 = 4;
int S2 = A1; 
int S3 = A2;
int S4 = A3;
int S5 = A0;
//int last_S1, last_S3, last_S5;

void setup() {
  // run once:
  Serial.begin(9600);
  
  pinMode(motor_a1, OUTPUT);
  pinMode(motor_a2, OUTPUT);
  pinMode(motor_b1, OUTPUT);
  pinMode(motor_b2, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

  pinMode(pinIR, INPUT);

  pinMode(13, OUTPUT);  // builtin led

  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  servomotor.attach(pinServo);
  servomotor.write(90);
  irrecv.enableIRIn();
  initializareMotoare(motor_a1, motor_a2, motor_b1, motor_b2);
}

void loop() {
  // directie: 0 stop; 1 inainte; 2 inapoi; 3 dr larg; 4 dr strans; 5 stg larg; 6 stg strans; 10 stop rapid
  //test();
  /*
  if(!(millis() % 500))  // standby, builtin led on/off
  {
    builtinLedOnOff();  
  } */
  Serial.println("stare r = ");
  Serial.print(stare_robot);
  Serial.println("");
  switch(stare_robot)
  {
    case 1:
    {
      //urmarireLinie();
      int x = 1*!digitalRead(S3) + 5*!digitalRead(S2) + 3*!digitalRead(S4) ;
      controlDirectie(x, 255 - 90*(!digitalRead(S2) + !digitalRead(S4)));
      //Serial.println(x);
      break;
    }
    case 2:
    {
      ocolireObstacole();
      break;
    }
    case 3:
    {
      break;
    }
    case 4:
    {
      controlDirectie(received_from_WiFi, 255);
      break;
    }
    default:
      controlDirectie(0, 0);
  }
  /////////////////////////////////////////////////////////////////////////
  if(irrecv.decode(&results)) // daca senzorul primeste date
    {
      //Serial.print("cod IRL ");
      //Serial.print(results.value, HEX);  // afisare cod IR in hexa
      //Serial.println("");
      tratareComandaIR(results.value);
      irrecv.resume();  // sterge din memorie
    }

 //test();
  }

  

  void urmarireLinie()
  {  /*
    if(!digitalRead(S3))
      {
        controlDirectie(1, 255);
      }  
    if(!digitalRead(S2))
      {
        controlDirectie(5, 160);     
      }
    if(!digitalRead(S4))
      {
        controlDirectie(3, 160);     
      }
    if(digitalRead(S3) && digitalRead(S2) && digitalRead(S4))
      controlDirectie(0, 255);  */
    controlDirectie(1*!digitalRead(S3) + 5*!digitalRead(S2) + 3*!digitalRead(S4), 255);
  }
void ocolireObstacole()
{
    //verifica distanta pana la obstacolul din fata
  float distanta_obstacol_stanga = 0;
  float distanta_obstacol_dreapta = 0;
  Serial.println(citesteDistanta(trig, echo));
  if (citesteDistanta(trig, echo) > 50)
  {
    controlDirectie(1, 255);
  }
  else
  {
    controlDirectie(1, 180);
    if (citesteDistanta(trig, echo) < 30)
    {
      Serial.println("<30 =");
      Serial.print(citesteDistanta(trig, echo));
      controlDirectie(0, 255);
      delay(1000);
      servomotor.write(0);
      delay(1000);
      distanta_obstacol_dreapta = citesteDistanta(trig, echo);
      delay(100);
      servomotor.write(180);
      delay(1000);
      distanta_obstacol_stanga = citesteDistanta(trig, echo);
      delay(100);
      if(distanta_obstacol_dreapta < 50 && distanta_obstacol_stanga < 50)
      {
        controlDirectie(0, 255);
        intoarce180();
        controlDirectie(1, 255);
      }
      else
      {
        if(distanta_obstacol_dreapta < distanta_obstacol_stanga)
        {
          intoarceStanga90();
          controlDirectie(1, 255);
        }
        else
        {
          intoarceDreapta90();
          controlDirectie(1, 255);
        }
        if(distanta_obstacol_dreapta == distanta_obstacol_stanga)
        {
          intoarceDreapta90();
          controlDirectie(1, 255);
        }
      }
      servomotor.write(90);
    }
  }
}

void intoarce180()
{
  controlDirectie(4, 170);
  delay(400);
  controlDirectie(0, 250);
}
void intoarceStanga90()
{
  controlDirectie(6, 170);
  delay(200);
  controlDirectie(0, 250);
}
void intoarceDreapta90()
{
  controlDirectie(4, 170);
  delay(200);
  controlDirectie(0, 250);
}
void tratareComandaIR(long int cod_comanda)
  {/*
    stare_robot = 2;
    if(cod_comanda == 0x44bb40bf) controlDirectie(0, 255); //stop
    if(cod_comanda == 0x44bb609f) controlDirectie(1, 255); //inainte
    if(cod_comanda == 0x44bb50af) controlDirectie(2, 255); //inapoi
    if(cod_comanda == 0x44bb807f) controlDirectie(3, 255); //dreapta larg
    if(cod_comanda == 0x44bbc23d) controlDirectie(5, 255); //stanga larg
    if(cod_comanda == 0xabcdef)
    {
      stare_robot == 1; //sau 2 !
      controlDirectie(0, 255); //stop
    }*/
    //stare_robot = 3;
    //Serial.println("rec ir activ");
    switch(cod_comanda)
    {
      case 0x44bb40bf: 
        controlDirectie(0, 255);
        break;
      case 0x44bb609f: 
        controlDirectie(1, 255);
        break;
      case 0x44bb50af: 
        controlDirectie(2, 255);
        break;
      case 0x44bb807f: 
        controlDirectie(3, 255);
        break;
      case 0x44bbc23d: 
        controlDirectie(5, 255);
        break;
      case 0x44bbd22d:
        stare_robot = 1;
        controlDirectie(0, 255);
        break; 
      case 0x44bbda25:
        stare_robot = 2;
        controlDirectie(0, 255);
        break;         
    }
  }

  void receiveEvent()
  {
    received_from_WiFi = Wire.read();
    stare_robot = 4;
    Serial.println(received_from_WiFi);
  }

  void requestEvent()
  {
    int i = 7;
    Wire.write(i);
  //Serial.println(i);
  }
  
  void builtinLedOnOff()
  {
    if (builtin_LED_prevState == 0)
    {
      digitalWrite(13, HIGH);
      builtin_LED_prevState = 1;
    }
    else
    {
      digitalWrite(13, LOW);
      builtin_LED_prevState = 0;
    }
  }

  void test()
  {
    
    Serial.print("S1 = ");
    Serial.print(digitalRead(S1));
    Serial.print("    S2 = ");
    Serial.print(digitalRead(S2));
    Serial.print("    S3 = ");
    Serial.print(digitalRead(S3));
    Serial.print("    S4 = ");
    Serial.print(digitalRead(S4));
    Serial.print("    S5 = ");
    Serial.print(digitalRead(S5));
    Serial.println();  

    /*
    controlDirectie(1, 255);
    delay(3000);
    controlDirectie(0, 255);
    delay(3000);
    controlDirectie(2, 255);
    delay(3000);
    controlDirectie(0, 255);
    delay(3000);
    controlDirectie(3, 255);
    delay(3000);
    controlDirectie(4, 255);
    delay(3000);
    controlDirectie(0, 255);
    delay(3000);
    controlDirectie(5, 255);
    delay(3000);
    controlDirectie(6, 255);
    delay(3000);
    controlDirectie(10, 255);
    delay(3000);  
    controlDirectie(1, 255);
    delay(3000);
    controlDirectie(0, 255);
    delay(3000); */
  }
