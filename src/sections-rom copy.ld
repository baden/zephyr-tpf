#include <zephyr/linker/iterable_sections.h>

ITERABLE_SECTION_ROM(tPrgParm, 4)
/* Теоретично достатньо вирівняти на розмір сторінки, яку можна стирати (8K для STM32H5xx). */
SECTION_PROLOGUE(progtable,,ALIGN(65536))
{
    _progtable_start = .;
    /* KEEP(*(.progtable*)) */
    KEEP (*(SORT(.progtable.1)))
    . = ALIGN(2);
    KEEP (*(SORT(.progtable.2)))
    . = ALIGN(4);
    KEEP (*(SORT(.progtable.4)))
    . = ALIGN(8);
    KEEP (*(SORT(.progtable.8)))
    . = ALIGN(16);
    KEEP (*(SORT(.progtable.16)))
    . = ALIGN(64);
    KEEP (*(SORT(.progtable.64)))
    /*. = ALIGN(4);*/
    _progtable_stop = .;
    . = ALIGN(0x0800);
    _progtable_end = .;
} GROUP_ROM_LINK_IN(RAMABLE_REGION/*, ROMABLE_REGION*/)
