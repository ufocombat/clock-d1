#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
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
boolean colon=true;

int am[] = {    
  11*60,        
  13*60,    
  18*60  
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);//Off for D1
  pinMode(PIR_PIN, INPUT);

  Serial.begin(9600);

//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    while (1);
//  }
  
  Serial.print("Today: ");
  Serial.println(Date2StrFull(rtc.now()));
  Serial.println();

  lcd.init();                     
  lcd.backlight();

    
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Установить часы как Дата Компиляции Скетча
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2017, 1, 31, 14, 17, 0));  
}

void CheckTheLigt(boolean f)
{
   if (f) lcd.backlight(); else lcd.noBacklight();
}

void loop() {
 
 DateTime n = rtc.now();
 
  if (Serial.available())
  {
    Alarm alarm = Alarm(Serial.readString()); 
    am[0] = alarm.minutes();
    
    Serial.print("Alarm on: ");
    Serial.println(alarm.ToStr());        
    
    Serial.print("Now: ");
    Serial.println(Date2StrFull(n));
    Serial.println();
  }
 
 if (lSec!=n.second())
 {
  lcd.setCursor(0, 0);
  lcd.print(Time2StrDDMM(n,colon));
  colon = !colon;

  int nnn = n.minute()+n.hour()*60;
  boolean a=false;

  int i;
  for (i=0; i < (sizeof(am)/sizeof(int)); i++){
    if (am[i]>nnn)
    {
      a=true;
      break;
    }
  }
 
  String toTime= "     ";

  int r=0;
  int h=0;

  if (a)  
  {
    int m = am[i]%60;
    DateTime b (n.year(),n.month(),n.day(),(int)((am[i]-m)/60),m,0);
    
    r = b.minute();
    h= b.hour();
    
    TimeSpan t =  b-n;
    
    if ((t.hours()>=0)&&(t.minutes()>=0))
    {
      String w = " "+(String)t.hours()+":"+Norm2Str(t.minutes());
      if (w.length()<=5) toTime=w;
    }   
  }
  
  lcd.print(" "+(String)h+":"+Norm2Str(r)+toTime);
  
  lcd.setCursor(0, 1);
  lcd.print(Date2Str(n)+" "+DayOfWeek(n));

  lSec=n.second();
 }
  else
  {
    delay(300);
  } 
 
  if (digitalRead(PIR_PIN)==1) time = millis();  
  CheckTheLigt(disp_light= millis()-time<20000);    
}
