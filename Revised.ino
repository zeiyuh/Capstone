#define USE_ARDUINO_INTERRUPTS true
#include <LiquidCrystal_I2C.h>

#define BLYNK_TEMPLATE_ID "TMPL6xBRa_QQe"
#define BLYNK_TEMPLATE_NAME "Mobile App"
#define BLYNK_AUTH_TOKEN "KqsDGkuwK3FoV4tWfzHCliEknldhOOua"

#define BLYNK_PRINT DebugSerial
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial (2, 3);
#include <BlynkSimpleStream.h>
char auth[] = "KqsDGkuwK3FoV4tWfzHCliEknldhOOua";

#define light_6 11
LiquidCrystal_I2C lcd1(0x27,20,4); 

void led (int led_value);
int store;
BLYNK_WRITE(V8){
  led (param.asInt());
  }

void led (int led_value){
  store = led_value;
  if (led_value == 1){
    digitalWrite(light_6, HIGH);
    }
  else {
    digitalWrite(light_6, LOW);
    }
  }   


void setup(){
  DebugSerial.begin(9600);
  Serial.begin(9600);
  Blynk.begin(Serial, auth);

  pinMode(light_6, OUTPUT);

  lcd1.init();
  lcd1.clear();         
  lcd1.backlight();
  }
void loop(){
  Blynk.run();
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print (store);
  delay(1000);
}
  
