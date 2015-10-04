#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h>
#include <EEPROM.h>
#include <AFMotor.h>
#include <LiquidCrystal.h>

/* PIN MAPPINGS
   ------------
   Motor Shield: 
     VCC, Gnd, Vin, 4, 7, 8, 11, 12
     
   Sound sensor:
     A3
     
   LCD:
     9, 10, 13, A0, A1, A2
     
   Front panel buttons:
     2, 3
     
   RTC:
     SCL, SDA
     
*/  
LiquidCrystal lcd(10, 9, 13, 14, 15, 16);
AF_DCMotor slot(2, MOTOR12_1KHZ);
const int decrease = 2;
const int increase = 3;

volatile int threshold = 416;
volatile int increased = 0;
volatile int decreased = 0;
const int address = 0;
int level; 
static bool slotIsOpen = false;

void setup() {
  pinMode(increase, INPUT);
  pinMode(decrease, INPUT);
  
  //Serial.begin(9600);
  if(EEPROM.read(address) < 0 || EEPROM.read(address) > 400){
    EEPROM.write(address, 104);
  }

  setSyncProvider(RTC.get); 
  /*if(timeStatus() != timeSet){
    Serial.println("Unable to sync with the RTC");
  }
  else{
    Serial.println("RTC has set the system time");
  }*/
        
  threshold = (EEPROM.read(address) * 4); //Sound sensor ouptuts value between 0 and 1024, EEPROM holds values between 0 and 255
  
  pinMode(A3, INPUT);
  lcd.begin(16, 2);
  delay(250);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("The threshold is:");
  lcd.setCursor(0, 1);
  lcd.print(threshold);
  delay(1000);
  lcd.clear();
  lcd.noDisplay();
}

void loop(){
    if (analogRead(A3) > threshold){
        level = analogRead(A3);
        openSlot();
        delay(500);
        displayTime();
        closeSlot();
        delay(500);
    }
    if(digitalRead(increase) == false){
        threshold += 4;
        EEPROM.write(address, (threshold / 4));
        delay(300);
    }
    if(digitalRead(decrease) == false){
        threshold -= 4;
        EEPROM.write(address, (threshold / 4));
        delay(300);
    }
}

void displayTime(){
    now();
    
    lcd.clear();
    lcd.display();

    if(timeStatus() != timeSet){
        lcd.setCursor(0, 0);
        lcd.print("Error! Tell Ben!!");
        delay(500);
    }
    else{
        lcd.setCursor(0, 0);
        lcd.print("Hey there!");
        delay(500);
        
        /*lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("The level was:");
        lcd.setCursor(0, 1);
        lcd.print(level);
        delay(2000);*/

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("The time is:");
        lcd.setCursor(0, 1);
        lcd.print(hour());
        lcd.print(":");
        if(minute() < 10){
            lcd.print("0");
        }
        lcd.print(minute());
        delay(3000);
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("The threshold is:");
        lcd.setCursor(0, 1);
        lcd.print(threshold);
        delay(1000);
        
    }
    lcd.noDisplay();
    lcd.clear();
}

void openSlot(){
    slot.setSpeed(250);
    slot.run(FORWARD);
    delay(1000);
    slot.run(RELEASE);
    slotIsOpen = true; 
}

void closeSlot(){
    slot.setSpeed(250);
    slot.run(BACKWARD);
    delay(1050);
    slot.run(RELEASE);
    slotIsOpen = false; 
}

