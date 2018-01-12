#include "Arduino.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include "AlarmLib.h"

#define PIR_PIN D2  // Пир датчик

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F,16,2); // 1-Выяснить адрес дисплея

/* Плата D1
 * SCL - дисплея и часов
 * SDA - дисплея и часов - подкючаются параллельно
 * Bluetooth 
 * Tx->Rx D1
 * Rx->Tx D0
*/

boolean disp_light = true;
int lSec = 0;
unsigned long time;

int alarms[] = {    
  11*60,        
  13*60,    
  18*60,
  19*60,
  20*60,
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);//Off for D1
  pinMode(PIR_PIN, INPUT);

  Serial.begin(9600);
  delay(100);

//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    while (1);
//  }
  
  Serial.print("Today: ");
  Serial.println(Date2StrFull(rtc.now()));
  Serial.println();

  lcd.init();                     
  lcd.backlight();

    
// rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Установить часы как Дата Компиляции Скетча
  // January 21, 2014 at 3alarms you would call:
  // rtc.adjust(DateTime(2017, 1, 31, 14, 17, 0));  
}

boolean FindNextAlarm(DateTime date, Alarm& alarm)
{
  int nnn = date.minute()+date.hour()*60;
  boolean a = false;
  
  int i;
  for (i=0; i < (sizeof(alarms)/sizeof(int)); i++){
    if (alarms[i]>nnn)
    {
      a=true;
      break;
    }
  }
    
  alarm = Alarm(alarms[i]);
  return a;
}

boolean _colon = true;

void loop() {
 
  DateTime n = rtc.now();
  Alarm alarm;
 
  if (Serial.available())
  {
    alarm = Alarm(Serial.readString()); 
    alarms[0] = alarm.minutes();
    
    Serial.println("Alarm on: "+alarm.ToStr());    
    Serial.println("Now: "+Date2StrFull(n));
  }
 
 if (lSec!=n.second())
 {
    lcd.setCursor(0, 0);
    lcd.print(Time2StrHHMM(n,_colon));
    _colon=!_colon;

  if (FindNextAlarm(n,alarm))  
  {
    lcd.print(" "+alarm.ToStr());    

   int delta = alarm.minutes()-Date_minutes(n);
    
    if (delta>=600)
    {
      int w = delta/60;
      lcd.print(" "+(String)w+"h ");//+    
    }
    else
    {
      String w = Min2hMM(delta);
      lcd.print(" "+(w.length()<=5?w:"    "));    
    }   
  }  
  
  lcd.setCursor(0, 1);
  lcd.print(Date2StrWeek(n));

  lSec=n.second();
 }
 else delay(300);   
 
if (digitalRead(PIR_PIN)==1) time = millis();  
if (disp_light= millis()-time<20000) lcd.backlight(); else lcd.noBacklight();  
}
