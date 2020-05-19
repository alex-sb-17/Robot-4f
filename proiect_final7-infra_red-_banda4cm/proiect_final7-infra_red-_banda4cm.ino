#include <WiFiClient.h>
#include <Wire.h>
#include <Servo.h>
#include <FastLED.h>
#include <IRremote.h>
#include "TimerOne.h"
#include "ultrasunete.h"
#include "control_motoare.h"

#define Led_PIN 13  // banda leduri
#define NUM_LEDS 8 
CRGB leds[NUM_LEDS];

#define SLAVE_ADDRESS 0x08    //#define ANSWERSIZE 1  // lungime raspuns asteptat de la slave
int received_from_WiFi = 0;  // initializat cu 0 (comanda de stop)

// stare initiala robot: 1 = urmarire linie, 2 = ocolire obstacole, 3 = telecomandat IR, 4 = telecomandat WiFi
int stare_robot;
int stare_anterioara_robot;
int temperatura = 22;

// servomotor
Servo servomotor;
int pinServo = 9;

// pin infrarosu telecomanda
int pinIR = 12;
IRrecv irrecv(pinIR);  // initializare library
decode_results results;  // in results primeste rezulatele

//interrupts
const byte MOTOR1 = 2;  // Motor 1 Interrupt Pin - INT 0 
const byte MOTOR2 = 3;  // Motor 2 Interrupt Pin - INT 1 

// Integers for pulse counters
unsigned int counter1 = 0; 
unsigned int counter2 = 0; 

float diskslots = 20; 

// builtin led state
byte builtin_LED_prevState = 0;
byte obstacol = 0;

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
  FastLED.addLeds<WS2812, Led_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(128,0,0);
  leds[7] = CRGB(128,0,0);
  FastLED.show();
  stare_robot = 1;
  stare_anterioara_robot = 1;
  aprindeLeduriSpate(0,50,20);
  
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
  servomotor.write(0);
  delay(1000);
  servomotor.write(180);
  delay(1000);
  servomotor.write(90);
  delay(1000);
  servomotor.detach();
  
  irrecv.enableIRIn();
  
  attachInterrupt(digitalPinToInterrupt (MOTOR1), ISR_count1, RISING);  // Increase counter 1 when speed sensor pin goes High
  attachInterrupt(digitalPinToInterrupt (MOTOR2), ISR_count2, RISING);  // Increase counter 2 when speed sensor pin goes High
  
  initializareMotoare(motor_a1, motor_a2, motor_b1, motor_b2);
}

void loop() {
  
  // directie: 0 stop; 1 inainte; 2 inapoi; 3 dr larg; 4 dr strans; 5 stg larg; 6 stg strans; 10 stop rapid

  //Serial.println("stare r = ");
  //Serial.print(stare_robot);
  //Serial.println("");

  switch(stare_robot)
  {
    case 1:
    {
      //urmarireLinie();
      if (stare_anterioara_robot != 1)
      {
        aprindeLeduriSpate(0,50,20);
        stare_anterioara_robot = 1;
      }        
      
      int x = 1*!digitalRead(S3) + 5*!digitalRead(S2) + 3*!digitalRead(S4) + 5*!digitalRead(S1) + 3*!digitalRead(S5);
      controlDirectie(x, 255 - 90*(!digitalRead(S2) + !digitalRead(S4)));
    /*  if(!digitalRead(S1)) x = 5;
        controlDirectie(x, 250);
      if(!digitalRead(S5)) x = 3;
        controlDirectie(x, 250); */
      //Serial.println(x);
      break;
    }
    case 2:
    {
      if(stare_anterioara_robot != 2)
      {
        aprindeLeduriSpate(120,120,70);
        stare_anterioara_robot = 2;
      }
     
      ocolireObstacole();
      
      //Serial.println("temp from wemos = ");
      //Serial.print(temperatura);
      //intoarce_90_180(90, 6);
      //stare_robot = 1;
      //delay(3000);
      break;
    }
    case 3:
    {
      if(stare_anterioara_robot != 3)
      {
        aprindeLeduriSpate(200,0,130);
        stare_anterioara_robot = 3;
      }  
      break;
    }
    case 4:
    {
      if(stare_anterioara_robot != 4)
      {
        aprindeLeduriSpate(10,10,150);
        stare_anterioara_robot = 4;
      }
      controlDirectie(received_from_WiFi, 255);
      
      Serial.println("received_from_WiFi = ");
      Serial.print(received_from_WiFi);
      Serial.println(" ");
      break;
    }
    default:
      controlDirectie(0, 0);
      aprindeLeduriSpate(255,0,0);
  }
  /////////////////////////////////////////////////////////////////////////
  if(irrecv.decode(&results)) // daca senzorul primeste date
    {
      Serial.print("cod IRL ");
      Serial.print(results.value, HEX);  // afisare cod IR in hexa
      Serial.println("");
      tratareComandaIR(results.value);
      irrecv.resume();  // sterge din memorie
    }
}   // end loop

void ocolireObstacole()
{
    //verifica distanta pana la obstacolul din fata
  float distanta_obstacol_stanga = 0;
  float distanta_obstacol_dreapta = 0;
  Serial.println(citesteDistanta(trig, echo, temperatura));
  if (citesteDistanta(trig, echo, temperatura) > 50)
  {

    if(obstacol == 100)  // obstacol jos
    {
      controlDirectie(0, 255);  // opreste
      delay(1000);
      controlDirectie(2, 255);  // inapoi
      delay(1000);
      intoarce_90_180(180, 4); // intoarce 180 grade, la dreapta
      delay(1000);
      obstacol = 0;
    }
    if(obstacol == 110)  // obstacol stanga
    {
      controlDirectie(3, 255);  // vireaza dreapta
      delay(500);
      obstacol = 0;
    }
    if(obstacol == 120)  // obstacol dreapta
    {
      controlDirectie(5, 255);  // vireaza stanga
      delay(500);
      obstacol = 0;
    }
    
    controlDirectie(1, 255);
    
  }
  else
  {
    controlDirectie(1, 180);  // scade viteza
    if (citesteDistanta(trig, echo, temperatura) < 30)
    {
      Serial.println("<30 =");
      Serial.println(citesteDistanta(trig, echo, temperatura));
      controlDirectie(0, 255);
      delay(1000);
      actioneazaServo(0);  // se uita la dreapta
      delay(1000);
      distanta_obstacol_dreapta = citesteDistanta(trig, echo, temperatura);
      Serial.println(" ");
      Serial.println("dist obst dreapta = ");
      Serial.print(distanta_obstacol_dreapta);
      Serial.println(" ");
      actioneazaServo(180);  // se uita la stanga
      delay(1000);
      distanta_obstacol_stanga = citesteDistanta(trig, echo, temperatura);
      Serial.println(" ");
      Serial.println("dist obst stanga = ");
      Serial.print(distanta_obstacol_stanga);
      Serial.println(" ");
      actioneazaServo(90);
      delay(1000);
      if(distanta_obstacol_dreapta < 50 && distanta_obstacol_stanga < 50)
      {
        leds[0] = CRGB(0,0,0);
        leds[7] = CRGB(0,0,0);
        FastLED.show();
        delay(500);
        leds[0] = CRGB(128,0,0);
        leds[7] = CRGB(128,0,0);
        FastLED.show();
        intoarce_90_180(180, 4); // intoarce 180 grade, la dreapta
        
        delay(1000);
        controlDirectie(1, 255);
      }
      else
      {
        if(distanta_obstacol_dreapta <= distanta_obstacol_stanga)
        {
          Serial.println("intorc la stanga");
          semnalizeaza(7);
          intoarce_90_180(90, 6);  // stanga strans
          delay(1000);
          controlDirectie(1, 255);
        }
        else
        {
          Serial.println("intorc la dreapta");
          semnalizeaza(0);
          intoarce_90_180(90, 4);  // dreapta strans
          delay(1000);
          controlDirectie(1, 255);
        }
      }
      //servomotor.write(90);
      //actioneazaServo(90);
    }
  }
}

void actioneazaServo(int pozitie_servo)
{
  Serial.println("servo actionat");
  servomotor.attach(pinServo);
  servomotor.write(pozitie_servo);
  delay(200);
  servomotor.detach();
  delay(50);
}

void intoarce_90_180(byte grade, byte directie_intoarcere)
{
  controlDirectie(0, 255);  // opreste
  //delay(500);
  counter1 = 0;
  counter2 = 0;
  long int k = millis();
  while (counter1 < 24/(180/grade) && counter2 < 24/(180/grade)) // 24 gasit experimental pt ~180 grade
  {
    controlDirectie(directie_intoarcere, 170); // directie - 4 dr sau 6 stg
    //delay(2);
  };
  //Serial.println(millis() - k);
  controlDirectie(0, 250);
}

void tratareComandaIR(long int cod_comanda)
  {
    //Serial.println("rec ir activ");
    if (cod_comanda == 0x44bb40bf || cod_comanda == 0x44bb609f || cod_comanda == 0x44bb50af || cod_comanda == 0x44bb807f || cod_comanda == 0x44bbc23d)
    {
      //aprindeLeduriSpate(200,0,130);
      stare_robot = 3;
    }
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
        stare_robot = 1;  // preda controlul catre urmaritorul de linie
        //aprindeLeduriSpate(0,50,20);
        controlDirectie(0, 255);
        break; 
      case 0x44bbda25:
        //aprindeLeduriSpate(120,120,70);
        stare_robot = 2;  // preda controlul catre ocolitorul de obstacole
        controlDirectie(0, 255);
        break;         
    }
  }

  void receiveEvent()
  {
    received_from_WiFi = Wire.read();
    if(stare_robot == 2)
    {
      if(received_from_WiFi == 100)
      {
        obstacol = 100;  // obstacol in fatza jos
      }
      if(received_from_WiFi == 110)
      {
        obstacol = 110;  // obstacol stanga
      }
      if(received_from_WiFi == 120)
      {
        obstacol = 120;  // obstacol dreapta
      }
      if(received_from_WiFi >=10 && received_from_WiFi <= 40)
      {
        temperatura = received_from_WiFi;
      }
      if(received_from_WiFi >= 0 & received_from_WiFi <= 5) stare_robot = 4;
    }
    else
    { // de testat dc a venit cod corect miscare !!!
      if(received_from_WiFi >= 0 & received_from_WiFi <= 5)
      stare_robot = 4;  // a receptionat altceva decat sesizare obstacol sau temperatura
    }
    //Serial.println(received_from_WiFi);
  }

void requestEvent()
{
  Wire.write(stare_robot);
}
  
void aprindeLeduriSpate(byte R, byte G, byte B)
{
    for(int i = 0; i < 6; i++)
      {
        leds[i+1] = CRGB(R,G,B);
      }
      FastLED.show();
}

void semnalizeaza(int i)
{
  leds[i] = CRGB(0,0,0);
  FastLED.show();
  delay(500);
  leds[i] = CRGB(128,0,0);
  FastLED.show();
  delay(500);
  leds[i] = CRGB(0,0,0);
  FastLED.show();
  delay(500);
  leds[i] = CRGB(128,0,0);
  FastLED.show();
}

void ISR_count1()  
{
  counter1++;  // increment Motor 1 counter value
} 
 
void ISR_count2()  
{
  counter2++;  // increment Motor 2 counter value
} 
