#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial(9, 8);
// используем 10 и 11 пины
// для программного порта
// 10 - RX, 11 - TX

void setup () {
	Serial.begin (9600);
    // открываем порт для консоли
	mySerial.begin (9600);
    // открываем софт-порт
	mp3_set_serial (mySerial);
    // задаем порт для плеера
	mp3_set_volume (15);
    //устанавливаем уровень громкости
}

void loop () {
	mp3_play (3002);
    // проигрываем файл mp3/0001.mp3
	delay (6000);

	mp3_next ();
    // включаем следующий файл
	delay (6000);
}
