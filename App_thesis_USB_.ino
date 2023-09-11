#define USE_ARDUINO_INTERRUPTS true
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <NewTone.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#define BLYNK_TEMPLATE_ID "TMPL69qwSaQZ8"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "h4fTQVXFmt2yyb8EAG5h_nNdbSdSDjgH"

#define BLYNK_PRINT DebugSerial
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial (2, 3);
#include <BlynkSimpleStream.h>
char auth[] = "h4fTQVXFmt2yyb8EAG5h_nNdbSdSDjgH";

#define buzzer_1 12
#define buzzer_2 13
#define servoMIN 150
#define servoMAX 600
#define light_1 6
#define light_2 7
#define light_3 8
#define light_4 9
#define light_5 10
#define light_6 11

#define indicator_1 2
#define indicator_2 3
#define indicator_3 4
#define indicator_4 5
#define irLED_1 22 
#define irLED_2 24

MAX30105 particleSensor;
const byte RATE_SIZE = 3; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int storage1=0;
int storage2=0;
int storage3=0;
int storage4=0;

int melody_1[] = { 262, 262, 196, 262, 262, 196, 262, 262 };
int noteDurations_1[] = { 10, 10, 10, 10, 10, 10, 10, 10 };
int melody_2[] = { 196, 196, 262, 196, 196, 262, 196, 196};
int noteDurations_2[] = { 10, 10, 10, 10, 10, 10, 10, 10 };

Adafruit_PWMServoDriver servodriver = Adafruit_PWMServoDriver();
int servo_1 = 0;
int servo_2 = 1;
int servo_3= 2;
int servo_4= 3;

int IRPin_1=A1; //IR
int IRPin_2=A2;

bool off_1= false; //button
bool off_2= false; 
bool off_3= false; 
bool off_4= false; 
bool missed_1 = false;
bool missed_2 = false;
bool missed_3 = false;
bool missed_4 = false;

int z=0;
int x=0;
int y=0; 

const byte ROWS = 4; //keypad
const byte COLS = 4; 
DS3231 rtc(SDA,SCL);
char key[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {37, 35, 33, 31}; 
byte colPins[COLS] = {45, 43, 41, 39};

Keypad keypad = Keypad(makeKeymap(key), rowPins, colPins, ROWS, COLS); 

int supplystored_1=0;
int supplystored_2=0;
int supplystored_3=0;
int supplystored_4=0;

int state= 0;
int ir_2= 0;
int ir_1= 0;

bool dispense_1 = false; //supply counter
bool dispense_2 = false;
bool dispense_3 = false;
bool dispense_4 = false;
bool stopper= false;

String storedHour_1;
String storedMinute_1;
String storedDay_1;
String storedMonth_1;
String storedYear_1;
String alarmDay_1;
String alarmMonth_1;
String alarmYear_1;

String storedHour_2;
String storedMinute_2;
String storedDay_2;
String storedMonth_2;
String storedYear_2;
String alarmDay_2;
String alarmMonth_2;
String alarmYear_2;

String storedHour_3;
String storedMinute_3;
String storedDay_3;
String storedMonth_3;
String storedYear_3;
String alarmDay_3;
String alarmMonth_3;
String alarmYear_3;

String storedHour_4;
String storedMinute_4;
String storedDay_4;
String storedMonth_4;
String storedYear_4;
String alarmDay_4;
String alarmMonth_4;
String alarmYear_4;

String D_app1;
String T_app1;
String D_app2;
String T_app2;
String D_app3;
String T_app3;
String D_app4;
String T_app4;

void storeMinute_1(String);
void storeHour_1(String);
void storeMinute_2(String);
void storeHour_2(String);
void storeMinute_3(String);
void storeHour_3(String);
void storeMinute_4(String);
void storeHour_4(String);

int cursor = 0;

LiquidCrystal_I2C lcd1(0x27,20,4); 
LiquidCrystal_I2C lcd2(0x26,16,2);  

void setup() {
        DebugSerial.begin(9600);
        Serial.begin(9600);
        Blynk.begin(Serial, auth);
      
        // Initialize sensor
        if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
        {
          //Serial.println("MAX30105 was not found. Please check wiring/power. ");
          while (1);
        }
        //Serial.println("Place your index finger on the sensor with steady pressure.");
      
        particleSensor.setup(); //Configure sensor with default settings
        particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
        particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
        
        pinMode(light_1, OUTPUT); //LED
        pinMode(light_2, OUTPUT);
        pinMode(light_3, OUTPUT);
        pinMode(light_4, OUTPUT);
        pinMode(light_5, OUTPUT);
        pinMode(light_6, OUTPUT);
        
        pinMode(indicator_1, OUTPUT);
        pinMode(indicator_2, OUTPUT);
        pinMode(indicator_3, OUTPUT);
        pinMode(indicator_4, OUTPUT);
      
        pinMode(irLED_1,OUTPUT);
        pinMode(IRPin_1,INPUT); // IR
        pinMode(irLED_2,OUTPUT);
        pinMode(IRPin_2,INPUT); // IR
        
        
        servodriver.begin();
        servodriver.setPWMFreq(60);
        servodriver.setPWM(servo_1, 0, servoMIN);
        servodriver.setPWM(servo_2, 0, servoMIN);
        servodriver.setPWM(servo_3, 0, servoMIN);
        servodriver.setPWM(servo_4, 0, servoMIN);
     
        lcd1.init();
        lcd2.init();
        lcd1.clear();         
        lcd2.clear();         
        lcd1.backlight();
        lcd2.backlight();

        lcd2.clear();
        lcd2.setCursor(0,0);
        lcd2.print ("  BPM Monitor");
        
        rtc.begin();
        rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
        rtc.setTime(9, 18, 0);     // Set the dafault time HR.MIN.SEC
        rtc.setDate(07, 9, 2023);   // Set the default date DD.MM.YYYY
      
        storedHour_1 = getHour_1();
        storedMinute_1 = getMinute_1();
      
        storedHour_2 = getHour_2();
        storedMinute_2 = getMinute_2();
      
        storedHour_3 = getHour_3();
        storedMinute_3 = getMinute_3();

        storedHour_4 = getHour_4();
        storedMinute_4 = getMinute_4();
      
        pinMode(buzzer_1, OUTPUT);
        digitalWrite(buzzer_1, LOW);
        pinMode(buzzer_2, OUTPUT);
        digitalWrite(buzzer_2, LOW);
       
      }

void loop() { 
        Blynk.run();
        int z=0;
        int x=0;
        int y=0;
        int supplystored_1;
        int supplystored_2;
        int supplystored_3;
        int supplystored_4;
        //IR_1();
        
        String dateString= rtc.getDateStr();
        String d= dateString.substring (0,2);
        String m= dateString.substring (3,5);
        String yr= dateString.substring (6,10);
        String currentDate= d+"."+ m+"."+ yr;
        
        String timeString = rtc.getTimeStr();
        String realHour = timeString.substring(0, 2);
        String realMinute = timeString.substring(3, 5);
        String currentTime = realHour+":"+realMinute+":00";
      
        String DOWString= rtc.getDOWStr();
        String currentDOW= DOWString;

        Main();
       
        bool alarm = false;
        
        while (alarm == false) {
        int z=0;
        int x=0;
        int y=0;
        int supplystored_1;
        int supplystored_2;
        int supplystored_3;
        int supplystored_4;
        
        String dateString= rtc.getDateStr();
        String d= dateString.substring (0,2);
        String m= dateString.substring (3,5);
        String yr= dateString.substring (6,10);
        String currentDate= d+"."+ m+"."+ yr;
        
        String timeString = rtc.getTimeStr();
        String realHour = timeString.substring(0, 2);
        String realMinute = timeString.substring(3, 5);
        String currentTime = realHour+":"+realMinute+":00";
      
        String DOWString= rtc.getDOWStr();
        String currentDOW= DOWString;
        
        Main();
        
        delay(2000); //required for print
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("Date:"+ currentDate);
        lcd1.setCursor(0, 1);
        lcd1.print("Time:"+ currentTime);
        lcd1.setCursor(0, 2);
        lcd1.print("Day: " + currentDOW);
        
        
        if (realHour == storedHour_1 && realMinute == storedMinute_1 && m == alarmMonth_1 && d == alarmDay_1 && yr == alarmYear_1) {
          alarm = true;
        } else if (realHour == storedHour_2 && realMinute == storedMinute_2 && m == alarmMonth_2 && d == alarmDay_2 && yr == alarmYear_2) {
          alarm = true;
        } else if (realHour == storedHour_3 && realMinute == storedMinute_3 && m == alarmMonth_3 && d == alarmDay_3 && yr == alarmYear_3) {
          alarm = true;
        } else if (realHour == storedHour_4 && realMinute == storedMinute_4 && m == alarmMonth_4 && d == alarmDay_4 && yr == alarmYear_4) {
          alarm = true;
        }else {
          alarm = false;
        }
        }
   
        if (realHour == storedHour_1 && realMinute == storedMinute_1 && m == alarmMonth_1 && d == alarmDay_1 && yr == alarmYear_1) { //alarm#1
          beatAvg = 0;
          ir_1 =digitalRead(IRPin_1);
          if (ir_1 == LOW){
            digitalWrite(irLED_1,HIGH);}
          else {   
            uint32_t period_1 = 2 * 60000L;       // 5 minutes loop
            for( uint32_t tStart_1 = millis();  (millis()-tStart_1) < period_1;  ){
               digitalWrite(irLED_1,LOW);
               soundAlarm();
               String dateString= rtc.getDateStr();
               String d= dateString.substring (0,2);
               String m= dateString.substring (3,5);
               String yr= dateString.substring (6,10);
               String currentDate= d+"."+ m+"."+ yr;
                    
               String timeString = rtc.getTimeStr();
               String realHour = timeString.substring(0, 2);
               String realMinute = timeString.substring(3, 5);
               String currentTime = realHour+":"+realMinute+":00";
                  
               String DOWString= rtc.getDOWStr();
               String currentDOW= DOWString;
                  
               Main();
                  
               delay(2000); //required for print
               lcd1.clear();
               lcd1.setCursor(0, 0);
               lcd1.print("Date:"+ currentDate);
               lcd1.setCursor(0, 1);
               lcd1.print("Time:"+ currentTime);
               lcd1.setCursor(0, 2);
               lcd1.print("Day: " + currentDOW);
 
                 uint32_t pulseperiod_1 = 0.75 * 60000L;       // 1 minute loop
                 for( uint32_t pulseStart_1 = millis();  (millis()-pulseStart_1) < pulseperiod_1;  ){
                  long irValue = particleSensor.getIR();
                  if (checkForBeat(irValue) == true){ // check for finger
                          //We sensed a beat!
                      long delta = millis() - lastBeat;
                      lastBeat = millis();
                      beatsPerMinute = 60 / (delta / 1000.0);
                      
                      if (beatsPerMinute < 255 && beatsPerMinute > 20){
                          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
                          rateSpot %= RATE_SIZE; //Wrap variable
                    
                          //Take average of readings
                          beatAvg = 0;
                          for (byte x = 0 ; x < RATE_SIZE ; x++)
                            beatAvg += rates[x];
                          beatAvg /= RATE_SIZE;
                          }
                           
                        lcd2.clear();
                        lcd2.setCursor(0,0);  
                        lcd2.print("BPM= ");
                        lcd2.print(beatsPerMinute);
                        lcd2.setCursor(0,1);
                        lcd2.print("Avg BPM= ");
                        lcd2.print(beatAvg);
                      
                      }
                      if (irValue < 50000) { 
                          soundAlarm();
                          lcd2.clear();
                          lcd2.setCursor(0,0);
                          lcd2.print("Nothing detected");
                          delay(500);
                          }
                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                          digitalWrite(irLED_1,HIGH);
                          LED_OFF();
                          digitalWrite(buzzer_1,LOW);
                          digitalWrite(buzzer_2,LOW);
                          break;
                            }
                        }
                     // Serial.print ("done");
                      storage1 = beatAvg;
                      if (storage1 >=60 && storage1 <=100){
                        BPMDisp();
                        soundAlarm();
                        servodriver.setPWM(servo_1, 0, servoMIN);
                        delay(500);
                        servodriver.setPWM(servo_1, 0, servoMAX);
                        delay(500);
                        servodriver.setPWM(servo_1, 0, servoMIN);
                        dispense_1 = true; 
                        IR_2();
                       // LED_ON();
                        digitalWrite(indicator_1, HIGH); 
                        off_1 = true;
                        break;
                        }
                       if (storage1 == 0){
                        missed_1 = true;
                        } 
                     else {
                        BPMNotDisp();
                        digitalWrite(indicator_1, HIGH);
                        off_1 = true;
                        break;
                      }   
                      
                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                          break;
                          }
                          //break;}

                      if (missed_1== true ){
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print ("   Time limit");
                        lcd2.setCursor(0,1);
                        lcd2.print ("  Missed dosage");
                        digitalWrite(indicator_1, HIGH);
                        off_1 = true;}
                      }}
                       LED_OFF();  
                      //digitalWrite(irLED_1,LOW);
                      digitalWrite(buzzer_1,LOW);
                      digitalWrite(buzzer_2,LOW);
                      digitalWrite(indicator_1, HIGH);  

                }
                  
 if (realHour == storedHour_2 && realMinute == storedMinute_2 && m == alarmMonth_2 && d == alarmDay_2 && yr == alarmYear_2) { //alarm#2
              beatAvg = 0;
                ir_1 =digitalRead(IRPin_1);
                  if (ir_1 == LOW){
                     digitalWrite(irLED_1,HIGH);
                     }
                  else {   
                  uint32_t period_2 = 5 * 60000L;       // 5 minutes loop
                for( uint32_t tStart_2 = millis();  (millis()-tStart_2) < period_2;  ){
                    digitalWrite(irLED_1,LOW);
                    soundAlarm();
                    String dateString= rtc.getDateStr();
                    String d= dateString.substring (0,2);
                    String m= dateString.substring (3,5);
                    String yr= dateString.substring (6,10);
                    String currentDate= d+"."+ m+"."+ yr;
                    
                    String timeString = rtc.getTimeStr();
                    String realHour = timeString.substring(0, 2);
                    String realMinute = timeString.substring(3, 5);
                    String currentTime = realHour+":"+realMinute+":00";
                  
                    String DOWString= rtc.getDOWStr();
                    String currentDOW= DOWString;
                    
                    Main();
                    
                    delay(2000); //required for print
                    lcd1.clear();
                    lcd1.setCursor(0, 0);
                    lcd1.print("Date:"+ currentDate);
                    lcd1.setCursor(0, 1);
                    lcd1.print("Time:"+ currentTime);
                    lcd1.setCursor(0, 2);
                    lcd1.print("Day: " + currentDOW);
 
                    uint32_t pulseperiod_2 = 1 * 60000L;       // 1 minute loop
                     for( uint32_t pulseStart_2 = millis();  (millis()-pulseStart_2) < pulseperiod_2;  ){
                        long irValue = particleSensor.getIR();
                        if (checkForBeat(irValue) == true){ // check for finger
                          //We sensed a beat!
                          long delta = millis() - lastBeat;
                          lastBeat = millis();
                    
                        beatsPerMinute = 60 / (delta / 1000.0);
                    
                        if (beatsPerMinute < 255 && beatsPerMinute > 20)
                        {
                          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
                          rateSpot %= RATE_SIZE; //Wrap variable
                    
                          //Take average of readings
                          beatAvg = 0;
                          for (byte x = 0 ; x < RATE_SIZE ; x++)
                            beatAvg += rates[x];
                          beatAvg /= RATE_SIZE;
                        } 
                      lcd2.clear();
                      lcd2.setCursor(0,0);  
                      lcd2.print("BPM= ");
                      lcd2.print(beatsPerMinute);
                      lcd2.setCursor(0,1);
                      lcd2.print("Avg BPM= ");
                      lcd2.print(beatAvg);
                      
                      }
                      if (irValue < 50000) { 
                        soundAlarm();
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print("Nothing detected");
                        delay(500);

                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                        digitalWrite(buzzer_1,LOW);
                        digitalWrite(buzzer_2,LOW);
                        break;
                          }
                        }}
                      //Serial.print ("done");
                      
                      storage1 = beatAvg;
                      if (storage1 >=60 && storage1 <=100){
                        BPMDisp();
                        soundAlarm();
                        servodriver.setPWM(servo_2, 0, servoMIN);
                        delay(500);
                        servodriver.setPWM(servo_2, 0, servoMAX);
                        delay(500);
                        servodriver.setPWM(servo_2, 0, servoMIN);
                        dispense_2 = true; 
                        IR_2();
                       // LED_ON();
                        digitalWrite(indicator_2, HIGH); 
                        off_2 = true;
                        break;
                        }
                      if (storage1 == 0){
                        missed_2 = true;
                        }
                     else {
                        BPMNotDisp();
                        digitalWrite(indicator_2, HIGH);
                        off_2 = true;
                        break;
                      }   
                      
                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                          break;
                          }
                          //break;}
                       if (missed_2== true ){
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print ("   Time limit");
                        lcd2.setCursor(0,1);
                        lcd2.print ("  Missed dosage");
                        digitalWrite(indicator_2, HIGH);
                        off_2 = true;}
                      }}
                       LED_OFF();  
                      digitalWrite(irLED_1,LOW);
                      digitalWrite(buzzer_1,LOW);
                      digitalWrite(buzzer_2,LOW);
                      digitalWrite(indicator_2, HIGH);  

                }

                if (realHour == storedHour_3 && realMinute == storedMinute_3 && m == alarmMonth_3 && d == alarmDay_3 && yr == alarmYear_3) { //alarm#1
                beatAvg = 0;
                ir_1 =digitalRead(IRPin_1);
                  if (ir_1 == LOW){
                     digitalWrite(irLED_1,HIGH);
                     }
                  else {   
                  uint32_t period_3 = 5 * 60000L;       // 5 minutes loop
                for( uint32_t tStart_3 = millis();  (millis()-tStart_3) < period_3;  ){
                    digitalWrite(irLED_1,LOW);
                    soundAlarm();
                    String dateString= rtc.getDateStr();
                    String d= dateString.substring (0,2);
                    String m= dateString.substring (3,5);
                    String yr= dateString.substring (6,10);
                    String currentDate= d+"."+ m+"."+ yr;
                    
                    String timeString = rtc.getTimeStr();
                    String realHour = timeString.substring(0, 2);
                    String realMinute = timeString.substring(3, 5);
                    String currentTime = realHour+":"+realMinute+":00";
                  
                    String DOWString= rtc.getDOWStr();
                    String currentDOW= DOWString;
                    
                    Main();
                    
                    delay(2000); //required for print
                    lcd1.clear();
                    lcd1.setCursor(0, 0);
                    lcd1.print("Date:"+ currentDate);
                    lcd1.setCursor(0, 1);
                    lcd1.print("Time:"+ currentTime);
                    lcd1.setCursor(0, 2);
                    lcd1.print("Day: " + currentDOW);
 
                    uint32_t pulseperiod_3 = 1 * 60000L;       // 1 minute loop
                     for( uint32_t pulseStart_3 = millis();  (millis()-pulseStart_3) < pulseperiod_3;  ){
                        long irValue = particleSensor.getIR();
                        if (checkForBeat(irValue) == true){ // check for finger
                          //We sensed a beat!
                          long delta = millis() - lastBeat;
                          lastBeat = millis();
                    
                        beatsPerMinute = 60 / (delta / 1000.0);
                    
                        if (beatsPerMinute < 255 && beatsPerMinute > 20)
                        {
                          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
                          rateSpot %= RATE_SIZE; //Wrap variable
                    
                          //Take average of readings
                          beatAvg = 0;
                          for (byte x = 0 ; x < RATE_SIZE ; x++)
                            beatAvg += rates[x];
                          beatAvg /= RATE_SIZE;
                        } 
                      lcd2.clear();
                      lcd2.setCursor(0,0);  
                      lcd2.print("BPM= ");
                      lcd2.print(beatsPerMinute);
                      lcd2.setCursor(0,1);
                      lcd2.print("Avg BPM= ");
                      lcd2.print(beatAvg);
                      
                      }
                      if (irValue < 50000) { 
                        soundAlarm();
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print("Nothing detected");
                        delay(500);

                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                        digitalWrite(buzzer_1,LOW);
                        digitalWrite(buzzer_2,LOW);
                          }
                        }}
                     // Serial.print ("done");
                      
                      storage1 = beatAvg;
                      if (storage1 >=60 && storage1 <=100){
                        BPMDisp();
                        soundAlarm();
                        servodriver.setPWM(servo_3, 0, servoMIN);
                        delay(500);
                        servodriver.setPWM(servo_3, 0, servoMAX);
                        delay(500);
                        servodriver.setPWM(servo_3, 0, servoMIN);
                        dispense_3 = true; 
                        IR_2();
                       // LED_ON();
                        digitalWrite(indicator_3, HIGH); 
                        off_3 = true;
                        break;
                        }
                     if (storage1 == 0){
                        missed_3 = true;
                        } 
                     else {
                        BPMNotDisp();
                        digitalWrite(indicator_3, HIGH);
                        off_3 = true;
                        break;
                      }   
                      
                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                          break;
                          }
                          //break;}
                      if (missed_3== true ){
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print ("   Time limit");
                        lcd2.setCursor(0,1);
                        lcd2.print ("  Missed dosage");
                        digitalWrite(indicator_3, HIGH);
                        off_1 = true;} 
                      }}
                       LED_OFF();  
                      digitalWrite(irLED_1,LOW);
                      digitalWrite(buzzer_1,LOW);
                      digitalWrite(buzzer_2,LOW);
                      digitalWrite(indicator_3, HIGH);  

                }

                if (realHour == storedHour_4 && realMinute == storedMinute_4 && m == alarmMonth_4 && d == alarmDay_4 && yr == alarmYear_4) { //alarm#1
                beatAvg = 0;
                ir_1 =digitalRead(IRPin_1);
                  if (ir_1 == LOW){
                     digitalWrite(irLED_1,HIGH);
                     }
                  else {   
                  uint32_t period_4 = 5 * 60000L;       // 5 minutes loop
                for( uint32_t tStart_4 = millis();  (millis()-tStart_4) < period_4;  ){
                    digitalWrite(irLED_1,LOW);
                    soundAlarm();
                    String dateString= rtc.getDateStr();
                    String d= dateString.substring (0,2);
                    String m= dateString.substring (3,5);
                    String yr= dateString.substring (6,10);
                    String currentDate= d+"."+ m+"."+ yr;
                    
                    String timeString = rtc.getTimeStr();
                    String realHour = timeString.substring(0, 2);
                    String realMinute = timeString.substring(3, 5);
                    String currentTime = realHour+":"+realMinute+":00";
                  
                    String DOWString= rtc.getDOWStr();
                    String currentDOW= DOWString;
                    
                    Main();
                    
                    delay(2000); //required for print
                    lcd1.clear();
                    lcd1.setCursor(0, 0);
                    lcd1.print("Date:"+ currentDate);
                    lcd1.setCursor(0, 1);
                    lcd1.print("Time:"+ currentTime);
                    lcd1.setCursor(0, 2);
                    lcd1.print("Day: " + currentDOW);
 
                    uint32_t pulseperiod_4 = 1 * 60000L;       // 1 minute loop
                     for( uint32_t pulseStart_4 = millis();  (millis()-pulseStart_4) < pulseperiod_4;  ){
                        long irValue = particleSensor.getIR();
                        if (checkForBeat(irValue) == true){ // check for finger
                          //We sensed a beat!
                          long delta = millis() - lastBeat;
                          lastBeat = millis();
                    
                        beatsPerMinute = 60 / (delta / 1000.0);
                    
                        if (beatsPerMinute < 255 && beatsPerMinute > 20)
                        {
                          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
                          rateSpot %= RATE_SIZE; //Wrap variable
                    
                          //Take average of readings
                          beatAvg = 0;
                          for (byte x = 0 ; x < RATE_SIZE ; x++)
                            beatAvg += rates[x];
                          beatAvg /= RATE_SIZE;
                        } 
                      lcd2.clear();
                      lcd2.setCursor(0,0);  
                      lcd2.print("BPM= ");
                      lcd2.print(beatsPerMinute);
                      lcd2.setCursor(0,1);
                      lcd2.print("Avg BPM= ");
                      lcd2.print(beatAvg);
                      
                      }
                      if (irValue < 50000) { 
                        soundAlarm();
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print("Nothing detected");
                        delay(500);

                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                        digitalWrite(buzzer_1,LOW);
                        digitalWrite(buzzer_2,LOW);
                        break; 
                          }
                        }}
                    //  Serial.print ("done");
                      
                      storage1 = beatAvg;
                      if (storage1 >=60 && storage1 <=100){
                        BPMDisp();
                        soundAlarm();
                        servodriver.setPWM(servo_4, 0, servoMIN);
                        delay(500);
                        servodriver.setPWM(servo_4, 0, servoMAX);
                        delay(500);
                        servodriver.setPWM(servo_4, 0, servoMIN);
                        dispense_4 = true; 
                        IR_2();
                       // LED_ON();
                        digitalWrite(indicator_4, HIGH);
                        off_4 = true;
                        break; 
                        }
                      if (storage1 == 0){
                        missed_4 = true;
                        }
                     else {
                        BPMNotDisp();
                        digitalWrite(indicator_4, HIGH);
                        off_4 = true;
                        break;
                      }   
                      
                      ir_1 =digitalRead(IRPin_1);
                      if (ir_1==LOW){
                        digitalWrite(irLED_1,HIGH);
                        LED_OFF();
                          break;
                          }
                          //break;}
                      if (missed_4== true ){
                        lcd2.clear();
                        lcd2.setCursor(0,0);
                        lcd2.print ("   Time limit");
                        lcd2.setCursor(0,1);
                        lcd2.print ("  Missed dosage");
                        digitalWrite(indicator_4, HIGH);
                        off_4 = true;}
                      }}
                       LED_OFF();  
                      digitalWrite(irLED_1,LOW);
                      digitalWrite(buzzer_1,LOW);
                      digitalWrite(buzzer_2,LOW);
                      digitalWrite(indicator_4, HIGH);  

                } 
                
                if(off_1 == true && off_2 == true && off_3 == true && off_4 == true){
                  digitalWrite(indicator_1, LOW);  
                  digitalWrite(indicator_2, LOW);  
                  digitalWrite(indicator_3, LOW);  
                  digitalWrite(indicator_4, LOW);  
                  } 


             
              }

/*..........................................................................*/
String getInputFromKeyBoard(int n) {
  String num = "";
  String number;
  String confirmNumber;
  int i = 0;

  while (1) {
    char key = keypad.getKey();
    if (key) {
      digitalWrite(buzzer_1, HIGH);
      digitalWrite(buzzer_2, HIGH);
      
      delay(100);
      digitalWrite(buzzer_1, LOW);
      digitalWrite(buzzer_2, LOW);
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
        i++;
        if (i <= n) {
          num += key;
        }
        lcd1.setCursor(14, 0);
        lcd1.print(num);
        lcd1.setCursor(0, 1);
        lcd1.print("*Cancel #Confirm");
        }

      if (key == '#' && i == n) {
        break;}

      if (key == '*') {
        MainMenu();}

      if ((key == '#' || key == 'B' || key == 'C') && i < n) {
        lcd1.clear();
        lcd1.print("Invalid Number!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
      }
      if (i > n) {
        lcd1.clear();
        lcd1.print("Incomplete input!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
      }

      if (i == n && ( key == 'B' || key == 'C' || key == 'D')) {
        lcd1.clear();
        lcd1.print("Invalid Option!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
      }}}
  return num;
  }
   
int getData() {
  lcd1.setCursor (0,1);
  lcd1.print ("*Cancel #Confirm");
  
  String container = "";
  lcd1.setCursor(15, 0);
  while (true) {
      char c = keypad.getKey();
    if (c == '*'){
      break;
      }  
    if (c == '#') {
      break;
  } else if (isDigit(c)) {
      container += c;
      lcd1.print(c);
  } else {
      //Nothing
    }
  }
  return container.toInt();
}
void storeHour_1(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k, String(n[k]).toInt());
  }}

void storeMinute_1(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k+2, String(n[k]).toInt());
  }}
String getHour_1() {
  String newHour_1 = "";
  for (int i = 0; i < 2; i++) {
    newHour_1 += EEPROM.read(i);
    //Serial.println(EEPROM.read(i));
  }
  return newHour_1;
}
String getMinute_1() {
  String newMinute_1 = "";
  for (int i = 2; i <4; i++) {
    newMinute_1 += EEPROM.read(i);
    //Serial.println(EEPROM.read(i));
  }
  return newMinute_1;
}

void storeHour_2(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k, String(n[k]).toInt());
  }}

void storeMinute_2(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k+2, String(n[k]).toInt());
  }}
String getHour_2() {
  String newHour_2 = "";
  for (int i = 0; i < 2; i++) {
    newHour_2 += EEPROM.read(i);
  //  Serial.println(EEPROM.read(i));
  }
  return newHour_2;
}
String getMinute_2() {
  String newMinute_2 = "";
  for (int i = 2; i <4; i++) {
    newMinute_2 += EEPROM.read(i);
  //  Serial.println(EEPROM.read(i));
  }
  return newMinute_2;
}

void storeHour_3(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k, String(n[k]).toInt());
  }}

void storeMinute_3(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k+2, String(n[k]).toInt());
  }}
String getHour_3() {
  String newHour_3 = "";
  for (int i = 0; i < 2; i++) {
    newHour_3 += EEPROM.read(i);
   // Serial.println(EEPROM.read(i));
  }
  return newHour_3;
}
String getMinute_3() {
  String newMinute_3 = "";
  for (int i = 2; i <4; i++) {
    newMinute_3 += EEPROM.read(i);
 //   Serial.println(EEPROM.read(i));
  }
  return newMinute_3;
}
void storeHour_4(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k, String(n[k]).toInt());
  }}

void storeMinute_4(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k+2, String(n[k]).toInt());
  }}
String getHour_4() {
  String newHour_4 = "";
  for (int i = 0; i < 2; i++) {
    newHour_4 += EEPROM.read(i);
    //Serial.println(EEPROM.read(i));
  }
  return newHour_4;
}
String getMinute_4() {
  String newMinute_4 = "";
  for (int i = 2; i <4; i++) {
    newMinute_4 += EEPROM.read(i);
   // Serial.println(EEPROM.read(i));
  }
  return newMinute_4;
}

void soundAlarm(){
  for (int thisNote_1 = 0; thisNote_1 < 8; thisNote_1++) { // Loop through the notes in the array.
    int noteDuration_1 = 1000/noteDurations_1[thisNote_1];
    NewTone(buzzer_1, melody_1[thisNote_1], noteDuration_1); // Play thisNote for noteDuration. 
    delay(noteDuration_1 * 4 / 3); // Wait while the tone plays in the background, plus another 33% delay between notes.
  }
   for (int thisNote_2 = 0; thisNote_2 < 8; thisNote_2++) { // Loop through the notes in the array.
    int noteDuration_2 = 1000/noteDurations_2[thisNote_2];
    NewTone(buzzer_2, melody_2[thisNote_2], noteDuration_2); // Play thisNote for noteDuration.
    delay(noteDuration_2 * 4 / 3); // Wait while the tone plays in the background, plus another 33% delay between notes.
  }
  LED_ON();
  }
void BPMDisp(){
      lcd2.clear();
      lcd2.setCursor(0,0); 
      lcd2.print (storage1); 
      lcd2.print(" is normal BPM");
      delay(1500); 
  }
void BPMNotDisp(){
     LED_OFF();
     digitalWrite(buzzer_1,LOW);
     digitalWrite(buzzer_2,LOW);
     lcd2.clear();
     lcd2.setCursor(0,0);  
     lcd2.print (storage1);
     lcd2.print(" abnormal BPM");
     lcd2.setCursor(0,1);  
     lcd2.print("Try again later");
     delay(1500);            
     servodriver.setPWM(servo_1, 0, servoMIN);
     servodriver.setPWM(servo_2, 0, servoMIN);
     servodriver.setPWM(servo_3, 0, servoMIN);
     servodriver.setPWM(servo_4, 0, servoMIN);
    }
    
void LED_ON (){
  digitalWrite(light_1, HIGH); 
  digitalWrite(light_2, HIGH); // Turn on LED2 
  digitalWrite(light_3, HIGH); // Turn on LED3 
  digitalWrite(light_4, HIGH); // Turn on LED4
  digitalWrite(light_5, HIGH);
  digitalWrite(light_6, HIGH); 
  delay (500);
  LED_OFF();

  digitalWrite(light_1, HIGH); 
  digitalWrite(light_2, HIGH); // Turn on LED2 
  digitalWrite(light_3, HIGH); // Turn on LED3 
  digitalWrite(light_4, HIGH); // Turn on LED4
  digitalWrite(light_5, HIGH);
  digitalWrite(light_6, HIGH); 
  delay (500);
  LED_OFF();
  }
  
void LED_OFF(){
  digitalWrite(light_1, LOW); 
  digitalWrite(light_2, LOW); // Turn on LED2 
  digitalWrite(light_3, LOW); // Turn on LED3 
  digitalWrite(light_4, LOW); // Turn on LED4
  digitalWrite(light_5, LOW); 
  digitalWrite(light_6, LOW); 
  delay (500);
  }
      
void Main(){
 // Serial.print("press");
  char key = keypad.getKey();
      if (key) {
        digitalWrite(buzzer_1, HIGH);
        digitalWrite(buzzer_2, HIGH);
        delay(100);
        digitalWrite(buzzer_1, LOW);
        digitalWrite(buzzer_2, LOW);
        
     switch (key){
        case 'A':
        MainMenu();
        break;
    }}
  }
     
void MainMenu(){
        char key = keypad.getKey();
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("A: Schedule");
        lcd1.setCursor(0, 1);
        lcd1.print("B: Supply");
        lcd1.setCursor(0, 2);
        lcd1.print("C: Alarm");
        lcd1.setCursor(0, 3);
        lcd1.print("D: Back");
        
 while (y<1){
      char key = keypad.getKey();
           
    if (key== 'A'){ 
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("  Schedule");
        Schedule();
        break;}
        
   else if (key== 'B'){
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print(" Supply Monitor");
        supplymonitor();
        break;}

    if (key== 'C'){
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("      Alarms ");
        alarmlist();
        break;}
        
     if (key== 'D'){
        break;}    
        
        }
      }    
void Schedule(){
        char key = keypad.getKey();
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("A: Schedule #1");
        lcd1.setCursor(0,1);
        lcd1.print("B: Schedule #2");
        lcd1.setCursor(0,2);
        lcd1.print("C: Schedule #3");
        lcd1.setCursor(0,3);
        lcd1.print("D: Schedule #4");
       
while (y<1){
  char key = keypad.getKey();
    if (key== 'A'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("   Schedule #1");
        delay(1000);
        zone_1();
        MainMenu();
        break;}

   if (key== 'B'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("  Schedule #2");
        delay(1000);
        zone_2();
        MainMenu();
        break;}
    
    if (key== 'C'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("  Schedule #3");
        delay(1000);
        zone_3();
        MainMenu();
        break;}
        
    if (key== 'D'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("  Schedule #4");
        delay(1000);
        zone_4();
        MainMenu();
        break;}
  }}
  
void alarmlist(){
  bool leave = false;
   while(leave== false){
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("A#1 Date:");                   
      String storedAlarmDate_1= alarmDay_1+"."+ alarmMonth_1 +"." + alarmYear_1;
      lcd1.print(storedAlarmDate_1);
      lcd1.setCursor(0, 1);
      lcd1.print("A#1 Time:");
      String storedAlarmTime_1 = storedHour_1 + ":" + storedMinute_1 + ":00";
      lcd1.print(storedAlarmTime_1);
      lcd1.setCursor(0, 2);
      lcd1.print("A#2 Date:");
      String storedAlarmDate_2= alarmDay_2+"."+ alarmMonth_2 + "." + alarmYear_2;
      lcd1.print(storedAlarmDate_2);
      lcd1.setCursor(0, 3);
      lcd1.print("A#2 Time:");
      String storedAlarmTime_2 = storedHour_2 + ":" + storedMinute_2 + ":00";
      lcd1.print(storedAlarmTime_2);
      
      delay(3000);
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("A#3 Date:");                   
      String storedAlarmDate_3= alarmDay_3+"."+ alarmMonth_3 + "." + alarmYear_3;
      lcd1.print(storedAlarmDate_3);
      lcd1.setCursor(0, 1);
      lcd1.print("A#3 Time:");
      String storedAlarmTime_3 = storedHour_3 + ":" + storedMinute_3 + ":00";
      lcd1.print(storedAlarmTime_3);
       lcd1.setCursor(0, 2);
      lcd1.print("A#4 Date:");                   
      String storedAlarmDate_4= alarmDay_4+"."+ alarmMonth_4 + "." + alarmYear_4;
      lcd1.print(storedAlarmDate_4);
      D_app4 = storedAlarmDate_4;
      lcd1.setCursor(0, 3);
      lcd1.print("A#4 Time:");
      String storedAlarmTime_4 = storedHour_4 + ":" + storedMinute_4 + ":00";
      lcd1.print(storedAlarmTime_4);
      //MainMenu();
      break;
   }
      } 
  
void zone_1(){
    char key = keypad.getKey();
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print("*: Change time");
    lcd1.setCursor(0,1);
    lcd1.print("A: Change date");
    
   while (y<1){
      char key = keypad.getKey();
      String getInputFromKeyBoard(int n);
    
    if (key == '*') {
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("*Set Hour:");
        String enteredHour_1 = getInputFromKeyBoard(2);
  
          if (enteredHour_1.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Minute:");
       
          String enteredMinute_1 = getInputFromKeyBoard(2);
          
          if (enteredMinute_1.length() == 2) {
            storeHour_1(enteredHour_1);
            storeMinute_1(enteredMinute_1);
            storedHour_1 = enteredHour_1;
            storedMinute_1 = enteredMinute_1;
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("  Alarm Time set to");
            lcd1.setCursor(0, 1);
            String alarmTime_1 = "     "+enteredHour_1 + ":" + enteredMinute_1 + ":00";
            lcd1.print(alarmTime_1);
            delay(1000);
          }}break;}
  
    if (key == 'A'){
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set Month:");
          String enteredMonth_1 = getInputFromKeyBoard(2);
            
         if (enteredMonth_1.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Day:");
            String enteredDay_1 = getInputFromKeyBoard(2);
          
          if (enteredDay_1.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Year:");
            String enteredYear_1 = getInputFromKeyBoard(4);

          if (enteredYear_1.length()== 4){
            String storedMonth_1= enteredMonth_1;
            String storedDay_1= enteredDay_1; 
            String storedYear_1= enteredYear_1;
            
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print(" Alarm Date set to");
            lcd1.setCursor(0, 1);
            alarmMonth_1= enteredMonth_1;
            alarmDay_1= enteredDay_1;
            alarmYear_1= enteredYear_1;
            
            String alarmDate_1= "    "+ enteredMonth_1 + "." + enteredDay_1 + "." + enteredYear_1;
            lcd1.print(alarmDate_1);
            delay(1000);
            D_app1 = enteredMonth_1 + "." + enteredDay_1 + "." + enteredYear_1;
            Blynk.virtualWrite(V0,D_app1);
          }}}break;}}
          }
void zone_2(){
      char key = keypad.getKey();
      lcd1.clear();
      lcd1.setCursor(0,0);
      lcd1.print("*: Change time");
      lcd1.setCursor(0,1);
      lcd1.print("A: Change date");
      
 while (y<1){
    char key = keypad.getKey();
    String getInputFromKeyBoard(int n);
  
    if (key == '*') {
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("*Set Hour:");
        String enteredHour_2 = getInputFromKeyBoard(2);
  
        if (enteredHour_2.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Minute:");
           String enteredMinute_2 = getInputFromKeyBoard(2);
          
          if (enteredMinute_2.length() == 2) {
            storeHour_2(enteredHour_2);
            storeMinute_2(enteredMinute_2);
            storedHour_2 = enteredHour_2;
            storedMinute_2 = enteredMinute_2;
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("  Alarm Time set to");
            lcd1.setCursor(0, 1);
            String alarmTime_2 = "     "+enteredHour_2 + ":" + enteredMinute_2 + ":00";
            lcd1.print(alarmTime_2);
            delay(1000);
          }}break;}
  
    if (key == 'A'){
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("*Set Month:");
        String enteredMonth_2 = getInputFromKeyBoard(2);
            
         if (enteredMonth_2.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Day:");
       
          String enteredDay_2 = getInputFromKeyBoard(2);
          
          if (enteredDay_2.length() == 2) {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("*Set Year:");
            String enteredYear_2 = getInputFromKeyBoard(4);

          if (enteredYear_2.length()== 4){
            String storedMonth_2= enteredMonth_2;
            String storedDay_2= enteredDay_2; 
            String storedYear_2= enteredYear_2;
            
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print(" Alarm Date set to");
            lcd1.setCursor(0, 1);
            alarmMonth_2= enteredMonth_2;
            alarmDay_2= enteredDay_2;
            alarmYear_2= enteredYear_2;
            String alarmDate_2= "     "+ enteredMonth_2 + "." + enteredDay_2 + ".]" + enteredYear_2;
            lcd1.print(alarmDate_2);
            delay(1000); 
          }}}break;}}
          }
          
void zone_3(){
    char key = keypad.getKey();
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print("*: Change time");
    lcd1.setCursor(0,1);
    lcd1.print("A: Change date");
    
     while (y<1){
      char key = keypad.getKey();
      String getInputFromKeyBoard(int n);
      
      if (key == '*') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set Hour:");
          String enteredHour_3 = getInputFromKeyBoard(2);
    
          if (enteredHour_3.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Minute:");
              String enteredMinute_3 = getInputFromKeyBoard(2);
            
            if (enteredMinute_3.length() == 2) {
              storeHour_3(enteredHour_3);
              storeMinute_3(enteredMinute_3);
              storedHour_3 = enteredHour_3;
              storedMinute_3 = enteredMinute_3;
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("  Alarm Time set to");
              lcd1.setCursor(0, 1);
              String alarmTime_3 = "     "+enteredHour_3 + ":" + enteredMinute_3 + ":00";
              lcd1.print(alarmTime_3);
              delay(1000);
            }}break;}
    
      if (key == 'A'){
           lcd1.clear();
           lcd1.setCursor(0, 0);
           lcd1.print("*Set Month:");
           String enteredMonth_3 = getInputFromKeyBoard(2);
              
           if (enteredMonth_3.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Day:");
              String enteredDay_3 = getInputFromKeyBoard(2);
            
           if (enteredDay_3.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Year:");
              String enteredYear_3 = getInputFromKeyBoard(4);
           
            if (enteredYear_3.length()== 4){ 
              String storedMonth_3= enteredMonth_3;
              String storedDay_3= enteredDay_3; 
              String storedYear_3= enteredYear_3;
              
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print(" Alarm Date set to");
              lcd1.setCursor(0, 1);
              alarmMonth_3= enteredMonth_3;
              alarmDay_3= enteredDay_3;
              alarmYear_3= enteredYear_3;
              String alarmDate_3= "     "+ enteredMonth_3 + "." + enteredDay_3 + "." + enteredYear_3;
              lcd1.print(alarmDate_3);
              delay(1000); 
            }}}break;}}
            }       

void zone_4(){
    char key = keypad.getKey();
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print("*: Change time");
    lcd1.setCursor(0,1);
    lcd1.print("A: Change date");
    
     while (y<1){
      char key = keypad.getKey();
      String getInputFromKeyBoard(int n);
      
      if (key == '*') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set Hour:");
          String enteredHour_4 = getInputFromKeyBoard(2);
    
          if (enteredHour_4.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Minute:");
              String enteredMinute_4 = getInputFromKeyBoard(2);
            
            if (enteredMinute_4.length() == 2) {
              storeHour_4(enteredHour_4);
              storeMinute_4(enteredMinute_4);
              storedHour_4 = enteredHour_4;
              storedMinute_4 = enteredMinute_4;
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("  Alarm Time set to");
              lcd1.setCursor(0, 1);
              String alarmTime_4 = "     "+enteredHour_4 + ":" + enteredMinute_4 + ":00";
              lcd1.print(alarmTime_4);
              delay(1000);
            }}break;}
    
      if (key == 'A'){
           lcd1.clear();
           lcd1.setCursor(0, 0);
           lcd1.print("*Set Month:");
           String enteredMonth_4 = getInputFromKeyBoard(2);
              
           if (enteredMonth_4.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Day:");
              String enteredDay_4= getInputFromKeyBoard(2);
            
           if (enteredDay_4.length() == 2) {
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print("*Set Year:");
              String enteredYear_4 = getInputFromKeyBoard(4);

           if (enteredYear_4.length() == 4){
              String storedMonth_4= enteredMonth_4;
              String storedDay_4= enteredDay_4; 
              String storedYear_4= enteredYear_4;
              
              lcd1.clear();
              lcd1.setCursor(0, 0);
              lcd1.print(" Alarm Date set to");
              lcd1.setCursor(0, 1);
              alarmMonth_4= enteredMonth_4;
              alarmDay_4= enteredDay_4;
              alarmYear_4= enteredYear_4;
              String alarmDate_4= "     "+ enteredMonth_4 + "." + enteredDay_4 + "." + enteredYear_4;
              lcd1.print(alarmDate_4);
              delay(1000); 
           }}}break;}}
            } 
                  
void supplymonitor(){
        char key = keypad.getKey();
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("A: Slot#1");
        lcd1.setCursor(0,1);
        lcd1.print("B: Slot#2");
        lcd1.setCursor(0,2);
        lcd1.print("C: Slot#3");
        lcd1.setCursor(0,3);
        lcd1.print("D: Slot#4");
       
while (y<1){
   char key = keypad.getKey();
    if (key== 'A'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("     Slot #1");
        delay(1000);
        slot_1();
        MainMenu();
        break;}

   if (key== 'B'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("    Slot #2");
        delay(1000);
        slot_2();
        MainMenu();
        break;}
    
    if (key== 'C'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("    Slot #3");
        delay(1000);
        slot_3();
        MainMenu();
        break;}
        
    if (key== 'D'){
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("    Slot #4");
        delay(1000);
        slot_4();
        MainMenu();
        break;}
  }}

void slot_1(){
      char key = keypad.getKey();
      lcd1.clear();
      lcd1.setCursor(0,0);
      lcd1.print("A: Set quantity");
      lcd1.setCursor(0,1);
      lcd1.print("*: View quantity");
  
   while (y<1){
    char key = keypad.getKey();
    
    if (key == 'A') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set quantity: ");
          int supply_1 = getData();

      if (supply_1 <= 15){    
          supplystored_1 = supply_1;
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Quantity set to: ");
          lcd1.print(supply_1);
          delay(1000);
          break;}
          
       if (key == '*'){
        break;
        }
       else{
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("Exceeded slot limit!");
        delay(1000);
        break;
        }}
  
   if (key == '*'){
         lcd1.clear();
         lcd1.setCursor(0, 0);
         lcd1.print("Medicine available   ");   
         lcd1.setCursor(0,1); 
         dispensecount();
         lcd1.print(supplystored_1); 
         delay(1000);  
         break;}} 
  }
  
void slot_2(){
      char key = keypad.getKey();
      lcd1.clear();
      lcd1.setCursor(0,0);
      lcd1.print("A: Set quantity");
      lcd1.setCursor(0,1);
      lcd1.print("*: View quantity");
  
   while (y<1){
    char key = keypad.getKey();
    
    if (key == 'A') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set quantity: ");
          int supply_2 = getData();

      if (supply_2 <= 15){    
          supplystored_2 = supply_2;
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Quantity set to: ");
          lcd1.print(supply_2);
          delay(1000);
          break;}
       if (key == '*'){
        break;
        }
        
       else{
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("Exceeded slot limit!");
        delay(1000);
        break;
        }}
  
   if (key == '*'){
         lcd1.clear();
         lcd1.setCursor(0, 0);
         lcd1.print("Medicine available   ");   
         lcd1.setCursor(0,1); 
         dispensecount();
         lcd1.print(supplystored_2); 
         delay(1000);  
         break;}} 
  }
void slot_3(){
      char key = keypad.getKey();
      lcd1.clear();
      lcd1.setCursor(0,0);
      lcd1.print("A: Set quantity");
      lcd1.setCursor(0,1);
      lcd1.print("*: View quantity");
  
   while (y<1){
    char key = keypad.getKey();
    
       if (key == 'A') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set quantity: ");
          int supply_3 = getData();

      if (supply_3 <= 15){    
          supplystored_3 = supply_3;
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Quantity set to: ");
          lcd1.print(supply_3);
          delay(1000);
          break;}
       if (key == '*'){
        break;
        }
        
       else{
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("Exceeded slot limit!");
        delay(1000);
        break;
        }}
  
   if (key == '*'){
         lcd1.clear();
         lcd1.setCursor(0, 0);
         lcd1.print("Medicine available   ");   
         lcd1.setCursor(0,1); 
         dispensecount();
         lcd1.print(supplystored_3); 
         delay(1000);  
         break;}} 
  }
  
void slot_4(){
      char key = keypad.getKey();
      lcd1.clear();
      lcd1.setCursor(0,0);
      lcd1.print("A: Set quantity");
      lcd1.setCursor(0,1);
      lcd1.print("*: View quantity");
  
   while (y<1){
    char key = keypad.getKey();
        if (key == 'A') {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("*Set quantity: ");
          int supply_4 = getData();

      if (supply_4 <= 15){    
          supplystored_4 = supply_4;
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Quantity set to: ");
          lcd1.print(supply_4);
          delay(1000);
          break;}
       if (key == '*'){
        break;
        }
        
       else{
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print("Exceeded slot limit!");
        delay(1000);
        break;
        }}
  
   if (key == '*'){
         lcd1.clear();
         lcd1.setCursor(0, 0);
         lcd1.print("Medicine available   ");   
         lcd1.setCursor(0,1); 
         dispensecount();
         lcd1.print(supplystored_4); 
         delay(1000);  
         break;}} 
  }  
//void IR_1(){
  //ir_1 =digitalRead(IRPin_1);
    // if (ir_1==0) {
      //   digitalWrite(irLED_1,HIGH);
      // }
    // else{
      //  digitalWrite(irLED_1,LOW);
  //}}   
void IR_2(){
   ir_2=digitalRead(IRPin_2);
     if (ir_2==0) {
         digitalWrite(irLED_2,HIGH);
         }
     else{
        digitalWrite(irLED_2,LOW);
  }}
  
void dispensecount(){
  if (dispense_1 == true){
    IR_2();
    supplystored_1 = supplystored_1 - 1; 
    dispense_1 = false;
    }
  if (dispense_2 == true){
     IR_2();
     supplystored_2 = supplystored_2 - 1;
     dispense_2 = false;
    }
  if (dispense_3 == true){
     IR_2();
     supplystored_3 = supplystored_3 - 1;
     dispense_3 = false;
    }
  if (dispense_4 == true){
     IR_2();
     supplystored_4 = supplystored_4 - 1;
     dispense_4= false;
    }
}
