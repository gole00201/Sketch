//=========================================================================+
//                               MEGA 123                                  |
//-------------------------------------------------------------------------+
#include <SoftwareSerial.h>
//#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TiCoServo.h>
#include "GyverButton.h"
#include <OneWire.h>
#include "SPI.h"             
#include "nRF24L01.h"        
#include "RF24.h"            
//###############################
//1111111111111111111111111111111
//###############################
#define PIN_GERKON_1_DOOR A0
#define PIN_DOMOFON_1_DOOR 22

#define PIN_DATCHIK_DVIZH A1         

#define PIN_DOMOFON_GRID 23
#define PIN_GERKON_GRID_OPEN A2
#define PIN_GERKON_GRID_CLOSE A3
#define PIN_STARBOSKOP 24

#define PIN_GENERATOR A4 

#define PIN_FAKELS 2

#define PIN_STOP_GAME_BUTTON A5

#define PIN_SMOKE_HOT 25           
#define PIN_SMOKE_START 26            
//###############################
//2222222222222222222222222222222
//###############################
#define PIN_TILE_1 A6                  
#define PIN_TILE_2 A7                  
#define PIN_TILE_3 A8                  
#define PIN_TILE_4 A9                  
#define PIN_TILE_5 A10                 
#define PIN_TILE_6 A11                 

#define PIN_GERKON_DOOR_TOMB_OPEN A12 
#define PIN_GERKON_DOOR_TOMB_CLOSE A13  
#define PIN_MOTOR_DOOR_TOMB_OPEN 27   
#define PIN_MOTOR_DOOR_TOMB_CLOSE 28  
#define PIN_ZASHITA A14  
//###############################
//3333333333333333333333333333333
//###############################
#define PIN_GERKON_BURNT_FAKEL 29

#define PIN_GERKON_ARCHEOLOGST 30
#define PIN_DOMOFON_ARCHEOLOGST 31

#define PIN_KANOP_RFID_1 32
#define PIN_KANOP_RFID_2 33
#define PIN_KANOP_RFID_3 34
#define PIN_KANOP_RFID_4 36
#define PIN_KANOP_RFID_5 38
#define PIN_GERKON_KANOP_BOX 40
#define PIN_DOMOFON_KANOP_BOX 42 

#define PIN_ANUBIS_EYES 44
#define PIN_ANUBIS_NOSE_FOTO A15
#define PIN_ANUBIS_LIGHT 46

#define PIN_GERCON_SARKOFAG_OPEN 48
#define PIN_GERCON_SARKOFAG_CLOSE 35
#define PIN_MOTOR_SARKOFAG_OPEN 37
#define PIN_MOTOR_SARKOFAG_CLOSE 39
#define PIN_GERCON_MASKA 41
#define PIN_GERCON_MEDALLION 43
#define PIN_SARKOFAG_EYES 45
#define PIN_GERCON_SFINKS 47
#define PIN_SARKOFAG_LED 49

#define PIN_SARKOFAG_SMOKE_HOT 14           
#define PIN_SARKOFAG_SMOKE_START 15   

#define PIN_LED_PODSTAVKA 8
#define COUNT_PIXEL 15
#define PIN_DODEKAEDR_RFID 9
#define PIN_GERKON_DODEKAEDR_BOX 19 
#define PIN_DOMOFON_DODEKAEDR_BOX 20 
#define PIN_KOMPAS_1 13
//#define PIN_KOMPAS_2 12
//#define PIN_KOMPAS_3 19
//#define PIN_KOMPAS_4 20

#define PIN_GERKON_ZHYK 6
#define PIN_GERKON_ARTEFAKT_1 16
#define PIN_GERKON_ARTEFAKT_2 7
#define PIN_GERKON_ARTEFAKT_3 17

#define PIN_FAKELS_TOMB_1 3 
#define PIN_FAKELS_TOMB_2 4
#define PIN_FAKELS_TOMB_3 5 
//###############################
 
#define PIN_RX_RO 12
#define PIN_TX_DI 11
#define PIN_RS485_CONTROL 10


//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+
struct { char Whom[10]="To_Bomba"; char Command[10]="Get_Info"; char Parametr[5]="DO"; } MESSAGE;    //Сообщение для устройств

struct { bool Bomba=false; } Device_Status;              //Статусы пдключения девайсов
struct { char From[5]=""; float Power=0; char Status[20]="Zero"; } BOMBA;                      //Объект бомбы




bool TR_Din_Machine_On = false;
bool TR_Fakels_12 = false;
bool TR_Fakels_Tomb = false;
bool TR_Door_Tomb_On=false;
bool TR_Door_Sarkofag_On=false;

unsigned long TM_Generator;

unsigned long TM;
unsigned long TM_Status_Message;



//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
SoftwareSerial RS485(PIN_RX_RO, PIN_TX_DI); // RX, TX
RF24 radio(18,53); //Объект радиомодуля Для MEGA2560 замените на RF24 radio(9,53);
OneWire RFD_KNP_1(PIN_KANOP_RFID_1); OneWire RFD_KNP_2(PIN_KANOP_RFID_2); OneWire RFD_KNP_3(PIN_KANOP_RFID_3); OneWire RFD_KNP_4(PIN_KANOP_RFID_4); OneWire RFD_KNP_5(PIN_KANOP_RFID_5); OneWire RFD_DDKDR(PIN_DODEKAEDR_RFID); // Считыватели конопов
GButton butt1(PIN_GENERATOR);
Adafruit_TiCoServo KMPS;
Adafruit_NeoPixel STRIP = Adafruit_NeoPixel(COUNT_PIXEL, PIN_LED_PODSTAVKA, NEO_RGB + NEO_KHZ800);

//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
void DEBUG_PRINT(String Str) {
  static int Num_Sm;
  Num_Sm++;
  PRINT("Msg(" + String(Num_Sm) +  ")   " + Str);
}
void SM(String Str, bool TR) { 
  static unsigned long NUM; NUM+=1;
  static String CMD=""; 
  unsigned long TM_After_Start = millis() / 1000ul; String Time;
  int H = (TM_After_Start / 3600ul); 
  int M = (TM_After_Start % 3600ul)/60ul;  
  int S = (TM_After_Start % 3600ul)%60ul;
  if (H>9) Time+=String(H); else Time+="0"+String(H); Time+=":";
  if (M>9) Time+=String(M); else Time+="0"+String(M); Time+=":";
  if (S>9) Time+=String(S); else Time+="0"+String(S); 

  PRINT("{s.d.c:" + String(CMD) + "}");
  PRINT("{s.d.n:" + String(NUM) + "}");
  PRINT("{s.d.t:" + String(millis()) + "}");
  //PRINT("["+Time+"]["+String(NUM)+"]["+CMD+"]---");
  CMD=Str; if (TR) PRINT("\n");
}
/*bool ST(bool Flag ) {
  if (Flag) PRINTln(F(" ... Done!")); 
  else PRINTln(F(" ... ERROR!"));
  return Flag;
}*/
void SEND_COMMAND( int Whom, int Command, int Int ) {
  struct cmd { int Command;  int Int=0; } CMD;      //Команда сервера
  CMD.Command = Command;
  CMD.Int = Int;
  if (Whom==123) Serial1.write((byte*)&CMD, sizeof(CMD));
}
bool UP_RADIO(){
  bool TR=radio.begin();
  radio.setChannel(100); // выбираем канал (от 0 до 127)
  radio.setDataRate(RF24_1MBPS); // скорость: RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
  radio.setPALevel(RF24_PA_HIGH); // мощность: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM
  radio.enableAckPayload(); // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
  radio.openWritingPipe(0xAABBCCDD11LL); // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
  return TR;
}
void PRINT(String Str) {
  digitalWrite(PIN_RS485_CONTROL, HIGH);
  //delay(100);
  //Serial.print(Str);
  RS485.print(Str);
  digitalWrite(PIN_RS485_CONTROL, LOW);
  //delay(500);
}
void SET_RGB(int i, int R, int G, int B) {STRIP.setPixelColor(i, STRIP.Color(B,R,G));}

//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup() { 
  //Serial.begin(9600);
  RS485.begin(57600); RS485.setTimeout(200); pinMode(PIN_RS485_CONTROL, OUTPUT); digitalWrite(PIN_RS485_CONTROL, LOW); PRINT("\n"); 
  STRIP.begin();
  STRIP.setBrightness(255);
  for (int i=0; i<COUNT_PIXEL; i++) SET_RGB(i, 0, 0, 0); 
  STRIP.show();
  
 
  //###############################
  //1111111111111111111111111111111
  //###############################
  pinMode(PIN_GERKON_1_DOOR, INPUT_PULLUP); 
  pinMode(PIN_DOMOFON_1_DOOR, OUTPUT); digitalWrite(PIN_DOMOFON_1_DOOR, HIGH);  

  pinMode(PIN_DATCHIK_DVIZH, INPUT);

  pinMode(PIN_GERKON_GRID_OPEN, INPUT_PULLUP); 
  pinMode(PIN_GERKON_GRID_CLOSE, INPUT_PULLUP); 
  pinMode(PIN_DOMOFON_GRID, OUTPUT); digitalWrite(PIN_DOMOFON_GRID, HIGH); 
  pinMode(PIN_STARBOSKOP, OUTPUT); 

  //pinMode(PIN_GENERATOR, INPUT_PULLUP);

  pinMode(PIN_FAKELS, OUTPUT);

  pinMode(PIN_STOP_GAME_BUTTON, INPUT_PULLUP);

  pinMode(PIN_SMOKE_HOT, OUTPUT);   digitalWrite(PIN_SMOKE_HOT, HIGH);
  pinMode(PIN_SMOKE_START, OUTPUT); digitalWrite(PIN_SMOKE_START, HIGH);   
  //###############################
  //2222222222222222222222222222222
  //###############################
  pinMode(PIN_TILE_1, INPUT_PULLUP);
  pinMode(PIN_TILE_2, INPUT_PULLUP);
  pinMode(PIN_TILE_3, INPUT_PULLUP);
  pinMode(PIN_TILE_4, INPUT_PULLUP);
  pinMode(PIN_TILE_5, INPUT_PULLUP);
  pinMode(PIN_TILE_6, INPUT_PULLUP);

  pinMode(PIN_GERKON_DOOR_TOMB_OPEN, INPUT_PULLUP);
  pinMode(PIN_GERKON_DOOR_TOMB_CLOSE, INPUT_PULLUP);
  pinMode(PIN_MOTOR_DOOR_TOMB_OPEN, OUTPUT); digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH);
  pinMode(PIN_MOTOR_DOOR_TOMB_CLOSE, OUTPUT); digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH);
  pinMode(PIN_ZASHITA, INPUT_PULLUP); 
  //###############################
  //3333333333333333333333333333333
  //###############################
  pinMode(PIN_GERKON_BURNT_FAKEL, INPUT_PULLUP); 

  pinMode(PIN_GERKON_ARCHEOLOGST, INPUT_PULLUP); 
  pinMode(PIN_DOMOFON_ARCHEOLOGST, OUTPUT); digitalWrite(PIN_DOMOFON_ARCHEOLOGST, HIGH);

  pinMode(PIN_GERKON_KANOP_BOX, INPUT_PULLUP); 
  pinMode(PIN_DOMOFON_KANOP_BOX, OUTPUT); digitalWrite(PIN_DOMOFON_KANOP_BOX, HIGH);

  pinMode(PIN_ANUBIS_EYES, OUTPUT); 
  pinMode(PIN_ANUBIS_NOSE_FOTO, INPUT);
  pinMode(PIN_ANUBIS_LIGHT, OUTPUT); 

  pinMode(PIN_GERCON_SARKOFAG_OPEN, INPUT_PULLUP);
  pinMode(PIN_GERCON_SARKOFAG_CLOSE, INPUT_PULLUP);
  pinMode(PIN_MOTOR_SARKOFAG_OPEN, OUTPUT); digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH);
  pinMode(PIN_MOTOR_SARKOFAG_CLOSE, OUTPUT); digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH); 
  pinMode(PIN_GERCON_MASKA, INPUT_PULLUP); 
  pinMode(PIN_GERCON_MEDALLION, INPUT_PULLUP);  
  pinMode(PIN_SARKOFAG_EYES, OUTPUT);
  pinMode(PIN_GERCON_SFINKS, INPUT_PULLUP);  
  pinMode(PIN_SARKOFAG_LED, OUTPUT);    

  pinMode(PIN_SARKOFAG_SMOKE_HOT, OUTPUT); digitalWrite(PIN_SARKOFAG_SMOKE_HOT, HIGH);  
  pinMode(PIN_SARKOFAG_SMOKE_START, OUTPUT); digitalWrite(PIN_SARKOFAG_SMOKE_START, HIGH);  

  pinMode(PIN_GERKON_DODEKAEDR_BOX, INPUT_PULLUP);
  pinMode(PIN_DOMOFON_DODEKAEDR_BOX, OUTPUT); digitalWrite(PIN_DOMOFON_DODEKAEDR_BOX, HIGH);  
  KMPS.attach(PIN_KOMPAS_1); KMPS.write(0); delay(1000); KMPS.detach();

  pinMode(PIN_GERKON_ZHYK, INPUT_PULLUP);
  pinMode(PIN_GERKON_ARTEFAKT_1, INPUT_PULLUP);
  pinMode(PIN_GERKON_ARTEFAKT_2, INPUT_PULLUP);
  pinMode(PIN_GERKON_ARTEFAKT_3, INPUT_PULLUP); 

  pinMode(PIN_FAKELS_TOMB_1, OUTPUT);       
  pinMode(PIN_FAKELS_TOMB_2, OUTPUT);      
  pinMode(PIN_FAKELS_TOMB_3, OUTPUT);                                       
  //###############################
}


//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop() {    
  static unsigned long NUM; NUM+=1;  static String Last_Command;
  butt1.tick();

  //Страбоскоп
  static unsigned long TM_Strab_Work; static int Strab_Iter;
  if (Strab_Iter!=0 and millis()-TM_Strab_Work>50) { Strab_Iter--; TM_Strab_Work=millis(); digitalWrite(PIN_STARBOSKOP, !digitalRead(PIN_STARBOSKOP)); if (Strab_Iter==0) digitalWrite(PIN_STARBOSKOP, LOW); }

  //Дым машина 1
  static bool TR_Smoke_1_On; static unsigned long TM_Smoke_1;
  if (TR_Smoke_1_On and millis()-TM_Smoke_1>2000) { digitalWrite(PIN_SMOKE_START, HIGH); TR_Smoke_1_On=false;}

  //Генератор
  static int Light; static unsigned long TM_Low_Light; static unsigned long TM_Rot_Start; static int Count;
  if (!TR_Din_Machine_On) {

    if (Light!=0 and millis()-TM_Low_Light>30) { TM_Low_Light=millis(); Light-=1; analogWrite(PIN_FAKELS, Light); }

    if ( butt1.isClick()) {
      PRINT("\n{s.dm.s:a}\n");
      if (Count==0) TM_Rot_Start=millis();
      Count++;
      Light+=random(10, 15); if (Light>30) Light=30;
    } if (Count!=0 and millis()-TM_Rot_Start>5000) {
      TM_Rot_Start=millis();
      if (Count>4) { TR_Din_Machine_On=true; } else Count=0; 
    }
  }

  //Компас
  static unsigned long TM_Compass; static bool TR_Compas;
  if (TR_Compas and millis()-TM_Compass>3000) { TR_Compas=false; KMPS.detach(); }

  //Каски
  static unsigned long TM_Kaska;
  if (millis()-TM_Kaska>1000) {
    TM_Kaska=millis();
    radio.begin();
    radio.setChannel(100); // выбираем канал (от 0 до 127)
    radio.setDataRate(RF24_1MBPS); // скорость: RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
    radio.setPALevel(RF24_PA_HIGH); // мощность: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM
    radio.enableAckPayload(); // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
    radio.openWritingPipe(0xAABBCCDD22LL); // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
    radio.write(&MESSAGE, sizeof(MESSAGE)); 
    if (radio.isAckPayloadAvailable()) radio.read(&BOMBA, sizeof(BOMBA)); 
  }




  //Сообщения
  if (millis()-TM_Status_Message>500) {
    static int Message_Num;
    switch (Message_Num) {
      //case 0: { PRINT(String(NUM/(millis()/1000))+"{s.d.n:" + String(NUM) + "}");    Message_Num++; break; }
      case 0: { PRINT("{s.d.n:" + String(NUM) + "}");    Message_Num++; break; }
      case 1: { PRINT("{s.d.t:" + String(millis()) + "}");    Message_Num++; break; }
      case 2: { PRINT("{s.d.c:" + String(Last_Command) + "}");    Message_Num++; break; }
      case 3: { PRINT("{i.b.n:MEGA_123}");    Message_Num++; break; }
      case 4: { PRINT("{s.r.o:" + String(UP_RADIO()) + "}");    Message_Num++; break; }
      case 5: { 
        bool TR=false;
        radio.write(&MESSAGE, sizeof(MESSAGE)); 
        if (radio.isAckPayloadAvailable()) { 
          radio.read(&BOMBA, sizeof(BOMBA)); 
          if (String(BOMBA.From)=="Bomba") {
            TR=true;
            strcpy(MESSAGE.Command, "Get_Info");
          } 
        }
        if (TR)  PRINT("{s.b.o:1}"); else  PRINT("{s.b.o:0}");
        Message_Num++; break; 
      }
      case 6: { PRINT("{s.b.v:" + String(BOMBA.Power) + "}");    Message_Num++; break; }
      case 7: { PRINT("{s.b.s:"); PRINT(BOMBA.Status); PRINT("}");    Message_Num++; break; }

      case 8: { PRINT("{s.fd.s:" + String(!digitalRead(PIN_GERKON_1_DOOR)) + "}");    Message_Num++; break; }
      case 9: { PRINT("{s.h.m:" + String(digitalRead(PIN_DATCHIK_DVIZH)) + "}");    Message_Num++; break; }
      case 10: { PRINT("{s.gd.s:" + String(!digitalRead(PIN_GERKON_GRID_OPEN))+String(!digitalRead(PIN_GERKON_GRID_CLOSE)) + "}");    Message_Num++; break; }
      case 11: { if (!TR_Din_Machine_On) PRINT("{s.dm.s:i}"); else PRINT("{s.dm.s:s}");    Message_Num++; break; }
      case 12: { PRINT("{s.bt.s:" + String(digitalRead(PIN_STOP_GAME_BUTTON)) + "}");    Message_Num++; break; }

      case 13: { PRINT("{s.t.s:" + String(!digitalRead(PIN_TILE_1))+String(!digitalRead(PIN_TILE_2))+String(!digitalRead(PIN_TILE_3))+String(!digitalRead(PIN_TILE_4))+String(!digitalRead(PIN_TILE_5))+String(!digitalRead(PIN_TILE_6)) + "}");    Message_Num++; break; }
      case 14: { PRINT("{s.td.s:" + String(!digitalRead(PIN_GERKON_DOOR_TOMB_OPEN))+String(!digitalRead(PIN_GERKON_DOOR_TOMB_CLOSE)) + "}");    Message_Num++; break; }
      
      case 15: { PRINT("{s.bf.s:" + String(!digitalRead(PIN_GERKON_BURNT_FAKEL)) + "}");    Message_Num++; break; }
      case 16: { PRINT("{s.ar.s:" + String(!digitalRead(PIN_GERKON_ARCHEOLOGST)) + "}");    Message_Num++; break; }
      case 17: { PRINT("{s.tk.s:" + String(!digitalRead(PIN_GERKON_KANOP_BOX)) + "}");     Message_Num++; break; }
      case 18: { PRINT("{s.da.s:" + String(analogRead(PIN_ANUBIS_NOSE_FOTO)) + "}");     Message_Num++; break; }
      case 19: { PRINT("{s.sd.s:" + String(!digitalRead(PIN_GERCON_SARKOFAG_OPEN))+String(!digitalRead(PIN_GERCON_SARKOFAG_CLOSE)) + "}");    Message_Num++; break; }
      case 20: { PRINT("{s.ms.s:" + String(!digitalRead(PIN_GERCON_MASKA)) + "}");    Message_Num++; break; }
      case 21: { PRINT("{s.mds.s:" + String(!digitalRead(PIN_GERCON_MEDALLION)) + "}");    Message_Num++; break; }
      case 22: { PRINT("{s.sfs.s:" + String(!digitalRead(PIN_GERCON_SFINKS)) + "}");    Message_Num++; break; }
      case 23: { PRINT("{s.tud.s:" + String(!digitalRead(PIN_GERKON_DODEKAEDR_BOX)) + "}");    Message_Num++; break; }
      case 24: { PRINT("{s.gz.s:"+String(!digitalRead(PIN_GERKON_ZHYK))+"}");    Message_Num++; break; }
      case 25: { PRINT("{s.ga.s:" + String(!digitalRead(PIN_GERKON_ARTEFAKT_1))+String(!digitalRead(PIN_GERKON_ARTEFAKT_2))+String(!digitalRead(PIN_GERKON_ARTEFAKT_3))+"}");    Message_Num++; break; }

      case 26: { PRINT("\n"); Message_Num=0; TM_Status_Message=millis(); }
    }

    /*
    if (RFD_KNP_1.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_KNP_1.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_KNP_1.read(); else Two+=RFD_KNP_1.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINTln("Каноп_1 = "+String(One)+String(Two)); // выводим
    }
    if (RFD_KNP_2.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_KNP_2.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_KNP_2.read(); else Two+=RFD_KNP_2.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINTln("Каноп_2 = "+String(One)+String(Two)); // выводим
    }
    if (RFD_KNP_3.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_KNP_3.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_KNP_3.read(); else Two+=RFD_KNP_3.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINTln("Каноп_3 = "+String(One)+String(Two)); // выводим
    }
    if (RFD_KNP_4.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_KNP_4.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_KNP_4.read(); else Two+=RFD_KNP_4.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINTln("Каноп_4 = "+String(One)+String(Two)); // выводим
    }
    if (RFD_KNP_5.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_KNP_5.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_KNP_5.read(); else Two+=RFD_KNP_5.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINTln("Каноп_5 = "+String(One)+String(Two)); // выводим
    }
    if (RFD_DDKDR.reset()) { // если обнаружено устройттво
      int One; int Two;
      RFD_DDKDR.write(0x33); delay(50); // отправляем команду "считать ROM"
      for(int i=0;i<8;i++){ if (i%2) One+=RFD_DDKDR.read(); else Two+=RFD_DDKDR.read(); } // считываем
      if (String(One)+String(Two)!="10241020") PRINT("Додекаедр = "+String(One)+String(Two)); // выводим
    }
    */

  }

  //Команды
  if (RS485.available() > 0) {
    while (1) {
      String command = RS485.readStringUntil(']'); // Отдельная команда      
      String commandValue = ""; // Значение, необязательно      
      if (command.length() == 0) { break; } // Если команды не поступило, прерываем цикл      
      if (command.substring(0, 1) != "[") { continue; } // Если команда начинается не с [, пропускаем      
      command = command.substring(1); // Команда без начально [      
      Last_Command = command; //PRINTln(command);
      int delimeterIndex = command.lastIndexOf("=");

      if (delimeterIndex >= 0) {
        commandValue = command.substring(delimeterIndex + 1);
        command = command.substring(0, delimeterIndex);
      }
      
      //ДОМОФОННЫЕ МАМГНИТЫ
      if      (command == "dm_stdr") { digitalWrite(PIN_DOMOFON_1_DOOR, !bool(commandValue.toInt())); } 
      else if (command == "dm_grdr") { digitalWrite(PIN_DOMOFON_GRID, !bool(commandValue.toInt())); }       
      else if (command == "dm_arch") { digitalWrite(PIN_DOMOFON_ARCHEOLOGST, !bool(commandValue.toInt())); }   
      else if (command == "dm_tbkn") { digitalWrite(PIN_DOMOFON_KANOP_BOX, !bool(commandValue.toInt())); }  
      else if (command == "dm_tbdd") { digitalWrite(PIN_DOMOFON_DODEKAEDR_BOX, !bool(commandValue.toInt())); }  
      //ОСВЕЩЕНИЕ
      else if (command == "ld_1str") Strab_Iter=20;
      else if (command == "ld_12rm") { TR_Fakels_12=bool(commandValue.toInt()); if (!TR_Fakels_12) digitalWrite(PIN_FAKELS, LOW); } 
      else if (command == "ld_sfey") { digitalWrite(PIN_SARKOFAG_EYES, commandValue.toInt()); } 
      else if (command == "ld_ldsf") { digitalWrite(PIN_SARKOFAG_LED, commandValue.toInt()); } 
      else if (command == "ld_fktb") { TR_Fakels_Tomb=bool(commandValue.toInt()); if (!TR_Fakels_Tomb) { digitalWrite(PIN_FAKELS_TOMB_1, LOW); digitalWrite(PIN_FAKELS_TOMB_2, LOW); digitalWrite(PIN_FAKELS_TOMB_3, LOW); } } 
      else if (command == "ld_eyan") { digitalWrite(PIN_ANUBIS_EYES, bool(commandValue.toInt())); } 
      else if (command == "ld_span") { digitalWrite(PIN_ANUBIS_LIGHT, bool(commandValue.toInt())); } 
      else if (command == "ld_pddk") { 
        if      (String(commandValue.c_str())==String("R")) for (int i=0; i<COUNT_PIXEL; i++) SET_RGB(i, 255, 0, 0); 
        else if (String(commandValue.c_str())==String("G")) for (int i=0; i<COUNT_PIXEL; i++) SET_RGB(i, 0, 255, 0);
        else if (String(commandValue.c_str())==String("B")) for (int i=0; i<COUNT_PIXEL; i++) SET_RGB(i, 0, 0, 255);
        STRIP.show();
      }
      //ДЫМОВУХИ
      else if (command == "sm_1hot") digitalWrite(PIN_SMOKE_HOT, !bool(commandValue.toInt()));
      else if (command == "sm_1smk") { digitalWrite(PIN_SMOKE_START, LOW); TR_Smoke_1_On=true; TM_Smoke_1=millis(); }
      else if (command == "sm_3hot") digitalWrite(PIN_SARKOFAG_SMOKE_HOT, !bool(commandValue.toInt()));
      else if (command == "sm_3smk") { digitalWrite(PIN_SARKOFAG_SMOKE_START, LOW); delay(2000); digitalWrite(PIN_SARKOFAG_SMOKE_START, HIGH); }
      //ДВЕРИ НА ПРИВОДЕ
      else if (command == "md_2rm")  { 
        if       (!bool(commandValue.toInt()) and digitalRead(PIN_GERKON_DOOR_TOMB_CLOSE)) { digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH); digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, LOW); TR_Door_Tomb_On=true;}
        else if  (bool(commandValue.toInt())  and digitalRead(PIN_GERKON_DOOR_TOMB_OPEN)) { digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, LOW);  digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH); TR_Door_Tomb_On=true;}
      } 
      else if (command == "md_2rm_st") { digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH); digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH); }
      else if (command == "md_krsf")  { 
        if       (!bool(commandValue.toInt()) and digitalRead(PIN_GERCON_SARKOFAG_CLOSE)) { digitalWrite(PIN_MOTOR_SARKOFAG_OPEN, HIGH); digitalWrite(PIN_MOTOR_SARKOFAG_CLOSE, LOW); TR_Door_Sarkofag_On=true;}
        else if  (bool(commandValue.toInt())  and digitalRead(PIN_GERCON_SARKOFAG_OPEN)) { digitalWrite(PIN_MOTOR_SARKOFAG_OPEN, LOW);  digitalWrite(PIN_MOTOR_SARKOFAG_CLOSE, HIGH); TR_Door_Sarkofag_On=true;}
      } 
      else if (command == "md_krsf_st") { digitalWrite(PIN_MOTOR_SARKOFAG_OPEN, HIGH); digitalWrite(PIN_MOTOR_SARKOFAG_CLOSE, HIGH); }
      //ДРУГОЕ
      else if (command == "ot_bmss") { 
        strcpy(MESSAGE.Command, "Set_Stat"); strcpy(MESSAGE.Parametr, commandValue.c_str());
        radio.write(&MESSAGE, sizeof(MESSAGE));
        if (radio.isAckPayloadAvailable() ){ radio.read(&BOMBA, sizeof(BOMBA)); };
      } 
      else if (command == "ot_cmps") { 
        KMPS.attach(PIN_KOMPAS_1); KMPS.write(float(commandValue.toInt())/1.5); TR_Compas=true; } 
      //ОШИБКА
      else {
        PRINT("{r.c." + Last_Command + ":not_exist}\n");
        break;
      }
      PRINT("{r.c." + Last_Command + ":completed}\n");
    }
  }
  


  /*
  static unsigned long TM_Anubis_Led; static int Foto_Vol; static bool TR_Anubis_On=false; int i=10;
  if (millis()-TM_Anubis_Led>2000) {
    TM_Anubis_Led=millis(); PRINT("\n"+String(analogRead(PIN_ANUBIS_NOSE_FOTO))+"\n");

    while (true) {
      digitalWrite(PIN_ANUBIS_LIGHT, LOW); delay(1000);
      if (analogRead(PIN_ANUBIS_NOSE_FOTO)>Foto_Vol+i or analogRead(PIN_ANUBIS_NOSE_FOTO)<Foto_Vol-i) {
        Foto_Vol=analogRead(PIN_ANUBIS_NOSE_FOTO); PRINT("1\n"); 
      } else break;

      digitalWrite(PIN_ANUBIS_LIGHT, HIGH); delay(1000);
      if (analogRead(PIN_ANUBIS_NOSE_FOTO)>Foto_Vol+i or analogRead(PIN_ANUBIS_NOSE_FOTO)<Foto_Vol-i) {
        Foto_Vol=analogRead(PIN_ANUBIS_NOSE_FOTO); PRINT("2\n"); 
      } else break;

      digitalWrite(PIN_ANUBIS_LIGHT, LOW); delay(1000);
      if (analogRead(PIN_ANUBIS_NOSE_FOTO)>Foto_Vol+i or analogRead(PIN_ANUBIS_NOSE_FOTO)<Foto_Vol-i) {
        Foto_Vol=analogRead(PIN_ANUBIS_NOSE_FOTO); PRINT("3\n"); 
      } else break;

      digitalWrite(PIN_ANUBIS_LIGHT, HIGH); delay(1000);
      if (analogRead(PIN_ANUBIS_NOSE_FOTO)>Foto_Vol+i or analogRead(PIN_ANUBIS_NOSE_FOTO)<Foto_Vol-i) {
        Foto_Vol=analogRead(PIN_ANUBIS_NOSE_FOTO); PRINT("4\n"); 
      } else break;
      break;
    }
    digitalWrite(PIN_ANUBIS_LIGHT, HIGH);
  }
  */


  

  //6
  //Дверь в гробницу
  if (TR_Door_Tomb_On) {
    if      (!digitalRead(PIN_MOTOR_DOOR_TOMB_OPEN)  and !digitalRead(PIN_GERKON_DOOR_TOMB_OPEN )) { digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH); TR_Door_Tomb_On=false;   }
    else if (!digitalRead(PIN_MOTOR_DOOR_TOMB_CLOSE) and !digitalRead(PIN_GERKON_DOOR_TOMB_CLOSE)) { digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH); TR_Door_Tomb_On=false; }
    if (!digitalRead(PIN_ZASHITA)) { digitalWrite(PIN_MOTOR_DOOR_TOMB_OPEN, HIGH); digitalWrite(PIN_MOTOR_DOOR_TOMB_CLOSE, HIGH); TR_Door_Tomb_On=false; } 
  }
  //Дверь саркофага
  if (TR_Door_Sarkofag_On) {
    if      (!digitalRead(PIN_MOTOR_SARKOFAG_OPEN)  and !digitalRead(PIN_GERCON_SARKOFAG_OPEN )) { digitalWrite(PIN_MOTOR_SARKOFAG_OPEN, HIGH); TR_Door_Sarkofag_On=false;  }
    else if (!digitalRead(PIN_MOTOR_SARKOFAG_CLOSE) and !digitalRead(PIN_GERCON_SARKOFAG_CLOSE)) { digitalWrite(PIN_MOTOR_SARKOFAG_CLOSE, HIGH); TR_Door_Sarkofag_On=false; }
  }


  //Мерцание факелов
  unsigned long TM_Blink_Fakel_12; unsigned long TM_Blink_Fakel_Tomb_1; unsigned long TM_Blink_Fakel_Tomb_2; unsigned long TM_Blink_Fakel_Tomb_3;
  if (TR_Fakels_12 and millis()-TM_Blink_Fakel_12>100) { TM_Blink_Fakel_12 = millis(); analogWrite(PIN_FAKELS, 25+random(-10, 10)); }
  if (TR_Fakels_Tomb) {
    if (millis()-TM_Blink_Fakel_Tomb_1>40) { TM_Blink_Fakel_Tomb_1=millis(); analogWrite(PIN_FAKELS_TOMB_1, 25+random(-10, 10)); }
    if (millis()-TM_Blink_Fakel_Tomb_2>50) { TM_Blink_Fakel_Tomb_2=millis(); analogWrite(PIN_FAKELS_TOMB_2, 25+random(-10, 10)); }
    if (millis()-TM_Blink_Fakel_Tomb_3>60) { TM_Blink_Fakel_Tomb_3=millis(); analogWrite(PIN_FAKELS_TOMB_3, 25+random(-10, 10)); }
  }

  


}
