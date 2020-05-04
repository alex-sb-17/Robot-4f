#include "ultrasunete.h"
#include "control_motoare.h"

#include <IRremote.h>
#include <Wire.h>

//#define SLAVE_ADDRESS 9
//#define ANSWERSIZE 1  // lungime raspuns asteptat de la slave

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

  pinMode(13, OUTPUT);  // builtin led
  
  irrecv.enableIRIn();
  Wire.begin();
  initializareMotoare(motor_a1, motor_a2, motor_b1, motor_b2);
}

void loop() {
  // directie: 0 stop; 1 inainte; 2 inapoi; 3 dr larg; 4 dr strans; 5 stg larg; 6 stg strans; 10 stop rapid
  //test();
  if(!(millis() % 500))  // standby, builtin led on/off
  {
    builtinLedOnOff();  
  }
  if(irrecv.decode(&results)) // daca senzorul primeste date
  {
    //Serial.print("cod IRL ");
    //Serial.print(results.value, HEX);  // afisare cod IR in hexa
    //Serial.println("");
    tratareComandaIR(results.value);
    irrecv.resume();  // sterge din memorie
  }

  
  /*
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
  controlDirectie(0, 255);
   
  */
 //test();
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
  void tratareComandaIR(long int cod_comanda)
  {
    if(cod_comanda == 0x44bb40bf) controlDirectie(0, 255);
    if(cod_comanda == 0x44bb609f) controlDirectie(1, 255);
    if(cod_comanda == 0x44bb50af) controlDirectie(2, 255);
    if(cod_comanda == 0x44bb807f) controlDirectie(3, 255);
    if(cod_comanda == 0x44bbc23d) controlDirectie(5, 255);
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
