/**
 * @brief Тест подключений для меги 123
 */

#define R1_1 22
#define R1_3 23
#define R3_2 31
#define R3_4 42
#define R3_8 20
#define OFF 1
#define ON 0

uint8_t relays[] = {R1_1, R1_3, R3_2, R3_4, R3_8};

void init_relay(){
    for(size_t i = 0; i < sizeof(relays); ++i){
        pinMode(relays[i], OUTPUT);
    }
}

void set_relay(uint8_t state){
    for(size_t i = 0; i < sizeof(relays); ++i){
        digitalWrite(relays[i], state);
    }
}

void test_relay(){
    for(size_t i = 0; i < sizeof(relays); ++i){
        digitalWrite(relays[i], ON);
        delay(100);
    }
    for(size_t i = sizeof(relays) - 1; i > 0; --i){
        digitalWrite(relays[i], OFF);
        delay(100);
    }
}

void setup(){
    set_relay(OFF);
}


void loop(){
    test_relay();
    return;
}