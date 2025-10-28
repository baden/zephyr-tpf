#include "io.h"
#include <string.h>

// Треба впевнитись шо довжина строки не повинна перевищувати 16 символів
const char *io_input_func_to_value(enum io_input_func func)
{
    switch(func) {
    case IO_INPUT_FUNC_NONE:        return "none";              // Выключен
    case IO_INPUT_FUNC_IGNITION:    return "ignition";          // Зажигание
    case IO_INPUT_FUNC_HOOD:        return "hood";              // Капот
    case IO_INPUT_FUNC_PIN:         return "pin";               // PIN
    case IO_INPUT_FUNC_PARKING:     return "parking";           // Парковка
    case IO_INPUT_FUNC_BRAKE:       return "brake";             // Тормоз
    case IO_INPUT_FUNC_DRIVER_DOOR: return "door";              // Дверь водителя
    case IO_INPUT_FUNC_HANDLE:      return "handle";            // Ручка
    case IO_INPUT_FUNC_12VRES:      return "12v_res";           // +12Vрез

    case IO_INPUT_FUNC_PASSENGERS_DOOR: return "door_p";        // Двери пассажирские
    case IO_INPUT_FUNC_TRUNK:       return "trunk";             // Багажник
    case IO_INPUT_FUNC_TRUNK_OPEN:  return "trunk_open";        // Открывание багажника
    case IO_INPUT_FUNC_ANTI_HIJACK: return "ahj";               // AntiHiJack
    case IO_INPUT_FUNC_OIL:         return "oil";               // Масло
    case IO_INPUT_FUNC_MASTER:      return "master";            // Мастер
    case IO_INPUT_FUNC_ENABLE:      return "enable";            // Разрешение
    case IO_INPUT_FUNC_ENABLE_ARM:  return "enable_arm";        // Разрешение постановки
    case IO_INPUT_FUNC_ENABLE_DISARM: return "enable_disarm";   // Разрешение снятия
    case IO_INPUT_FUNC_DISABLE:     return "disable";           // Запрет
    case IO_INPUT_FUNC_DISABLE_ARM: return "disable_arm";       // Запрет постановки
    case IO_INPUT_FUNC_DISABLE_DISARM: return "disable_disarm"; // Запрет снятия
    case IO_INPUT_FUNC_ZONE1:       return "zone1";             // Зона 1
    case IO_INPUT_FUNC_ZONE2:       return "zone2";             // Зона 2
    case IO_INPUT_FUNC_PANIC:       return "panic";             // Паника штатной системы охраны
    case IO_INPUT_FUNC_CANDLES:     return "candles";           // Свечи
    case IO_INPUT_FUNC_THRESHOLD:   return "threshold";         // Порог

    case IO_INPUT_FUNC_DOOR_FRONT_RIGHT:    return "door_fr"; // Передние правые двери
    case IO_INPUT_FUNC_DOOR_BACK_RIGHT:     return "door_br";  // Задние правые двери
    case IO_INPUT_FUNC_DOOR_FRONT_LEFT:     return "door_fl";  // Передние левые двери
    case IO_INPUT_FUNC_DOOR_BACK_LEFT:      return "door_bl";   // Задние левые двери

    case IO_INPUT_FUNC_HAND_BRAKE:  return "hand_brake";       // Ручной тормоз
    }
    return "unknown";
}

enum io_input_func io_input_value_to_func(const char *value)
{
    unsigned int v = IO_INPUT_FUNC_MAX_VALUE;
    for(; v > 0; v--) {
        if(strcmp(value, io_input_func_to_value(v)) == 0) {
            return v;
        }
    }
    return IO_INPUT_FUNC_NONE;
}



// Треба впевнитись шо довжина строки не повинна перевищувати 16 символів
const char *io_output_func_to_value(enum io_output_func func)
{
    switch(func) {
    case IO_OUTPUT_FUNC_NONE:       return "none";          // Выключен
    case IO_OUTPUT_FUNC_SIREN:      return "siren";         // Сирена
    case IO_OUTPUT_FUNC_LIGHT:      return "light";         // Повороты
    case IO_OUTPUT_FUNC_CZ_CLOSE:   return "cz_close";      // Закрывание Ц.З.
    case IO_OUTPUT_FUNC_CZ_OPEN:    return "cz_open";       // Открывание Ц.З.
    case IO_OUTPUT_FUNC_TRUNK_OPEN: return "trunk_open";    // Открывание Багажника
    case IO_OUTPUT_FUNC_START_STOP: return "start_stop";    // Start/Stop
    case IO_OUTPUT_FUNC_ENGINE_LOCK:return "engine_lock";   // НЗБД (нормально замкнутая блокировка двигателя)
    case IO_OUTPUT_FUNC_DOP1:       return "dop1";          // Дополнительный канал 1
    case IO_OUTPUT_FUNC_DOP2:       return "dop2";          // Дополнительный канал 2
    case IO_OUTPUT_FUNC_DOP3:       return "dop3";          // Дополнительный канал 3
    case IO_OUTPUT_FUNC_ENGINE_UNLOCK: return "engine_unlock"; // НРБД (нормально разомкнутая блокировка двигателя)
    case IO_OUTPUT_FUNC_HOOD_LOCK:  return "hood_lock";     // Закрывание замка капота
    case IO_OUTPUT_FUNC_HOOD_UNLOCK:return "hood_unlock";   // Открывание замка капота
    case IO_OUTPUT_FUNC_DOOR_LOCK:  return "door_lock";     // Закрывание штыревых блокираторов дверей
    case IO_OUTPUT_FUNC_DOOR_UNLOCK:return "door_unlock";   // Открывание штыревых блокираторов дверей
    case IO_OUTPUT_FUNC_HORN:       return "horn";          // Клаксон
    case IO_OUTPUT_FUNC_AVARIYKA:   return "avariyka";      // Аварийка статусная (система удерживает низкий уровень в течение всего времени работы аварийки)
    case IO_OUTPUT_FUNC_AVAR_IMPULSE: return "avar_impulse"; // Аварийка импульсная (первый импульс включает аварийку, второй – выключает)
    case IO_OUTPUT_FUNC_CZ2_OPEN:   return "cz2_open";      // Открывание второй ступени Ц.З.
    case IO_OUTPUT_FUNC_BYPASS:     return "bypass";        // Обходчик
    case IO_OUTPUT_FUNC_KEY:        return "key";           // Ключ
    case IO_OUTPUT_FUNC_ACC:        return "acc";           // ACC
    case IO_OUTPUT_FUNC_IGNITION:   return "ignition";      // Зажигание
    case IO_OUTPUT_FUNC_IGNITION2:  return "ignition2";     // Зажигание 2
    case IO_OUTPUT_FUNC_STARTER:    return "starter";       // Стартер
    case IO_OUTPUT_FUNC_OPEN_DOOR:  return "open_door";     // Открывание двери
    case IO_OUTPUT_FUNC_BRAKE:      return "brake";         // Тормоз
    case IO_OUTPUT_FUNC_COMFORT:    return "comfort";       // Комфорт
    case IO_OUTPUT_FUNC_INLIGHT:    return "inlight";       // Подсветка
    case IO_OUTPUT_FUNC_FORCE:      return "force";         // Статус охраны
    case IO_OUTPUT_FUNC_REGISTRATOR:return "registrator";   // Регистратор
    }
    return "unknown";
}

enum io_output_func io_output_value_to_func(const char *value)
{
    unsigned int v = IO_OUTPUT_FUNC_MAX_VALUE;
    for(; v > 0; v--) {
        if(strcmp(value, io_output_func_to_value(v)) == 0) {
            return v;
        }
    }
    return IO_OUTPUT_FUNC_NONE;
}
