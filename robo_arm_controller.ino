/*
 * Arduino-посредник для управления роборукой на CD4000
 * 
 * Подключение:
 * - Реле/транзисторы параллельно кнопкам ПУСК и СБРОС
 * - Сдвиговый регистр 74HC595 для эмуляции DIP-переключателей
 * - Оптроны для чтения светодиодов (обратная связь)
 */

// Пины для эмуляции кнопок (через реле или транзисторы)
#define PIN_BTN_START    2   // Параллельно кнопке ПУСК
#define PIN_BTN_RESET    3   // Параллельно кнопке СБРОС

// Пины для эмуляции DIP-переключателя (8 бит через сдвиговый регистр 74HC595)
#define PIN_DIP_DATA     4   // Данные
#define PIN_DIP_CLOCK    5   // Тактовый
#define PIN_DIP_LATCH    6   // Защёлка

// Пины для чтения светодиодов (через оптроны или делители напряжения)
#define PIN_LED_READY    7   // Зелёный светодиод (готов)
#define PIN_LED_FAULT    8   // Красный светодиод (авария)
#define PIN_LED_POWER    9   // Жёлтый светодиод (питание)

// Пины для чтения сигнала MATCH (остановка по положению)
#define PIN_MATCH        10  // Сигнал совпадения с компаратора

void setup() {
    Serial.begin(115200);
    
    // Настройка пинов управления
    pinMode(PIN_BTN_START, OUTPUT);
    pinMode(PIN_BTN_RESET, OUTPUT);
    pinMode(PIN_DIP_DATA, OUTPUT);
    pinMode(PIN_DIP_CLOCK, OUTPUT);
    pinMode(PIN_DIP_LATCH, OUTPUT);
    
    // Настройка пинов чтения
    pinMode(PIN_LED_READY, INPUT);
    pinMode(PIN_LED_FAULT, INPUT);
    pinMode(PIN_LED_POWER, INPUT);
    pinMode(PIN_MATCH, INPUT);
    
    // Начальное состояние — кнопки не нажаты
    digitalWrite(PIN_BTN_START, LOW);
    digitalWrite(PIN_BTN_RESET, LOW);
    
    // Установка DIP в 0
    setDIPSwitch(0);
    
    Serial.println("READY");
}

void loop() {
    // Проверка аварии
    if (digitalRead(PIN_LED_FAULT) == HIGH) {
        Serial.println("FAULT");
    }
    
    // Проверка завершения движения
    if (digitalRead(PIN_MATCH) == HIGH) {
        Serial.println("GRIPPED");
    }
    
    // Обработка команд из браузера
    if (Serial.available() > 0) {
        byte command = Serial.read();
        
        switch (command) {
            case 0x00:  // PING
                Serial.println("READY");
                break;
                
            case 0x01:  // GRIP (нажать ПУСК)
                pressStart();
                break;
                
            case 0x02:  // RESET (нажать СБРОС)
                pressReset();
                break;
                
            case 0x03:  // SET_STEPS (установить DIP)
                if (Serial.available() > 0) {
                    byte steps = Serial.read();
                    setDIPSwitch(steps);
                }
                break;
        }
    }
}

// Эмуляция нажатия кнопки ПУСК
void pressStart() {
    Serial.println("MOVING");
    digitalWrite(PIN_BTN_START, HIGH);
    delay(100);  // Импульс 100 мс
    digitalWrite(PIN_BTN_START, LOW);
}

// Эмуляция нажатия кнопки СБРОС
void pressReset() {
    digitalWrite(PIN_BTN_RESET, HIGH);
    delay(100);  // Импульс 100 мс
    digitalWrite(PIN_BTN_RESET, LOW);
    Serial.println("RELEASED");
}

// Установка DIP-переключателя через сдвиговый регистр 74HC595
void setDIPSwitch(byte value) {
    digitalWrite(PIN_DIP_LATCH, LOW);
    shiftOut(PIN_DIP_DATA, PIN_DIP_CLOCK, MSBFIRST, value);
    digitalWrite(PIN_DIP_LATCH, HIGH);
}