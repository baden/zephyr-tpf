#pragma once
#include <stdint.h>

// #include <zephyr/sys/util_macro.h>
#include <zephyr/sys/iterable_sections.h>
#include <zephyr/sys_clock.h>

// #include "../io/io.h"

#define TPF_MAX_NAME_LENGTH 100

// TODO: Use Zephyr staffs

#define xstr(s) str(s)
#define str(s) #s
#define _MERGE_(n1, n2) __MERGE__(n1, n2)
#define __MERGE__(n1, n2) n1 ## n2

#define _TPF_JOIN_NAME(n1, n2) __TPF_JOIN_NAME(n1, n2)
#define __TPF_JOIN_NAME(n1, n2) n1 ## n2

#define _TPF_DF(var)  _MERGE_(var, _FD)
#define _TPF_REC(var) _MERGE_(REC_, var)
#define _TPF_NAME(var) _MERGE_(REC_name_, var)
#define _TYPE(type_id)	_MERGE_(t_tpf_, type_id)

#define VAR(name) _MERGE_(prg_, name)

// typedef int16_t         t_tpf_INT16;
// typedef uint16_t        t_tpf_UINT16;
typedef uint8_t			t_tpf_BOOL;
typedef uint8_t			t_tpf_UINT8;
typedef int32_t         t_tpf_INT32;
typedef uint32_t        t_tpf_UINT32;
typedef const char		t_tpf_STR64[64];
typedef k_timeout_t     t_tpf_TIME;
typedef int32_t         t_tpf_TIME_MS;
typedef uint32_t		t_tpf_VOLTAGE;	// mV?
typedef int32_t        	t_tpf_CURRENT;	// mA?
typedef uint8_t			t_tpf_INPUT;	// Do not use enum io_input_func (alignment is different)
typedef uint8_t			t_tpf_OUTPUT;	// Do not use enum io_output_func (alignment is different)
// typedef int	            t_tpf_VOLTAGE;
// typedef int				t_tpf_TEMPERATURE;
typedef uint32_t			t_tpf_BITFIELD;	// Спеціальний тип, який використовується для мульти чекбоксів (до 32x значень, реально до 26х напевно)

// #define _TPF_ALIGN_INT16 2
// #define _TPF_ALIGN_UINT16 2

// TODO: Can i use sizeof()?
#define _TPF_ALIGN_BOOL 1
#define _TPF_ALIGN_UINT8 1
#define _TPF_ALIGN_INT32 4
#define _TPF_ALIGN_UINT32 4
#define _TPF_ALIGN_STR64 64
#define _TPF_ALIGN_TIME 4
#define _TPF_ALIGN_TIME_MS 4
#define _TPF_ALIGN_VOLTAGE 4
#define _TPF_ALIGN_CURRENT 4
#define _TPF_ALIGN_INPUT 1
#define _TPF_ALIGN_OUTPUT 1
#define _TPF_ALIGN_BITFIELD 4
// #define _TPF_ALIGN_TEMPERATURE 2
#define _TPF_ALIGN(_type) _TPF_ALIGN_##_type


// Uniform
// #define TPF_CONV_INT16(_value) _value
// #define TPF_CONV_UINT16(_value) _value
#define TPF_CONV_BOOL(_value) (_value ? 1 : 0)
#define TPF_CONV_UINT8(_value) _value
#define TPF_CONV_INT32(_value) _value
#define TPF_CONV_UINT32(_value) _value
#define TPF_CONV_STR64(_value) _value
#define TPF_CONV_INPUT(_value) _value
#define TPF_CONV_OUTPUT(_value) _value
// #define TPF_CONV_TIME(_value) k_ticks_to_ms_floor32(_value.ticks)
#define TPF_CONV_TIME(_value) _value
#define TPF_CONV_TIME_MS(_value) _value
#define TPF_CONV_VOLTAGE(_value) (unsigned int)(_value * 1000U)
#define TPF_CONV_CURRENT(_value) (int)(_value * 1000U)
// #define TPF_CONV_VOLTAGE(_value) (int)(_value * 1000)
#define TPF_CONV_TEMPERATURE(_value) (int)(_value * 10)
#define TPF_CONV_BITFIELD(_value) _value

#define TPF_CONV(_type, _value) TPF_CONV_##_type(_value)


#define	_PRG(_type)   __attribute__((section(".progtable."  xstr(_TPF_ALIGN(_type)))))
#define _PRG_A  __attribute__((section(".progtable_array")))


typedef enum {
	// PRG_TYPE_INT16		=  0,	// 16-ти битное значение
	// PRG_TYPE_UINT16		=  1,	//
	PRG_TYPE_BOOL		=  0,	// Не подобається мені цей тип, треба якось його упакувати
	PRG_TYPE_UINT8		=  1,	// 8-мі беззнакове бітне значення
	PRG_TYPE_INT32		=  2,	// 32-x битное значение
	PRG_TYPE_UINT32		=  3,	//
	// PRG_TYPE_INT64		=  2,	// Значение 64 бит знаковое
	// PRG_TYPE_STR16		=  3,	// Строковое значение (до 16ти символов)
	PRG_TYPE_STR64		=  4,	// Строковое значение (до 64х символов)
	// // PRG_TYPE_BOOL		=  5,	// Булевое значение (1 бит) * Я пока не придумал механизма хранения значений
	PRG_TYPE_TIME		=  5,	// Значение, используется в таймерах, интервалах времени и т.п.
	PRG_TYPE_TIME_MS	=  6,	// Значение, используется в таймерах, интервалах времени и т.п. Значение в миллисекундах.
	// PRG_TYPE_PIN4		=  6,	// Двоично-десятичное значение 0xXXXX соответствует строке 0000...9999
	// PRG_TYPE_SIGNALS	=  7,	// Битовая комбинация off, lights, siren, both
	// PRG_TYPE_ALERTS		=  8,	// Битовая комбинация off, sms, call, both
	PRG_TYPE_INPUT		=  9,	// Функция входа
	// PRG_TYPE_INPUT_CON	= 10,	// Тип контакта входа
	// PRG_TYPE_INPUT_IMP	= 11,	// Импеданс контакта входа
	PRG_TYPE_OUTPUT		= 12,	// Функция выхода
	// PRG_TYPE_ACT		= 13,	// Битовая комбинация off, arm, disarm, both
	PRG_TYPE_VOLTAGE	= 14,	// Напряжения (хранение в милливольтах)
	PRG_TYPE_CURRENT	= 15,	// Ток (хранение в миллиамперах)
	// PRG_TYPE_TEMPERATURE= 15,	// Температура (1 == 0.1 градуса)
	// PRG_TYPE_TILTANGLE	= 16,	// Угол в градусах. На самом деле хранится квадрат произведения на 12.0
    //
    //
	PRG_TYPE_BITFIELD	= 20,	// Спеціальний тип, який використовується для мульти чекбоксів (до 32x значень)

	PRG_TYPE_META		= 31	// Значение, состоящее из нескольких других. Мета-константа, не имеющая физического воплощения
	// 							// Данные константы располагаются в глобальной области и не доступны для модификации.
	// 							// Вместо этого SMS-команда 1111 set <meta> <value1> <value2> <value3> ...
	// 							// Модифицирует соответствущие констнанты.
	// 							// Данная константа не имеет понятие "заводская установка".

} tprg_type;

struct tPrgParm {
	const char *name;
	const tprg_type type;
	const void *value_ptr;
	const void *default_ptr;
};

#define TPF(__name) (_MERGE_(prg_, __name))

#define TPF_FORM volatile

#define TPF_DEF(_type, _name, _value)                                       \
    TPF_FORM _TYPE(_type) VAR(_name) _PRG(_type) = TPF_CONV(_type, _value); \
    const _TYPE(_type) _TPF_DF(_name) = TPF_CONV(_type, _value);        	\
    const char _TPF_NAME(_name)[] = {xstr(_name)};                          \
    const STRUCT_SECTION_ITERABLE(tPrgParm, _name) = {                      \
        .name = _TPF_NAME(_name),                                           \
        .type = _MERGE_(PRG_TYPE_, _type),                                  \
        .value_ptr = (void*)&VAR(_name),                                    \
        .default_ptr = (void*)&_TPF_DF(_name),                              \
    }

void tpfDumpRam(void);
bool tpfPatchInMem(const char *name, const char *value);

int tpf_dump_as_json(char *buffer, size_t buffer_size, bool include_defaults, uint32_t timestamp);
