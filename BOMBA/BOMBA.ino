#include "SPI.h"      // библиотека для протокола SPI
#include "nRF24L01.h" // библиотека для nRF24L01+
#include "RF24.h"     // библиотека для радио модуля
#include "GyverTM1637.h"
#include <Keypad.h>
#include <GyverPower.h>
#include "printf.h"
#include <Adafruit_INA219.h> 

#define PIN_GERKON_GAYKA A7
#define PIN_TIMER_START A6
#define PIN_PIEZO A1
//#define PIN_POWER A7
#define CLK A3
#define DIO A2
//#define PASS 1234
#define TTIME 20

#define PIN_POWER_RADIO_AND_DISP A0
//#define PIN_POWER_RADIO A3
//#define PIN_POWER_DISPLAY A4

//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+

struct { char Whom[10]=""; char Command[10]=""; char Parametr[5]=""; } MESSAGE;
struct { char From[5]="Bomba";  float Power=0; char Status[20]="Dinamo_Off"; } BOMBA;
//Dinamo_Off - Ждем пока раскрутят динамо машиину
//Whait_Gayka - Ждем пока вкрутят гайку
//Wait_Code_Enter - Ждем пока ввудет код
//Wait_Timer_Start - Ждем пока запустят отсчет
//Wait_Timer_End - Ждем пока завершится отсчет
//Timer_End - Таймер закончил отсчет

int PASS = 1234;
int TIMER = 20;

int NUM_ENTER = 0; unsigned long TM_BLINK = 0; bool TR=false;
int CODE[4]; 
unsigned long TM = 0;
int TIME = TIMER; 

unsigned long TM_Up = 0;
unsigned long TM_Sleep = 0;
unsigned long Timeout_Sleep = 60000;

const uint8_t num_channels = 126; uint8_t values[num_channels];                                     //Для проверки радио
const int num_reps = 100; bool constCarrierMode = 0;                                                //Для проверки радио


//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
RF24 radio(9,10); // Для MEGA2560 замените на RF24 radio(9,53);

GyverTM1637 disp(CLK, DIO); //7-сегмент дисплей

Adafruit_INA219 ina219;         // Создаем объект ina219

const byte ROWS = 4; const byte COLS = 4; //Матричная клавиатура
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'}, 
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; byte colPins[COLS] = {5, 4, 3, 2}; //Стрроки/столбцы
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
void SM(String Str, bool TR) {
  unsigned long TM_After_Start = millis() / 1000ul; String Time;
  int H = (TM_After_Start / 3600ul); 
  int M = (TM_After_Start % 3600ul)/60ul;  
  int S = (TM_After_Start % 3600ul)%60ul;
  if (H>9) Time+=String(H); else Time+="0"+String(H); Time+=":";
  if (M>9) Time+=String(M); else Time+="0"+String(M); Time+=":";
  if (S>9) Time+=String(S); else Time+="0"+String(S); 
  Serial.print("["+Time+"][--- "+Str+" ---]");
  if (TR) Serial.println();
}
bool ST(bool Flag ) {
  if (Flag) Serial.println(F(" ... Done!")); 
  else Serial.println(F(" ... ERROR!"));
  return Flag;
}
byte CONVERT(int Val) {
  switch (Val) {
    case 0: return byte(0x3f);
    case 1: return byte(0x06);
    case 2: return byte(0x5b);
    case 3: return byte(0x4f);
    case 4: return byte(0x66);
    case 5: return byte(0x6d);
    case 6: return byte(0x7d);
    case 7: return byte(0x07);
    case 8: return byte(0x7f);
    case 9: return byte(0x6f);
  }
}
void ANIM_ON() {
  int a=100, b=100, c=30;
  disp.clear(); delay(200);
  disp.displayByte(0x0, 0x0, 0x1, 0x0); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x0, 0x1, 0x1); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x0, 0x1, 0x3); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x0, 0x1, 0x7); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x0, 0x1, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x0, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x0, 0x08, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x08, 0x08, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x18, 0x08, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x38, 0x08, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x08, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x09, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  
  
  disp.displayByte(0x39, 0x09, 0x08, 0x0F); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x08, 0x0E); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x08, 0x0C); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x08, 0x08); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x08, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x09, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x39, 0x01, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x31, 0x01, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x21, 0x01, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x01, 0x01, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x00, 0x01, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.displayByte(0x00, 0x00, 0x00, 0x00); delay(a); tone(PIN_PIEZO, b, c);
  disp.clear(); tone(PIN_PIEZO, 2000, 30);
}
void ANIM_ENTER_CODE() { 
  if (millis()-TM_BLINK > 500) {
    TM_BLINK = millis();
    if (TR) {
      TR = false;
      switch (NUM_ENTER) {
        case 0: disp.displayByte(0x08, _empty, _empty, _empty); break;
        case 1: disp.displayByte(CONVERT(CODE[0]), 0x08, 0x0, 0x0); break;
        case 2: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), 0x08, 0x0); break;
        case 3: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), CONVERT(CODE[2]), 0x08); break;
        case 4: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), CONVERT(CODE[2]), CONVERT(CODE[3]));  break;
      }
    }
    else if (!TR) {
      TR = true;
      switch (NUM_ENTER) {
        case 0: disp.displayByte(_empty, _empty, _empty, _empty); break;
        case 1: disp.displayByte(CONVERT(CODE[0]), 0x0, 0x0, 0x0); break;
        case 2: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), 0x0, 0x0); break;
        case 3: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), CONVERT(CODE[2]), 0x0); break;
        case 4: disp.displayByte(CONVERT(CODE[0]), CONVERT(CODE[1]), CONVERT(CODE[2]), CONVERT(CODE[3])); break;
      }
    }
  } 
}
void ANIM_CODE_CORRECT() {
  disp.point(POINT_ON,1);
  for (int i=99; i>0; i--){
    disp.displayByte(CONVERT(int(i/10)), CONVERT(i-int(i/10)*10), CONVERT(9), CONVERT(9));
    tone(PIN_PIEZO, 500+random(0, 2000), 1);
    delay(10);
    //RADIO_CHECK();
  }
  for (int i=99; i>TIMER-1; i--){
    disp.displayByte( CONVERT(0), CONVERT(0), CONVERT(int(i/10)), CONVERT(i-int(i/10)*10));
    tone(PIN_PIEZO, 500+random(0, 2000), 1);
    delay(10);
    //RADIO_CHECK();
  }
}
void SET_STATUS(String Stat) {
  Timeout_Sleep=1800000;
  if (Stat=="Dinamo_Off") { 
    disp.clear(); 
    disp.point(POINT_OFF,1);
    strcpy(BOMBA.Status, "Dinamo_Off");
  } else
  if (Stat=="Whait_Gayka") { 
    strcpy(BOMBA.Status, "Whait_Gayka");
    disp.clear(); 
    disp.point(POINT_OFF,1);
  } else
  if (Stat=="Wait_Code_Enter") { 
    strcpy(BOMBA.Status, "Wait_Code_Enter");
    ANIM_ON(); 
    NUM_ENTER=0; 
  } else
  if (Stat=="Wait_Timer_Start") { 
    strcpy(BOMBA.Status, "Wait_Timer_Start");
    ANIM_CODE_CORRECT(); 
  } else
  if (Stat=="Wait_Timer_End") { 
    strcpy(BOMBA.Status, "Wait_Timer_End");
    TM=millis();
    TIME = TIMER;
  }
  if (Stat=="Timer_End") { 
    strcpy(BOMBA.Status, "Timer_End");
    tone(PIN_PIEZO, 2000); delay(1000); noTone(PIN_PIEZO);
    disp.clear();
    disp.point(POINT_OFF,1);
  }

}
void RADIO_CHECK() {
  //Инициализация радиомодуля
  radio.begin(); // включаем радио модуль                                             // Инициируем работу nRF24L01+
  //Serial.println(radio.begin());
  radio.setChannel      (100);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
  radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
  radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
  radio.openReadingPipe (1, 0xAABBCCDD11LL);                 // Открываем 1 трубу с адресом 0xAABBCCDD11, для приема данных.
  radio.startListening  ();                                  // Включаем приемник, начинаем прослушивать открытые трубы.
  radio.writeAckPayload (1, &BOMBA, sizeof(BOMBA) );     // Помещаем данные всего массива ackData в буфер FIFO. Как только будут получены любые данные от передатчика на 1 трубе, то данные из буфера FIFO будут отправлены этому передатчику вместе с пакетом подтверждения приёма его данных.
  
  delay(100);
  //Serial.println(123);
  if(radio.available()){ 
    //Serial.println(123);                                    
    radio.read(&MESSAGE, sizeof(MESSAGE)); 
    if (String(MESSAGE.Whom) == "To_Bomba"){
      Serial.print(MESSAGE.Command); Serial.print(" "); Serial.println(PASS);
      if (String(MESSAGE.Command) == "Get_Info") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); }//SET_STATUS("Whait_Gayka"); }  
      if (String(MESSAGE.Command) == "Set_Stat") {
        if (String(MESSAGE.Parametr) == "WU") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); Timeout_Sleep=1800000; }
        if (String(MESSAGE.Parametr) == "DO") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Dinamo_Off"); }
        if (String(MESSAGE.Parametr) == "WG") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Whait_Gayka"); }
        if (String(MESSAGE.Parametr) == "WCE"){ radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Wait_Code_Enter"); }
        if (String(MESSAGE.Parametr) == "WTS"){ radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Wait_Timer_Start"); }
        if (String(MESSAGE.Parametr) == "WTE"){ radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Wait_Timer_End"); }
        if (String(MESSAGE.Parametr) == "TE") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); SET_STATUS("Timer_End"); }
      } 
      if (String(MESSAGE.Command) == "Set_Pass") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); PASS=atoi(MESSAGE.Parametr);  }
      if (String(MESSAGE.Command) == "Set_Timer") { radio.writeAckPayload(1, &BOMBA, sizeof(BOMBA)); TIMER=atoi(MESSAGE.Parametr);  }
      
    }
  } 
}

//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup() {
  Serial.begin(57600);
  
  printf_begin();

  /*
  //Инициализация радиомодуля
  SM(F("Check Radio module..."), 0); while(ST(radio.begin())==0) {} // включаем радио модуль                                             // Инициируем работу nRF24L01+
  radio.setChannel      (100);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
  radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
  radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
  radio.openReadingPipe (1, 0xAABBCCDD11LL);                 // Открываем 1 трубу с адресом 0xAABBCCDD11, для приема данных.
  radio.startListening  ();                                  // Включаем приемник, начинаем прослушивать открытые трубы.
  radio.writeAckPayload (1, &BOMBA, sizeof(BOMBA) );     // Помещаем данные всего массива ackData в буфер FIFO. Как только будут получены любые данные от передатчика на 1 трубе, то данные из буфера FIFO будут отправлены этому передатчику вместе с пакетом подтверждения приёма его данных.
  */

  //Инициализация 7ми сегментного дисплея
  disp.clear(); disp.brightness(3);  // яркость, 0 - 7 (минимум - максимум)
  
  //Инициализация Пинов
  //pinMode(PIN_GERKON_GAYKA, INPUT);
  //pinMode(PIN_TIMER_START, INPUT);

  pinMode(PIN_POWER_RADIO_AND_DISP, OUTPUT); digitalWrite(PIN_POWER_RADIO_AND_DISP, HIGH);
  //pinMode(PIN_POWER_RADIO, OUTPUT); digitalWrite(PIN_POWER_RADIO, HIGH);
  //pinMode(PIN_POWER_DISPLAY, OUTPUT);

  //Инициализация контроля заряда аккумулятора
  //analogReference(INTERNAL); // установка опорного напряжения для аналоговых входов
  //analogReference(EXTERNAL);
  //power.setSystemPrescaler(PRESCALER_2);


  power.autoCalibrate();

}


//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop() {
  //Замер уровня заряда батареи  
  //analogReference(EXTERNAL);
  //BOMBA.Power = analogRead(PIN_POWER);
  //BOMBA.Power = (analogRead(PIN_POWER)*(1.1/1023))*6.35;
  //BOMBA.Power = analogRead(PIN_POWER)*(1.5/1024)/0.168;
  //Serial.println(analogRead(PIN_POWER)*(1.5/1024)/0.168);
  //BOMBA.Power = analogRead(PIN_POWER)*(1.1/1023)/0.182;
  //analogReference(DEFAULT);
  //Serial.println("Anal: "+String(analogRead(PIN_POWER))+"   Anal_V:"+String(analogRead(PIN_POWER)*(1.1/1023)));
  //BOMBA.Power = ((analogRead(PIN_POWER)*1.1)/1023 / 0.091 / 2 ); // 0.091 = R2/(R1+R2)  0.99кОм / (9.88кОм + 0.99кОм)
  
  ina219.begin(); ina219.setCalibration_16V_400mA();
  BOMBA.Power = ina219.getBusVoltage_V();
  //Serial.print(BOMBA.Power);
  //BOMBA.Power = ((analogRead(PIN_POWER)*1.1)/1023 * 11 ); // 0.091 = R2/(R1+R2)  0.99кОм / (9.88кОм + 0.99кОм)
  //Принимаем сообщение от сервера
  RADIO_CHECK();


  

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //Основной цикл программы
  if (String(BOMBA.Status)!="Whait_Gayka" and String(BOMBA.Status)!="Timer_End" and String(BOMBA.Status)!="Dinamo_Off" and digitalRead(PIN_GERKON_GAYKA)) SET_STATUS("Whait_Gayka");

  //Whait_Gayka - Ждем пока вкрутят гайку
  if (String(BOMBA.Status)=="Whait_Gayka" and !bool(analogRead(PIN_GERKON_GAYKA))) {
    SET_STATUS("Wait_Code_Enter"); 
  } 
  //Wait_Code_Enter - Ждем пока ввудет код
  if (String(BOMBA.Status)=="Wait_Code_Enter") {
    //if (digitalRead(PIN_GERKON_GAYKA)) { SET_STATUS("Whait_Gayka"); return; }
    ANIM_ENTER_CODE();
    char customKey = customKeypad.getKey();//записывем нажатый символ
    if (customKey){//если что-то нажато
      tone(PIN_PIEZO, 500+random(0, 500), 50);
      int Num = customKey-'0';
      //Serial.println(Num);
      NUM_ENTER++;
      TM_BLINK = 0;
      switch (NUM_ENTER-1) {
        case 0: CODE[0]=int(Num); break;
        case 1: CODE[1]=int(Num); break;
        case 2: CODE[2]=int(Num); break;
        case 3: CODE[3]=int(Num); break;
      }
      if (NUM_ENTER>3) {
        if (PASS != int(CODE[0]*1000+CODE[1]*100+CODE[2]*10+CODE[3]) ) {
          ANIM_ENTER_CODE(); 
          tone(PIN_PIEZO, 10); delay(1000); noTone(PIN_PIEZO); 
          NUM_ENTER=0;
        }
        if (PASS == int(CODE[0]*1000+CODE[1]*100+CODE[2]*10+CODE[3]) ) {
          tone(PIN_PIEZO, 500); delay(100); noTone(PIN_PIEZO);
          tone(PIN_PIEZO, 1000); delay(100); noTone(PIN_PIEZO);
          //ANIM_CODE_CORRECT();
          SET_STATUS("Wait_Timer_Start");
        }
      }
    }
  } 
  //Wait_Timer_Start - Ждем пока запустят отсчет
  if (String(BOMBA.Status)=="Wait_Timer_Start" and !bool(analogRead(PIN_TIMER_START))) {
    SET_STATUS("Wait_Timer_End"); 
  } 
  //Wait_Timer_End - Ждем пока завершится отсчет
  if (String(BOMBA.Status)=="Wait_Timer_End") {
    if (millis()-500 > TM) disp.point(POINT_ON,1);
    if (millis()-1000 > TM) {
      TM=millis();
      tone(PIN_PIEZO, 2000, 100);
      TIME--;
      disp.displayByte(_0, _0, CONVERT(int(TIME/10)), CONVERT(TIME-int(TIME/10)*10));
      disp.point(POINT_OFF,1);
    }
    if (TIME==0) SET_STATUS("Timer_End");
  }

  //or (millis()-TM_Sleep > 30*60*1000)) 
  //Спящий режим
  //if (millis()-TM_Up > Timeout_Sleep and (String(BOMBA.Status)=="Dinamo_Off" or String(BOMBA.Status)=="Timer_End") ) {
  if (millis()-TM_Up > Timeout_Sleep) {
    TM_Up=millis(); Timeout_Sleep=60000;
    digitalWrite(PIN_POWER_RADIO_AND_DISP, LOW);
    //digitalWrite(PIN_POWER_DISPLAY, HIGH);
    power.sleepDelay(55000);
    digitalWrite(PIN_POWER_RADIO_AND_DISP, HIGH);
    //digitalWrite(PIN_POWER_DISPLAY, LOW);
  }

  /*
  if (String(BOMBA.Status)=="Dinamo_Off" or String(BOMBA.Status)=="Timer_End" or (millis()-30*60*1000 > TM_Sleep)) {
    power.sleepDelay(10*1000);
  }
  */

}
