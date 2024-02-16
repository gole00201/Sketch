#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial(9, 8);
// 9 - RX, 8 - TX

void setup () {
	Serial.begin(9600);
    // открываем порт для консоли
	mySerial.begin(9600);
    // открываем софт-порт
	mp3_set_serial(mySerial);
    // задаем порт для плеера
	mp3_set_volume(30);
    //устанавливаем уровень громкости
}

void loop () {
	mp3_play (1001);
    Serial.print("Play sound");
	delay (6000);
    mp3_play (1002);
    Serial.print("Play sound");
	delay (6000);
    mp3_play (1003);
    Serial.print("Play sound");
	delay (6000);
    mp3_play (1004);
    Serial.print("Play sound");
	delay (6000);
    mp3_play (1005);
    Serial.print("Play sound");
	delay (6000);
    // включаем следующий файл
	delay (6000);
}
