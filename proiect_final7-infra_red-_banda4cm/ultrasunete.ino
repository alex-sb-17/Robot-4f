//#include <Arduino.h>

float citesteDistanta(int trigger_pin, int echo_pin,int temp)
{
  long durata; //lucram cu microsecunde
  float distanta;
  
  digitalWrite(trigger_pin, LOW); // asteptam liniste inainte de a trimite bip 
  delayMicroseconds(2);
  
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);  // face bip 10us si se opreste
  // citim echo si vedem cat dureaza pana vine ecoul

  durata = pulseIn(echo_pin, HIGH);  // asteptam pana vine high - asteapta un nr de us pana se schimba starea
  // puteam face cu millis

  //Serial.println(durata);
  //distanta = (durata * 0.0342) / 2;
  distanta = (durata * (0.03315 + 0.0000607*temp)) / 2;
  return distanta;
  /*
  Serial.print("Distanta: ");
  Serial.print(distanta);
  Serial.println(" cm.");
  */
}
