#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"



#define SERVO1_PIN 32 // ESP32 pin GPIO26 connected to servo motor
#define SERVO2_PIN 27

#define IR1_PIN 18
#define IR2_PIN 17
#define IR3_PIN 16
#define IR4_PIN 5

#define LED1_PIN 0
#define LED2_PIN 13
#define LED3_PIN 15
#define LED4_PIN 3

#define LEDS_PIN 19
#define SLED1_PIN 12
#define SLED2_PIN 14

#define TRIG_PIN 23 // ESP32 pin GPIO23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 26 // ESP32 pin GPIO22 connected to Ultrasonic Sensor's ECHO pin


#define LR_PIN 35

const char* ssid     = "Device";
const char* password = "12345678";

AsyncWebServer server(80);


Servo servoMotor1;
Servo servoMotor2;

float duration_us, distance_cm;

int availableParkings = 0;

bool isLightingAuto = true;

String ledState;

bool enDoor = true;
bool exDoor = true;

bool ir1;
bool ir2;
bool ir3;
bool ir4;

bool isEntranceInUse = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() {
  // begin serial port
  Serial.begin (115200);
  pinsSetup();
  setParkings();
  serverSetup();

  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Smart Garage");
  lcdUpdate();

  delay(5000);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "noAv"){
    return String(availableParkings);
  }
  if(var == "car1"){
    if(!ir1){
      return "car1";
    }
    return "car";
  }
  if(var == "car2"){
    if(!ir2){
      return "car2";
    }
    return "car";
  }
  if(var == "car3"){
    if(!ir3){
      return "car3";
    }
    return "car";
  }
  if(var == "car4"){
    if(!ir4){
      return "car4";
    }
    return "car";
  }
  return String();
}

String getJSON(){
  String car1;
  String car2;
  String car3;
  String car4;
  if(!ir1){
    car1 = "\"car1\" : \"car1\",";
  }
  else{
    car1 = "\"car1\" : \"car\",";
  }  
  
  if(!ir2){
    car2 = "\"car2\" : \"car2\",";
  }
  else{
    car2 = "\"car2\" : \"car\",";
  }  

  if(!ir3){
    car3 = "\"car3\" : \"car3\",";
  }
  else{
    car3 = "\"car3\" : \"car\",";
  }  

  if(!ir4){
    car4 = "\"car4\" : \"car4\",";
  }
  else{
    car4 = "\"car4\" : \"car\",";
  }  

  String lightStatus;
  if(isLightingAuto){
    lightStatus = "\"lightStatus\" : \"auto\",";
  }
  else{
    if(digitalRead(LEDS_PIN) == HIGH){
      lightStatus = "\"lightStatus\" : \"on\",";
    }
    else{
       lightStatus = "\"lightStatus\" : \"off\",";
    }
  }

  String time;
  if(isNight()){
    time = "\"time\" : \"Night\",";
  }
  else{
    time = "\"time\" : \"Day\",";
  }

  String nearestObj = "\"nearestObj\" :  "+ String(distance_cm) + ",";
  String noAv = "\"noAv\" :  "+ String(availableParkings);
  return "{"+ car1 + car2 + car3 + car4 + nearestObj+ time + lightStatus + noAv + "}";
}

void serverSetup(){
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  
  ();

  // Start server
  server.begin();
}


void routing(){

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(isNight()){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    }
    else{
      request->send(SPIFFS, "/index2.html", String(), false, processor);
    }
  });
  
  server.on("/DarkGarage1.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/DarkGarage1.jpg", "image/jpg", false);
  });

  server.on("/LightGarage3.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/LightGarage3.jpg", "image/jpg", false);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/style2.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style2.css", "text/css");
  });

  server.on("/parking.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/parking3.html", String(),false, processor);
  });

  server.on("/parking-style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/parking-style.css", "text/css");
  });

  server.on("/jquery-3.6.4.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery-3.6.4.min.js", "text/javascript");
  });

  server.on("/parking.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/parking.jpg", "image/jpg", false);
  });

  server.on("/car.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/car.png", "image/png", false);
  });

  server.on("/getParkingStatus", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getJSON());
  });

  server.on("/getStatus", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getJSON());
  });

  server.on("/admin.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/admin.html", String(),false, processor);
  });

  server.on("/admin.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/admin.js", "text/javascript");
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  server.on("/openLights", HTTP_GET, [](AsyncWebServerRequest *request){
    isLightingAuto = false;
    openSideLights();
    request->send(200, "text/plain", "done");
  });

  server.on("/closeLights", HTTP_GET, [](AsyncWebServerRequest *request){
    isLightingAuto = false;
    closeSideLights();
    request->send(200, "text/plain", "done");
  });

  server.on("/autoLights", HTTP_GET, [](AsyncWebServerRequest *request){
    isLightingAuto = true;
    request->send(200, "text/plain", "done");
  });

  server.on("/openEntranceDoor", HTTP_GET, [](AsyncWebServerRequest *request){
    if(enDoor){
      enDoor = false;
      openEnteranceDoor();
      enDoor = true;
    }
    request->send(200, "text/plain", "done");
  });

  server.on("/openExitDoor", HTTP_GET, [](AsyncWebServerRequest *request){
    if(exDoor){
      exDoor = false;
      openExitDoor();
      exDoor = true;
    }
    request->send(200, "text/plain", "done");
  });

}


void pinsSetup(){
  pinMode(LR_PIN, INPUT);

  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(IR4_PIN, INPUT);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(SLED1_PIN, OUTPUT);
  pinMode(SLED2_PIN, OUTPUT);
 
  pinMode(LEDS_PIN, OUTPUT);

  servoMotor1.attach(SERVO1_PIN);
  servoMotor2.attach(SERVO2_PIN);
  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
}


void setParkings(){
  ir1 = digitalRead(IR1_PIN);
  ir2 = digitalRead(IR2_PIN);
  ir3 = digitalRead(IR3_PIN);
  ir4 = digitalRead(IR4_PIN);

  if(ir1){
    availableParkings++;
    digitalWrite(LED1_PIN, HIGH);
  }
  else{
    digitalWrite(LED1_PIN, LOW);
  }

  if(ir2){
    availableParkings++;
    digitalWrite(LED2_PIN, HIGH);
  }
  else{
    digitalWrite(LED2_PIN, LOW);
  }

  if(ir3){
    availableParkings++;
    digitalWrite(LED3_PIN, HIGH);
  }
  else{
    digitalWrite(LED3_PIN, LOW);
  }

  if(ir4){
    availableParkings++;
    digitalWrite(LED4_PIN, HIGH);
  }
  else{
    digitalWrite(LED4_PIN, LOW);
  }
}

void updateParkings(){
  int ir1Val = digitalRead(IR1_PIN);
  int ir2Val = digitalRead(IR2_PIN);
  int ir3Val = digitalRead(IR3_PIN);
  int ir4Val = digitalRead(IR4_PIN);

  if(ir1Val != ir1){
    if(ir1Val){
      availableParkings++;
      digitalWrite(LED1_PIN, HIGH);
      ir1 = 1;
      if(exDoor){
      exDoor = false;
      lcdUpdate();
      openExitDoor();
      exDoor = true;
    }
    }
    else{
      availableParkings--;
      lcdUpdate();
      digitalWrite(LED1_PIN, LOW);
      ir1 = 0;
    }
  }

  if(ir2Val != ir2){
    if(ir2Val){
      availableParkings++;
      digitalWrite(LED2_PIN, HIGH);
      ir2 = 1;
      if(exDoor){
      exDoor = false;
      lcdUpdate();
      openExitDoor();
      exDoor = true;
    }
    }
    else{
      availableParkings--;
      lcdUpdate();
      digitalWrite(LED2_PIN, LOW);
      ir2 = 0;
    }
  }

  if(ir3Val != ir3){
    if(ir3Val){
      availableParkings++;
      digitalWrite(LED3_PIN, HIGH);
      ir3 = 1;
      if(exDoor){
      exDoor = false;
      lcdUpdate();
      openExitDoor();
      exDoor = true;
    }
    }
    else{
      availableParkings--;
      lcdUpdate();
      digitalWrite(LED3_PIN, LOW);
      ir3 = 0;
    }
  }

  if(ir4Val != ir4){
    if(ir4Val){
      availableParkings++;
      digitalWrite(LED4_PIN, HIGH);
      ir4 = 1;
      if(exDoor){
      exDoor = false;
      lcdUpdate();
      openExitDoor();
      exDoor = true;
    }
    }
    else{
      availableParkings--;
      lcdUpdate();
      digitalWrite(LED4_PIN, LOW);
      ir4 = 0;
    }
  }
}

void openSideLights(){
  digitalWrite(LEDS_PIN, HIGH);
  digitalWrite(SLED1_PIN, HIGH);
  digitalWrite(SLED2_PIN, HIGH);
}

void closeSideLights(){
  digitalWrite(LEDS_PIN, LOW);
  digitalWrite(SLED1_PIN, LOW);
  digitalWrite(SLED2_PIN, LOW);
}


bool isNight(){
  int LR = analogRead(LR_PIN);
  Serial.println(LR);
  if(LR >= 3200){
    return true;
  }
  return false;
}

void openEnteranceDoor(){
  for (int pos = 90; pos >= 0; pos -= 1) {
    // in steps of 1 degree
    servoMotor1.write(pos);
    delay(10); // waits 15ms to reach the position
  }

  delay(1000);

  for (int pos = 0; pos <= 90; pos += 1) {
    // in steps of 1 degree
    servoMotor1.write(pos);
    delay(10); // waits 15ms to reach the position
  }
}

void openExitDoor(){
  delay(1000);
  for (int pos = 0; pos <= 80; pos += 1) {
    // in steps of 1 degree
    servoMotor2.write(pos);
    delay(10); // waits 15ms to reach the position
  }

  delay(2000);

  for (int pos = 80; pos >= 0; pos -= 1) {
    // in steps of 1 degree
    servoMotor2.write(pos);
    delay(10); // waits 15ms to reach the position
  }
}

bool isObjeectNearDoor(){
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  if(distance_cm <= 15 && distance_cm > 8){
    return true;
  }
  return false;
}

void printAvailableParkings(){

  Serial.print("available parrkings:   ");
  Serial.print(availableParkings);
}

void lightsControl(){
  if(isLightingAuto){
    if(isNight()){
      openSideLights();
    }else{
      closeSideLights();
    }
  }
}

void lcdUpdate(){
  lcd.setCursor(0,1);
  lcd.print("Empty Parking: " + String(availableParkings));
}

void loop() {
  updateParkings();

  if(isObjeectNearDoor() && availableParkings && enDoor)
  {
    enDoor = false;
    openEnteranceDoor();
    enDoor = true;
  }
  
  lightsControl();

  printAvailableParkings();

  
  delay(200);

}