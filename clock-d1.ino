#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x3F,16,2); // 1-Выяснить адрес дисплея
RTC_DS1307 rtc;

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

  Serial.begin(57600);
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

String norm(int v)
{
  String work = (String)v;
  while (work.length()<2) work ="0"+work;
  return work;
}

String Today(DateTime n)
{
  return (String)n.year()+"."+(String)n.month()+"."+(String)n.day(); 
}

String Time(DateTime n)
{  
  return norm(n.hour())+":"+norm(n.minute())+":"+norm(n.second());
}

String DayOfWeek(DateTime n)
{
  char DOW[][4]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
  return DOW[n.dayOfTheWeek()];
}

volatile int lSec = 0;
unsigned long time;


void showTimeSpan(const char* txt, const TimeSpan& ts) {
    Serial.print(txt);
    Serial.print(" ");
    Serial.print(ts.days(), DEC);
    Serial.print(" days ");
    Serial.print(ts.hours(), DEC);
    Serial.print(" hours ");
    Serial.print(ts.minutes(), DEC);
    Serial.print(" minutes ");
    Serial.print(ts.seconds(), DEC);
    Serial.print(" seconds (");
    Serial.print(ts.totalseconds(), DEC);
    Serial.print(" total seconds)");
    Serial.println();
}

 
void loop() {
  
 DateTime n = rtc.now();
 if (lSec!=n.second())
 {
  
  lcd.setCursor(0, 1);
  lcd.print(Today(n)+" "+DayOfWeek(n));

  lcd.setCursor(0, 0);
  lcd.print(Time(n));

  int am[4] = {12*60,17*60+58,18*60, 21*60};
  
  int nnn = n.minute()+n.hour()*60;
  boolean a=false;

  int i;
  for (i=0; i < 4; i++){
    if (am[i]>=nnn)
    {
      a=true;
      break;
    }
  }
 
  String l= "     ";

  if (a)
  {
    int m = am[i]%60;
    DateTime b (n.year(),n.month(),n.day(),(int)((am[i]-m)/60),m,0);
    TimeSpan t =  b-n;
    
    if ((t.hours()>=0)&&(t.minutes()>=0))
    {
      l = (String)t.hours()+":"+norm(t.minutes());
    }
    
    if (t.hours()>9) l=" "+l;
  }
  
  lcd.setCursor(11, 0); 
  lcd.print(l);

  lSec=n.second();
 }
 
  if (digitalRead(PIR_PIN)==1) time = millis();  
 
  CheckTheLigt(disp_light= millis()-time<20000); 
 
  delay(300);
}
