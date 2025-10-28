#pragma once

// #include "led.h"
#include <stdbool.h>

// Варианты программирования универсальных выходов
enum io_output_func {
    IO_OUTPUT_FUNC_NONE = 0,        // Выключен
    IO_OUTPUT_FUNC_SIREN,           // Сирена
    IO_OUTPUT_FUNC_LIGHT,           // Повороты
    IO_OUTPUT_FUNC_CZ_CLOSE,        // Закрывание Ц.З.
    IO_OUTPUT_FUNC_CZ_OPEN,         // Открывание Ц.З.
    IO_OUTPUT_FUNC_TRUNK_OPEN,      // Открывание Багажника
    IO_OUTPUT_FUNC_START_STOP,      // Start/Stop
    IO_OUTPUT_FUNC_ENGINE_LOCK,     // НЗБД (нормально замкнутая блокировка двигателя)
    IO_OUTPUT_FUNC_DOP1,            // Дополнительный канал 1
    IO_OUTPUT_FUNC_DOP2,            // Дополнительный канал 2
    IO_OUTPUT_FUNC_DOP3,            // Дополнительный канал 3
    IO_OUTPUT_FUNC_ENGINE_UNLOCK,   // НРБД (нормально разомкнутая блокировка двигателя)
    IO_OUTPUT_FUNC_HOOD_LOCK,       // Закрывание замка капота
    IO_OUTPUT_FUNC_HOOD_UNLOCK,     // Открывание замка капота
    IO_OUTPUT_FUNC_DOOR_LOCK,       // Закрывание штыревых блокираторов дверей
    IO_OUTPUT_FUNC_DOOR_UNLOCK,     // Открывание штыревых блокираторов дверей
    IO_OUTPUT_FUNC_HORN,            // Клаксон
    IO_OUTPUT_FUNC_AVARIYKA,        // Аварийка статусная (система удерживает низкий уровень в течение всего времени работы аварийки)
    IO_OUTPUT_FUNC_AVAR_IMPULSE,    // Аварийка импульсная (первый импульс включает аварийку, второй – выключает)
    IO_OUTPUT_FUNC_CZ2_OPEN,        // Открывание второй ступени Ц.З.
    IO_OUTPUT_FUNC_BYPASS,          // Обходчик
    IO_OUTPUT_FUNC_KEY,             // Ключ
    IO_OUTPUT_FUNC_ACC,             // ACC
    IO_OUTPUT_FUNC_IGNITION,        // Зажигание
    IO_OUTPUT_FUNC_IGNITION2,       // Зажигание 2
    IO_OUTPUT_FUNC_STARTER,         // Стартер
    IO_OUTPUT_FUNC_OPEN_DOOR,       // Открывание двери
    IO_OUTPUT_FUNC_BRAKE,           // Тормоз
    IO_OUTPUT_FUNC_COMFORT,         // Комфорт
    IO_OUTPUT_FUNC_INLIGHT,         // Подсветка
    IO_OUTPUT_FUNC_FORCE,           // Статус охраны
    IO_OUTPUT_FUNC_REGISTRATOR,     // Регистратор
};
#define IO_OUTPUT_FUNC_MAX_VALUE IO_OUTPUT_FUNC_REGISTRATOR

// void io_out_set(enum io_output_func out_func, int state);

enum io_polarity {
    IO_POLARITY_NORMAL = 0,
    IO_POLARITY_INVERTED
};

// typedef struct io_output_config {
//     enum io_output_func func;
//     enum io_polarity polarity;
// } io_output_t;


// In1 – Зажигание (+) ; 6V
// In2 – Капот (–) ; 3V
// In3 – PIN (Сервисная кнопка) (–) ; 3V
// In4 – Парковка (–) ; 3V
// In5 – Тормоз (+) ; 6V
// In6 – Дверь водителя (–) ; 3V
// In7 – Ручка (–) ; 3V
// In8 – +12Vрез. (+)
// Варианты программирования универсальных входов
// Двери пассажирские (–) ; 3V
// Багажник (–) ; 3V
// Открывание багажника (+) ; 6V
// AntiHiJack (+) ; 6V
// Масло (+) ; 6V
// Мастер (–) ; 3V
// Разрешение (+) ; 6V
// Разрешение постановки (+) ; 6V
// Разрешение снятия (+) ; 6V
// Запрет (–) ; 3V
// Запрет постановки (–) ; 3V
// Запрет снятия (–) ; 3V
// Зона 1 (–) ; 3V
// Зона 2 (–) ; 3V
// Паника штатной системы охраны (+) ; 6V
// Свечи (+) ; 6V
// Порог (+) ; 12.2V


enum io_input_func {
    IO_INPUT_FUNC_NONE = 0,         // Выключен
    IO_INPUT_FUNC_IGNITION,         // Зажигание
    IO_INPUT_FUNC_HOOD,             // Капот
    IO_INPUT_FUNC_PIN,              // PIN
    IO_INPUT_FUNC_PARKING,          // Парковка
    IO_INPUT_FUNC_BRAKE,            // Тормоз
    IO_INPUT_FUNC_DRIVER_DOOR,      // Дверь водителя
    IO_INPUT_FUNC_HANDLE,           // Ручка
    IO_INPUT_FUNC_12VRES,           // +12Vрез

    IO_INPUT_FUNC_PASSENGERS_DOOR,  // Двери пассажирские
    IO_INPUT_FUNC_TRUNK,            // Багажник
    IO_INPUT_FUNC_TRUNK_OPEN,       // Открывание багажника
    IO_INPUT_FUNC_ANTI_HIJACK,      // AntiHiJack
    IO_INPUT_FUNC_OIL,              // Масло
    IO_INPUT_FUNC_MASTER,           // Мастер
    IO_INPUT_FUNC_ENABLE,           // Разрешение
    IO_INPUT_FUNC_ENABLE_ARM,       // Разрешение постановки
    IO_INPUT_FUNC_ENABLE_DISARM,    // Разрешение снятия
    IO_INPUT_FUNC_DISABLE,          // Запрет
    IO_INPUT_FUNC_DISABLE_ARM,      // Запрет постановки
    IO_INPUT_FUNC_DISABLE_DISARM,   // Запрет снятия
    IO_INPUT_FUNC_ZONE1,            // Зона 1
    IO_INPUT_FUNC_ZONE2,            // Зона 2
    IO_INPUT_FUNC_PANIC,            // Паника штатной системы охраны
    IO_INPUT_FUNC_CANDLES,          // Свечи
    IO_INPUT_FUNC_THRESHOLD,        // Порог

    IO_INPUT_FUNC_DOOR_FRONT_RIGHT, // Передние правые двери
    IO_INPUT_FUNC_DOOR_BACK_RIGHT,  // Задние правые двери
    IO_INPUT_FUNC_DOOR_FRONT_LEFT,  // Передние левые двери
    IO_INPUT_FUNC_DOOR_BACK_LEFT,   // Задние левые двери

    IO_INPUT_FUNC_HAND_BRAKE,       // Ручной тормоз


};
#define IO_INPUT_FUNC_MAX_VALUE IO_INPUT_FUNC_HAND_BRAKE

// void input_state_cb(int channel, bool state);

const char *io_input_func_to_value(enum io_input_func func);
enum io_input_func io_input_value_to_func(const char *value);
const char *io_output_func_to_value(enum io_output_func func);
enum io_output_func io_output_value_to_func(const char *value);

