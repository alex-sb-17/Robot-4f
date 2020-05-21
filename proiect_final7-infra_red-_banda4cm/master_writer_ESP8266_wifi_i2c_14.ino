// versiune accesibila din internet
// include senzor temp
// include senzor ir de obstacole

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>

#include "index.h"

const char* ssid = "ZTE";
const char* password = "m1necraft";

ESP8266WebServer server(80);

#define DHTPIN D7 
#define DHTTYPE DHT11
DHT dht = DHT(DHTPIN, DHTTYPE);

#define SDA_PIN D4
#define SCL_PIN D3
#define IR_PINd D8
#define IR_PINs D12
#define CLP_PIN D10

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;

float t = 22; // temperatura
int stare_robot = 0;  // comunicata de arduino; 0 dc nu a primit date

// de vazut de ce nu merge cu PROGMEM

void setup() 
{
  pinMode(IR_PINd, INPUT);
  pinMode(IR_PINs, INPUT);
  pinMode(CLP_PIN, INPUT);
  
  dht.begin();
  
  Serial.begin(9600);
  WiFi.begin(ssid,password);

  Serial.println("");
  Serial.print("Astept sa ma conectez ...");
 
  while( WiFi.status() != WL_CONNECTED ) 
  {
    delay(500);
    Serial.print(".");
  }
  if( WiFi.status() == WL_CONNECTED ) 
  {
    // Mesaj Conectare
    Serial.println("");
    Serial.println("Wireless Conectat! ");  

    // Adresa IP
    Serial.print("Adresa IP: ");
    Serial.println( WiFi.localIP() );
   
    server.on("/", handle_index);
    server.on("/stop", handle_stop);
    server.on("/inainte", handle_inainte);
    server.on("/inapoi", handle_inapoi);
    server.on("/stanga", handle_stanga);
    server.on("/dreapta", handle_dreapta);
    //server.on("/returnValue", handle_return);
    server.on("/returnTemperatura", handle_temperatura);
    server.on("/returnStare", handle_stare);
    // start web server
    server.begin();
    Serial.println("Serverul a pornit!");
  
    Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER);
  }
  else 
  {
    // In caz ca nu reuseste sa se conecteze
    Serial.println("Eroare neprevazuta! Nu sunt conectat la WiFi !");
  }
}

void loop() 
{
  server.handleClient();
  delay(1);
  if(digitalRead(CLP_PIN))
  {
    trimite(100);  // a intilnit obstacol care a actionat clapeta frontala
  }
  /////////////////////////////////
  if(!(millis()%10000))
  {
    t = dht.readTemperature();
    Serial.println("temp trimisa = ");
      Serial.print(int(t));
    if(t >= 10 && t <= 40)
    {
      trimite(int(t));
      //Serial.println("temp trimisa = ");
      //Serial.print(int(t));
    }
  }
  if(!(millis()%1000))
  {
    Wire.requestFrom(I2C_SLAVE, 1);    // request 1 byte from slave device #8
    //while (Wire.available()) { // slave may send less than requested
    stare_robot = Wire.read(); 
    
    //Serial.println(stare_robot);
    //Serial.println(digitalRead(CLP_PIN));
  }
  if(!(millis()%400))
  {
    Serial.println("IR = ");
    Serial.print(digitalRead(D8));
    Serial.println(" ");
    if(!digitalRead(IR_PINd))
      trimite(120);  // obstacol la dreapta
    if(!digitalRead(IR_PINs))
      trimite(110);  // obstacol la stanga
  }
}

void trimite(int y)
{
    Wire.beginTransmission(I2C_SLAVE); // transmit to device #8
    Wire.write(y);              // sends one byte
    Serial.println("trimis comanda la arduino");
    Serial.print(y);
    Serial.println(" ");
    Wire.endTransmission();    // stop transmitting
}

void handle_index()
{
  server.send(200, "text/html", PAGINA_INDEX);
}
void handle_stop()
{
  server.send(200, "text/html", PAGINA_INDEX);
  trimite(0);
}

void handle_inainte()
{
  server.send(200, "text/html", PAGINA_INDEX);
  trimite(1);
}

void handle_inapoi()
{
  server.send(200, "text/html", PAGINA_INDEX);
  trimite(2);
}

void handle_stanga()
{
  server.send(200, "text/html", PAGINA_INDEX);
  trimite(5);
}

void handle_dreapta()
{
  server.send(200, "text/html", PAGINA_INDEX);
  trimite(3);
}

void handle_temperatura() {
   t = dht.readTemperature();
   //double t = analogRead(A0);
   String content = String(t);
   //Serial.print(t);
   server.send(200, "text/plain", content);
}

void handle_stare() {
   String content;
   switch (stare_robot)
   {
      case 1:
        content = "Urmarire linie";
        break;
      case 2:
        content = "Ocolire obstacole";
        break;
      case 3:
        content = "Control IR";
        break;
      case 4:
        content = "Control Web";
        break;
   }
   server.send(200, "text/plain", content);
}
