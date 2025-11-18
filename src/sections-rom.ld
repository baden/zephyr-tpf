#include <zephyr/linker/iterable_sections.h>

ITERABLE_SECTION_ROM(tPrgParm, 4)
/* Теоретично достатньо вирівняти на розмір сторінки, яку можна стирати (8K для STM32H5xx). */

