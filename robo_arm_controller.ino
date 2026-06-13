/*
 * Arduino-посредник для управления роборукой на CD4000
 * 
 * Подключение:
 * - Реле/транзисторы параллельно кнопкам ПУСК и СБРОС
 * - Сдвиговый регистр 74HC595 для эмуляции DIP-переключателей
 * - Оптроны для чтения светодиодов (обратная связь)
 */

#define PIN_BTN_START    2
#define PIN_BTN_RESET    3
#define PIN_DIP_DATA     4
#define PIN_DIP_CLOCK    5
#define PIN_DIP_LATCH    6
#define PIN_LED_READY    7
#define PIN_LED_FAULT    8
#define PIN_LED_POWER    9
#define PIN_MATCH        10

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BTN_START, OUTPUT);
    pinMode(PIN_BTN_RESET, OUTPUT);
    pinMode(PIN_DIP_DATA, OUTPUT);
    pinMode(PIN_DIP_CLOCK, OUTPUT);
    pinMode(PIN_DIP_LATCH, OUTPUT);
    pinMode(PIN_LED_READY, INPUT);
    pinMode(PIN_LED_FAULT, INPUT);
    pinMode(PIN_LED_POWER, INPUT);
    pinMode(PIN_MATCH, INPUT);
    digitalWrite(PIN_BTN_START, LOW);
    digitalWrite(PIN_BTN_RESET, LOW);
    setDIPSwitch(0);
    Serial.println("READY");
}

void loop() {
    if (digitalRead(PIN_LED_FAULT) == HIGH) {
        Serial.println("FAULT");
    }
    if (digitalRead(PIN_MATCH) == HIGH) {
        Serial.println("GRIPPED");
    }
    if (Serial.available() > 0) {
        byte command = Serial.read();
        switch (command) {
            case 0x00: Serial.println("READY"); break;
            case 0x01: pressStart(); break;
            case 0x02: pressReset(); break;
            case 0x03:
                if (Serial.available() > 0) {
                    byte steps = Serial.read();
                    setDIPSwitch(steps);
                }
                break;
        }
    }
}

void pressStart() {
    Serial.println("MOVING");
    digitalWrite(PIN_BTN_START, HIGH);
    delay(100);
    digitalWrite(PIN_BTN_START, LOW);
}

void pressReset() {
    digitalWrite(PIN_BTN_RESET, HIGH);
    delay(100);
    digitalWrite(PIN_BTN_RESET, LOW);
    Serial.println("RELEASED");
}

void setDIPSwitch(byte value) {
    digitalWrite(PIN_DIP_LATCH, LOW);
    shiftOut(PIN_DIP_DATA, PIN_DIP_CLOCK, MSBFIRST, value);
    digitalWrite(PIN_DIP_LATCH, HIGH);
}