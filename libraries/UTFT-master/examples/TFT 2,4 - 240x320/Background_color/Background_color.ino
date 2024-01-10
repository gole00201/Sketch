//Цветной графический дисплей 2.4 TFT 240x320     http://iarduino.ru/shop/Displei/cvetnoy-graficheskiy-tft-ekran-240-320---2-4-rdquo.html

#include <UTFT.h>                              // подключаем библиотеку UTFT
UTFT myGLCD(TFT01_24SP,5,4,8,7,6);             // объявляем объект myGLCD класса библиотеки UTFT указывая тип дисплея TFT01_24SP и номера выводов Arduino к которым подключён дисплей: SDI/MOSI, SCK, CS, RESET, DC/RS. Можно использовать любые выводы Arduino.
                                               //
void setup(){                                  //
   myGLCD.InitLCD();                           // инициируем дисплей
   myGLCD.clrScr();                            // стираем всю информацию с дисплея
}                                              //
                                               //
void loop(){                                   //
   myGLCD.fillScr(VGA_RED   ); delay(1000);    // заливаем дисплей красным,     ждём 1 секунду
   myGLCD.fillScr(VGA_GREEN ); delay(1000);    // заливаем дисплей зелёным,     ждём 1 секунду
   myGLCD.fillScr(VGA_BLUE  ); delay(1000);    // заливаем дисплей синим,       ждём 1 секунду
   myGLCD.fillScr(VGA_SILVER); delay(1000);    // заливаем дисплей серебристым, ждём 1 секунду
   myGLCD.fillScr(VGA_MAROON); delay(1000);    // заливаем дисплей бордовым,    ждём 1 секунду
   myGLCD.fillScr(VGA_NAVY  ); delay(1000);    // заливаем дисплей тем. синим,  ждём 1 секунду
}                                              //
