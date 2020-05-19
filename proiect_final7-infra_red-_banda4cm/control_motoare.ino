//#include <Arduino.h>

int MotorA_1A; 
int MotorA_1B;
int MotorB_1A; 
int MotorB_1B;

void initializareMotoare(int motorA1A, int motorA1B, int motorB1A, int motorB1B)
{
  MotorA_1A = motorA1A; 
  MotorA_1B = motorA1B;
  MotorB_1A = motorB1A;
  MotorB_1B = motorB1B;
  }

void controlDirectie (int directie , int viteza) 
 {
  switch(directie) {
      case 0:
        // STOP Normal
        digitalWrite(MotorA_1A, LOW);
        digitalWrite(MotorA_1B, LOW);
        digitalWrite(MotorB_1A, LOW);
        digitalWrite(MotorB_1B, LOW);
        break;
      case 1:
        // INAINTE
        analogWrite(MotorA_1A, viteza);
        digitalWrite (MotorA_1B, LOW);
        
        analogWrite(MotorB_1A, viteza);
        digitalWrite (MotorB_1B, LOW); 
        break;
     case 2:
        // INAPOI
        digitalWrite(MotorA_1A, LOW);
        analogWrite(MotorA_1B, viteza);
        digitalWrite(MotorB_1A, LOW);
        analogWrite(MotorB_1B, viteza);
        break;
     case 3:
        // DREAPTA LARG
        digitalWrite(MotorA_1A, LOW);
        digitalWrite(MotorA_1B, LOW);
        analogWrite(MotorB_1A, viteza);
        digitalWrite(MotorB_1B, LOW);
        break;
     case 4:
        // DREAPTA STRANS
        digitalWrite(MotorA_1A, LOW);
        analogWrite(MotorA_1B, viteza);
        analogWrite(MotorB_1A, viteza);
        digitalWrite(MotorB_1B, LOW);
        break;
     case 5:
        // STANGA LARG
        analogWrite(MotorA_1A, viteza);
        digitalWrite(MotorA_1B, LOW);
        digitalWrite(MotorB_1A, LOW);
        digitalWrite(MotorB_1B, LOW);
        break;
     case 6:
        // STANGA STRANS
        analogWrite(MotorA_1A, viteza);
        digitalWrite(MotorA_1B, LOW);
        digitalWrite(MotorB_1A, LOW);
        analogWrite(MotorB_1B, viteza);
        break;
     case 10:
        // STOP RAPID
        digitalWrite(MotorA_1A, HIGH);
        digitalWrite(MotorA_1B, HIGH);
        digitalWrite(MotorB_1A, HIGH);
        digitalWrite(MotorB_1B, HIGH);
        break;  
  }
}
