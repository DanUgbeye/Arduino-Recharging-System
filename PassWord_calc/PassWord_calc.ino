#include <RTClib.h>
#include <CountDown.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EEPROM.h"

LiquidCrystal_I2C lcd(0x20,20,4);
CountDown CD;
RTC_PCF8523 rtc;

int tries = 0;
int next_UnusedAddress;
int gatePin = 12;
DateTime RESET(2010, 1, 1, 12, 0, 0);

//change number below to the total number of passwords you are using
const int no_of_Pw = 3;

//change the number below to the LENGTH of each password
int pwLength = 3;

//space taken to write a date to EEPROM, DO NOT CHANGE!
int space_for_date = 9;

char p1[]= "111";
char p2[] = "222";
char p3[] = "333";

char CLEARED[no_of_Pw];

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};   //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// Prototyping the functions
bool checkPw(String input);
void menu();
void loading();
void clearRow(int row);
String getPassword();
void battery();
void writeData(char password[]);
String readData(int currentPos);
void clearData(int currentPos);
void timeLeft(CountDown cd);
void resetDate();
void writeDate(DateTime date);
DateTime readDate();
bool checkDateSaved();
void showDate(const DateTime& dt, int row);

//======================================SETUP AND LOOP FUNCTIONS=================================

void setup() {
  // put your setup code here, to run once:
  next_UnusedAddress = space_for_date + (no_of_Pw * pwLength);

  EEPROM.begin();

  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("\t**** WELCOME ****");
  delay(1000);

  pinMode(gatePin, OUTPUT);

  for(int i = 0; i < pwLength; i++){
      CLEARED[i] = '0';
  }

  if (! rtc.begin()) {
    lcd.setCursor(0,2);
    lcd.print(" Couldn't find RTC");
    delay(500);
    clearRow(2);
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    lcd.setCursor(0,2);
    lcd.print("RTC NOT initialized");
    lcd.setCursor(0,3);
    lcd.print("   Setting time!");
    delay(500);
    clearRow(2);
    clearRow(3);
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }
  
  rtc.start();

  String test = (readData(space_for_date));

  if(test != p1 && test != CLEARED){

    next_UnusedAddress = space_for_date;
    resetDate();

    //use the writedata function to save all passwords to the EEPROM
    writeData(p1);
    writeData(p2);
    writeData(p3);

    lcd.setCursor(0,2);
    lcd.print("   INITIALIZING");
    delay(500);
    clearRow(2);
  }

  loading();
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(gatePin, LOW);
  menu();
}

//======================================USER-DEFINED FUNCTIONS=================================

//--------------------------------------CHECK-DATE-SAVED FUNCTION------------------------------
bool checkDateSaved(){

  DateTime date = readDate();

  if(date == RESET){
    return false;
  }else if(date.year() == 2010 && date.month() == 1
           && date.day() == 1 && date.hour() == 12
            && date.minute() == 0 && date.second() == 0){
    return false;
  }else{
    return true;
  }
}

//---------------------------------------RESET-DATE FUNCTION-------------------------------------
void resetDate(){

  writeDate(RESET);

  if(readDate() == RESET){
    lcd.setCursor(19,3);
    lcd.print("*");
    delay(350);
    lcd.setCursor(19,3);
    lcd.print(" ");
  }else{
    lcd.setCursor(19,3);
    lcd.print("e");
    delay(350);
    lcd.setCursor(19,3);
    lcd.print(" ");
  }
}

//-------------------------------------WRITE-DATE FUNCTION--------------------------------------
void writeDate(DateTime date){

  String year = String(date.year());

  //the lines of code below splits the year into four values so each can be saved to the EEPROM
  byte yr1 = byte(String(year[0]).toInt());
  byte yr2 = byte(String(year[1]).toInt());
  byte yr3 = byte(String(year[2]).toInt());
  byte yr4 = byte(String(year[3]).toInt());
  byte month = date.month();
  byte day = date.day();
  byte hour = date.hour();
  byte minute = date.minute();
  byte second = date.second();

  for (int i = 0; i < space_for_date; i++)
  {
    if (i == 0){
      EEPROM.update(i, yr1);
    }
    if (i == 1){
      EEPROM.update(i, yr2);
    }
    if (i == 2){
      EEPROM.update(i, yr3);
    }
    if (i == 3){
      EEPROM.update(i, yr4);
    }
    if (i == 4){
      EEPROM.update(i, month);
    }
    if (i == 5){
      EEPROM.update(i, day);
    }
    if (i == 6){
      EEPROM.update(i, hour);
    }
    if (i == 7){
      EEPROM.update(i, minute);
    }
    if (i == 8){
      EEPROM.update(i, second);
    }
  }
}

//-------------------------------------READ-DATE FUNCTION--------------------------------------
DateTime readDate(){
  String yr;
  String yr1;
  String yr2;
  String yr3;
  String yr4;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;

  for (int i = 0; i < space_for_date; i++){
    if (i == 0){
      yr1 = String(EEPROM.read(i));
    }
    if (i == 1){
      yr2 = String(EEPROM.read(i));
    }
    if (i == 2){
      yr3 = String(EEPROM.read(i));
    }
    if (i == 3){
      yr4 = String(EEPROM.read(i));
    }
    if (i == 4){
      month = EEPROM.read(i);
    }
    if (i == 5){
      day = EEPROM.read(i);
    }
    if (i == 6){
      hour = EEPROM.read(i);
    }
    if (i == 7){
      minute = EEPROM.read(i);
    }
    if (i == 8){
      second = EEPROM.read(i);
    }
  }

  //appending the full year and converting it to an integer
  yr = String(yr1) + String(yr2) + String(yr3) + String(yr4);
  int year = yr.toInt();

  DateTime date(year, month, day, hour, minute, second);

  return date;
}

//--------------------------------------CLEAR-ROW FUNCTION--------------------------------------
void clearRow(int row){
  for(int i = 19; i > -1; i--){
    lcd.setCursor(i , row);
    lcd.print(" ");
  }
}

//--------------------------------------GET-PASSWORD FUNCTION-------------------------
String getPassword(){

  String pwd = "";
  clearRow(1);
  while(true){
    battery();
    char key = keypad.getKey();
    delay(10);

    if (key) {
      if (key >= '0' && key <= '9') {     // only act on numeric keys
        if(pwd.length() < 10){
          pwd += key;                     // append new character to input string
          lcd.setCursor(1,1);
          lcd.print(pwd);
        }else{
          lcd.setCursor(17,1);
          lcd.print("max");
          delay(200);
          lcd.setCursor(17,1);
          lcd.print("   ");
        }
        key = '\0';
      } else if (key == '*') {
        if(pwd != ""){
          lcd.setCursor(0,1);
          lcd.print("      deleted      ");
          delay(150);
          clearRow(1);
          pwd = "";                       // clear input
        }
        key = '\0';
      }else if (key == '#'){
        if(pwd.length() > 0){
          clearRow(1);
          break;
        }else{
          lcd.setCursor(0,1);
          lcd.print("    enter a pin!");
          delay(250);
          clearRow(1);
        }

        key = '\0';
      }
    }
  }
  return pwd;
}

//--------------------------------------BATTERY FUNCTION---------------------------
void battery(){
  int a= 1;
  int b=12;
  int c=100;
  int bat = analogRead(A0);//assigning the analog input pin to A0
  float voltage = bat * (12.0/1023);
  int pent = voltage*a/b*c;//calculating battery pencentage
  lcd.setCursor(0,3);
  lcd.print ("batt: ");
  lcd.print (pent);
  lcd.print ("%  ");
}

//--------------------------------------CHECKPW FUNCTION-------------------------
bool checkPw(String input){

    int currentPos = space_for_date;
    String pw;

    if(String(input) == String(CLEARED)){
        return false;
    }else{
        for(int i = 0; i < no_of_Pw; i++){

             pw = readData(currentPos);

             if(pw == String(CLEARED)){
                currentPos += pwLength;
                continue;
             }

            if(String(input) == pw){
                clearData(currentPos);
                currentPos = 0;
                return true;
            }
            currentPos += pwLength;
        }
        return false;
    }
}

//--------------------------------------MENU FUNCTION-------------------------
void menu(){

  bool saved = checkDateSaved();

  if(saved == true ){
    timeLeft(CD);
  }else{
    String input = "\0";
    lcd.clear();
    battery();
    lcd.setCursor(0,0);
    lcd.print(" ENTER CARD TO USE");
    lcd.setCursor(0,2);
    lcd.print("*: delete   #: enter");

    //Gets keypad input from the user
    input = getPassword();

    bool match = checkPw(input);
    clearRow(0);
    clearRow(1);
    clearRow(2);
    lcd.setCursor(0,1);
    lcd.print("   CHECKING CARD ");
    delay(500);

    if(match == true){
      clearRow(0);
      clearRow(1);
      clearRow(2);
      lcd.setCursor(0,1);
      lcd.print("      SUCCESS!");
      delay(500);
      input = "";
      tries = 0;

      //timer function comes here
      timeLeft(CD);

    }else if(match == false){
      clearRow(1);
      clearRow(2);
      lcd.setCursor(0,1);
      lcd.print("   INVALID CARD!");
      digitalWrite(gatePin, LOW);
      input = "";
      delay(500);
      ++tries;

      if(tries > 4){
        clearRow(0);
        clearRow(1);
        clearRow(2);
        lcd.setCursor(0,0);
        lcd.print("   SYSTEM LOCKED!!");
        delay(250);
        lcd.setCursor(0, 1);
        lcd.print("     WAIT!");

        //code to lock for 5 seconds
        for (int i = 5; i > 0; i--)
        {
          lcd.setCursor(12, 1);
          lcd.print(i);
          lcd.print("s");
          delay(1000);
        }

        tries = 0;
      }

      if(tries == 3){
        clearRow(2);
        lcd.setCursor(0,2);
        lcd.print("  2 entries left!!");
        delay(500);
      }

      if(tries == 4){
        clearRow(2);
        lcd.setCursor(0,2);
        lcd.print("   1 entry left!!");
        delay(500);
      }
    }
  }
}

//---------------------------------------LOADING FUNCTION---------------------------
void loading(){
    lcd.setCursor(0,2);
    lcd.print("\t\t   LOADING");
    for(int i = 0; i < 3; i++){
      lcd.setCursor(12,2);
      lcd.print("   ");
      lcd.setCursor(12,2);
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(250);
    }
}

//--------------------------------------WRITE-DATA FUNCTION-------------------------
void writeData(char pw[]){
    byte value;
    for(int i = 0; i < pwLength; i++){
        value = byte(pw[i]);
        EEPROM.write(next_UnusedAddress + i, value);
    }
    next_UnusedAddress += pwLength;
}

//---------------------------------------CLEAR-DATA FUNCTION-------------------------
void clearData(int currentPos){
    for(int i = currentPos; i < currentPos + pwLength; i++){
        //checking if max used address is exceeded
        if(i >= next_UnusedAddress){
            lcd.setCursor(0,3);
            lcd.print("ERROR READING DATA!");
            delay(500);
            clearRow(3);
            battery();
            return ;
        }
        EEPROM.update(i, 0);
    }
}

//---------------------------------------READ-DATA FUNCTION-------------------------
String readData(int currentPos){
    String pw = ""  ;
    for(int i = currentPos; i < currentPos + pwLength; i++){
        //checking if max used address is exceeded
        if(i >= next_UnusedAddress && next_UnusedAddress < space_for_date){
            lcd.setCursor(0,3);
            lcd.print("ERROR READING DATA!");
            delay(500);
            clearRow(3);
            battery();
            return "";
        }
        pw += char(EEPROM.read(i));
    }
    return pw;
}

//---------------------------------------TIME-LEFT FUNCTION------------------------------
void timeLeft(CountDown cd){

  clearRow(0);
  clearRow(1);
  clearRow(2);

  DateTime now;
  DateTime future;
  TimeSpan left;

//checking if a date is saved on EEPROM
  bool saved = checkDateSaved();
  if ( saved == true){
    future = readDate();
    now = rtc.now();

    //checking if the saved future date is past
    if(now > future){
      return ;
    }

    left = future - now;
  }else{
    now = rtc.now();
    future = now + TimeSpan(30, 0, 0, 0);
    writeDate(future);
    left = future - now;
  }

  lcd.setCursor(0, 0);
  lcd.print("     TIME LEFT");

  cd.start((left.days()), (left.hours()), (left.minutes()), (left.seconds()));
  if (cd.remaining() > 0){
    digitalWrite(gatePin, HIGH);
  }

  unsigned long timeleft;

  while(cd.remaining() > 0){

    if (timeleft != cd.remaining() || cd.remaining() == 0 ){

      timeleft = cd.remaining();

      // Calculating the days, hours, minutes and seconds left
      int days = int(timeleft / (60UL * 60UL * 24UL));

      int hours = int((timeleft % (60UL * 60UL * 24UL)) / (60 * 60));

      int minutes = int((timeleft % (60 * 60)) /  60);

      int seconds = int((timeleft % 60) / 1);

      // Outputing the Time left
      if((days < 10 && days > 0) || days == 0){
        lcd.setCursor(0, 1);
        lcd.print("\t 0");
        lcd.print(days);
        lcd.print("d ");
      } else{
        lcd.setCursor(0, 1);
        lcd.print("\t ");
        lcd.print(days);
        lcd.print("d ");
      }

      if((hours < 10 && hours > 0) || hours == 0 ){
        lcd.print("0");
        lcd.print(hours);
        lcd.print("h ");
      } else{
        lcd.print(hours);
        lcd.print("h ");
      }

      if((minutes < 10 && minutes > 0) || minutes == 0 ){
        lcd.print("0");
        lcd.print(minutes);
        lcd.print("m ");
      } else{
        lcd.print(minutes);
        lcd.print("m ");
      }

      if((seconds < 10 && seconds > 0) || seconds == 0 ){
        lcd.print("0");
        lcd.print(seconds);
        lcd.print("s   ");
      } else{
        lcd.print(seconds);
        lcd.print("s   ");
      }

      battery();
      delay(100);
    }
  }

  resetDate();
  clearRow(0);
  clearRow(2);
  clearRow(1);
  lcd.setCursor(0,0);
  lcd.print("   TIME EXPIRED!");
  digitalWrite(gatePin, LOW);
  delay(1000);
}
