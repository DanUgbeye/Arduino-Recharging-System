#include <LiquidCrystal_I2C.h>

//
//    FILE: demo_DHMS.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo
//    DATE: 2017-07-16
//     URL: http://forum.arduino.cc/index.php?topic=356253
//          https://github.com/RobTillaart/CountDown
//

#include "CountDown.h"

CountDown CD;
LiquidCrystal_I2C lcd(0x20,20,4);

void setup()
{
    Serial.begin(115200);
    Serial.println(__FILE__);
    Serial.print("COUNTDOWN_LIB_VERSION: ");
    Serial.println(COUNTDOWN_LIB_VERSION);
    
    float no = 3.999999;
    int aprx = int(no);

    lcd.init();
    lcd.setBacklight(HIGH);

    lcd.setCursor(0,0);
    lcd.print(aprx);
    delay(500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("\t\t\tWELCOME!!");
    delay(200);

    // countdown 1 minute
    CD.start(1, 0, 1, 0);
}

void loop()
{
  static uint32_t last_remaining = 0;
  if (last_remaining != CD.remaining() || CD.remaining() == 0 )
  {
    Serial.println();
    lcd.setCursor(0,0);
    lcd.print(" ");
    last_remaining = CD.remaining();
  }
  Serial.print('\t');
  Serial.print(CD.remaining());
  lcd.setCursor(0,1);
  lcd.print(CD.remaining());
  delay(250);
}
