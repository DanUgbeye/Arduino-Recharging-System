#include <EEPROM.h>
#include <RTClib.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x20,20,4);

void setup(){

lcd.setCursor(0,0);
lcd.print("wel");
}
void loop(){
  
  
  lcd.setCursor(0,0);
lcd.print("Enter Pin");
delay(100);

/*pinMode (gatePin,OUTPUT);
                  double e= micros();
                  double p;
                  int M;
                  int s;
                  int secs;
                  double tens_seconds;
                 

customKey = customKeypad.getKey();
if (customKey){
  Data[data_count] = customKey;
  lcd.setCursor(data_count,1);
  lcd.print(Data[data_count]);
  data_count++;
}

if(data_count == 5){
     lcd.clear();
    
     if(strcmp(Data,p1)||strcmp(Data,up1)){//code to check if the password is correct
      lcd.setCursor(0,1);
      lcd.print("invalid");
      delay(100);
                  secs = (e-p)/1000000;
                 M =e/6000000;
                  lcd.setCursor (10,0);
                  lcd.print(M);
                      if(M==10){
                  lcd. setCursor (0,2);
                  lcd.print("time up");
                  e = 0;
                  }
                  delay(10000);
     }
     /* else {
        lcd.setCursor(0,1);
        lcd.print("timer begins");
        delay(100);
      }
      if(!strcmp(Data,p2)&&!strcmp(Data,up2)){
        lcd.setCursor(0,1);
        lcd.print("already used");
        delay(100);
      }
     
     
     lcd.clear();
     clearData();
}*/
} 
