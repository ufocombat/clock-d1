#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include "RTClib.h"
#include "AlarmLib.h"


RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F,16,2); // 1-Выяснить адрес дисплея

#define PIR_PIN D2  // Пир датчик

/* Плата D1
 * SCL - дисплея и часов
 * SDA - дисплея и часов - подкючаются параллельно
*/

volatile boolean disp_light = true;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);//Off for D1

  pinMode(PIR_PIN, INPUT);

  Serial.begin(9600);
//  Serial.println("Clock...");

  lcd.init();                     
  lcd.backlight();

//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    while (1);
//  }
    
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Установить часы как Дата Компиляции Скетча
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2017, 1, 31, 14, 17, 0));  
}

void CheckTheLigt(boolean f)
{
   if (f) lcd.backlight(); else lcd.noBacklight();
}

volatile int lSec = 0;
unsigned long time;

int val;

int am[] = {    
  11*60+15,        
  13*60,    
  18*60,
  21*60
};
 
void loop() {
 
 DateTime n = rtc.now();
 
  if (Serial.available())
  {
    val = Serial.parseInt();      
    Alarm a = Alarm(val);
    
    am[0] = val;
    Serial.print("Alarm on: ");
    Serial.print(a.ToStr());        
    
    Serial.print(" (");        
    Serial.print(val);
    Serial.println(")");    
    
    Serial.println("Now: ");
    Serial.println(Date2Str(n)+" "+DayOfWeek(n)+" "+Time2Str(n));
    Serial.println();
  }

  

 
 if (lSec!=n.second())
 {
  lcd.setCursor(0, 0);
  lcd.print(Time2Str(n));

//  lcd.setCursor(0, 1);
//  lcd.print(Today(n)+" "+DayOfWeek(n));

  
  int nnn = n.minute()+n.hour()*60;
  boolean a=false;

  int i;
  for (i=0; i < (sizeof(am)/sizeof(int)); i++){
    if (am[i]>=nnn)
    {
      a=true;
      break;
    }
  }
 
  String l= "     ";

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
      l = (String)t.hours()+":"+Norm2Str(t.minutes());
    }
    
    if (t.hours()>9) l=" "+l;
  }
  
  lcd.setCursor(16-l.length(), 0); 
  lcd.print(l);

  lcd.setCursor(0, 1);
  lcd.print((String)h+":"+Norm2Str(r));


  lSec=n.second();
 }
else
{
  delay(300);
} 
 
  if (digitalRead(PIR_PIN)==1) time = millis();  
 
  CheckTheLigt(disp_light= millis()-time<20000);    
}
