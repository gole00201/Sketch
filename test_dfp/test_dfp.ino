#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial(9, 8);
// 9 - RX, 8 - TX

void setup () {
	Serial.begin (9600);
    // открываем порт для консоли
	mySerial.begin (9600);
    // открываем софт-порт
	mp3_set_serial (mySerial);
    // задаем порт для плеера
	mp3_set_volume (30);
    //устанавливаем уровень громкости
}

void loop () {
	mp3_play (3002);
    Serial.print("Play sound");
	delay (6000);

	mp3_next ();
    // включаем следующий файл
	delay (6000);
}
