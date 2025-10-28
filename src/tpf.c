#include "tpf.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>
// #include "app_version.h"
#include "io.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(tpf, LOG_LEVEL_DBG);


extern const void *_progtable_start;
extern const void *_progtable_stop;
extern const void *_progtable_end;

#define TPF_FLASH_AREA          storage_partition_tpf
#define TPF_FLASH_AREA_OFFSET   FIXED_PARTITION_OFFSET(TPF_FLASH_AREA)
#define TPF_FLASH_AREA_SIZE     FIXED_PARTITION_SIZE(TPF_FLASH_AREA)
#define TPF_FLASH_AREA_ID       FIXED_PARTITION_ID(TPF_FLASH_AREA)
#define TEST_NVS_FLASH_AREA_DEV FIXED_PARTITION_DEVICE(TPF_FLASH_AREA)

static const struct device *const flash_dev = TEST_NVS_FLASH_AREA_DEV;

#define ROM_FLASH_DEV DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));
static const struct device *const rom_flash_dev = ROM_FLASH_DEV;

struct nvs_fixture {
	struct nvs_fs fs;
	struct stats_hdr *sim_stats;
	struct stats_hdr *sim_thresholds;
};

// TODO: Якось це значення можна отримати від драйвера
// FLASH_STM32_WRITE_BLOCK_SIZE

// #define ERASE_BLOCK_SIZE 2048
#define FLASH_PARTITION storage_partition_tpf
#define FLASH_CONTROLLER DT_PARENT(DT_PARENT(DT_NODELABEL(FLASH_PARTITION)))
#define FLASH_WRITE_SIZE DT_PROP(FLASH_CONTROLLER, write_block_size)
#define FLASH_ERASE_BLOCK_SIZE DT_PROP(FLASH_CONTROLLER, erase_block_size)
// #define ERASE_BLOCK_SIZE DT_PROP(DT_PARENT(TEST_NVS_FLASH_AREA_DEV), erase_block_size)


#define FLASH_MEM CONFIG_FLASH_BASE_ADDRESS
#define RAM_MEM   CONFIG_SRAM_BASE_ADDRESS

void tpfDumpRam(void)
{
    int err;
    const struct flash_area *fa;
	struct flash_pages_info info;
    static struct nvs_fixture fixture;

    LOG_INF("TPF Flash area constants.");
    LOG_INF("TPF_FLASH_AREA_OFFSET = %d", TPF_FLASH_AREA_OFFSET);
    LOG_INF("TPF_FLASH_AREA_SIZE = %d", TPF_FLASH_AREA_SIZE);
    LOG_INF("FLASH_WRITE_SIZE = %d", FLASH_WRITE_SIZE);
    LOG_INF("FLASH_ERASE_BLOCK_SIZE = %d", FLASH_ERASE_BLOCK_SIZE);
    LOG_INF("FLASH_MEM = %x", FLASH_MEM);
    LOG_INF("  RAM_MEM = %x", RAM_MEM);

	__ASSERT_NO_MSG(device_is_ready(flash_dev));
	__ASSERT_NO_MSG(device_is_ready(rom_flash_dev));

    LOG_INF("ROM Flash dev: %s", rom_flash_dev->name);
    LOG_INF("Storage Flash dev: %s", flash_dev->name);

	err = flash_area_open(TPF_FLASH_AREA_ID, &fa);

    __ASSERT(err == 0, "Error opening storage area");

    fixture.fs.offset = TPF_FLASH_AREA_OFFSET;
    err = flash_get_page_info_by_offs(flash_area_get_device(fa), fixture.fs.offset,
					  &info);
    __ASSERT(err == 0, "Error getting page info");

    LOG_INF("TPF nvs info. size=%d", info.size);


    LOG_INF("__progtable_start = 0x%08x", (unsigned)&_progtable_start);
    LOG_INF("__progtable_stop = 0x%08x", (unsigned)&_progtable_stop);
    LOG_INF("__progtable_end = 0x%08x", (unsigned)&_progtable_end);



    STRUCT_SECTION_FOREACH(tPrgParm, p) {
        tprg_type t = p->type;
        switch (t)
        {
        case PRG_TYPE_TIME: {
            const k_timeout_t* t = (k_timeout_t*)(p->value_ptr);
            uint32_t ms = k_ticks_to_ms_floor32(t->ticks);
            if(ms % 1000 == 0) {
                LOG_INF("tpf(%s)= K_SECONDS(%d)", p->name, ms/1000);
            } else {
                LOG_INF("tpf(%s)= K_MSEC(%d)", p->name, ms);
            }
            break;
        }


        case PRG_TYPE_INPUT: {
            const uint8_t* v = (uint8_t*)(p->value_ptr);
            LOG_INF("tpf(%s)=%s", p->name, io_input_func_to_value(*v));
            break;
        }

        case PRG_TYPE_OUTPUT: {
            const uint8_t* v = (uint8_t*)(p->value_ptr);
            LOG_INF("tpf(%s)=%s", p->name, io_output_func_to_value(*v));
            break;
        }

        default:
            LOG_INF("tpf(%s)=%d", p->name, *(int*)p->value_ptr);
            break;
        }
    }

    // Dump ROM-data
    off_t address = (off_t)&_progtable_start;

    uint8_t buffer[FLASH_ERASE_BLOCK_SIZE];
    const uint8_t *rom_data = (const uint8_t *)address;

    // TODO: Немає ніякого сенсу читати через flash_read бо вся пам'ять
    // доступна для прямого читання. Це виключно для тесту.
    flash_read(rom_flash_dev, address - CONFIG_FLASH_BASE_ADDRESS, &buffer, sizeof(buffer));
    LOG_HEXDUMP_INF(buffer, 64, "readed ROM of constants");

    LOG_HEXDUMP_INF(rom_data, 64, "direct ROM of constants");

    // TPF_test1("in_func");

    LOG_INF("TPF Erasing....");
	/* erase flash content */
	flash_erase(rom_flash_dev, address - CONFIG_FLASH_BASE_ADDRESS, FLASH_ERASE_BLOCK_SIZE);

    for(int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = i;
    }

    // LOG_INF("TPF Write data.");
    // flash_write(rom_flash_dev, address, &buffer, sizeof(buffer));


    LOG_INF("Dump again.");

    STRUCT_SECTION_FOREACH(tPrgParm, p) {
        tprg_type t = p->type;
        switch (t)
        {
        case PRG_TYPE_TIME: {
            const k_timeout_t* t = (k_timeout_t*)(p->value_ptr);
            uint32_t ms = k_ticks_to_ms_floor32(t->ticks);
            if(ms % 1000 == 0) {
                LOG_INF("tpf(%s)= K_SECONDS(%d)", p->name, ms/1000);
            } else {
                LOG_INF("tpf(%s)= K_MSEC(%d)", p->name, ms);
            }
            break;
        }

        case PRG_TYPE_INPUT: {
            const uint8_t* v = (uint8_t*)(p->value_ptr);
            LOG_INF("tpf(%s)=%s", p->name, io_input_func_to_value(*v));
            break;
        }

        case PRG_TYPE_OUTPUT: {
            const uint8_t* v = (uint8_t*)(p->value_ptr);
            LOG_INF("tpf(%s)=%s", p->name, io_output_func_to_value(*v));
            break;
        }

        default:
            LOG_INF("tpf(%s)=%d", p->name, *(int*)p->value_ptr);
            break;
        }
    }

    LOG_HEXDUMP_INF(rom_data, 64, "direct ROM of constants");


    flash_read(rom_flash_dev, TPF_FLASH_AREA_OFFSET, &buffer, sizeof(buffer));
    LOG_HEXDUMP_INF(buffer, 64, "readed TPF of constants");

    flash_erase(rom_flash_dev, TPF_FLASH_AREA_OFFSET, FLASH_ERASE_BLOCK_SIZE);

    for(int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = i;
    }

    LOG_INF("TPF Write data.");
    flash_write(rom_flash_dev, TPF_FLASH_AREA_OFFSET, &buffer, sizeof(buffer));

    flash_read(rom_flash_dev, TPF_FLASH_AREA_OFFSET, &buffer, sizeof(buffer));
    LOG_HEXDUMP_INF(buffer, 64, "readed TPF of constants");

}

static int init(void)
{
    return 0;
}

// TODO: Не знайшов я в Zephyr аналогу того рішення, яке було у трекерах
// Так шо будемо переносити моє. Але треба додати кільцевий буфер для
// зберігання даних, щоб не втрачати їх при пропаданні живлення

SYS_INIT(init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);


#include <stdio.h>
// #include "NuMicro.h"


#define APROM_TEST_BASE             0x80000
// #define APROM_TEST_END              APROM_TEST_BASE+0x4000
#define APROM_TEST_END              APROM_TEST_BASE+0x1000
#define TEST_PATTERN                0x5A5A5A5A

#if 0
void SYS_Init(void);
static int32_t FillDataPattern(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern);
static int32_t  VerifyData(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern);
static int32_t  FlashTest(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern);

int32_t FillDataPattern(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t u32Addr;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        if (FMC_Write(u32Addr, u32Pattern) != 0)          /* Program flash */
        {
            printf("FMC_Write address 0x%x failed!\n", u32Addr);
            return -1;
        }
    }
    return 0;
}


int32_t  VerifyData(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;
    uint32_t    u32Data;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        u32Data = FMC_Read(u32Addr);

        if (g_FMC_i32ErrCode != 0)
        {
            printf("FMC_Read address 0x%x failed!\n", u32Addr);
            return -1;
        }
        if(u32Data != u32Pattern)
        {
            printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr, u32Data, u32Pattern);
            return -1;
        }
    }
    return 0;
}
#endif

#if 0
static int32_t  FlashTest(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += FMC_FLASH_PAGE_SIZE)
    {
        printf("    Flash test address: 0x%x    \r", u32Addr);

        // Erase page
        if (FMC_Erase(u32Addr) != 0)
        {
            printf("FMC_Erase address 0x%x failed!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0)
        {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }

        // Write test pattern to fill the whole page
        if(FillDataPattern(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0)
        {
            printf("Failed to write page 0x%x!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all equal to test pattern
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0)
        {
            printf("\nData verify failed!\n ");
            return -1;
        }

        // Erase page
        if (FMC_Erase(u32Addr) != 0)
        {
            printf("FMC_Erase address 0x%x failed!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0)
        {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }
    }
    printf("\r    Flash Test Passed.          \n");
    return 0;
}
#endif

/*
 * This function will allow execute from sram region.  This is needed only for
 * this sample because by default all soc will disable the execute from SRAM.
 * An application that requires that the code be executed from SRAM will have
 * to configure the region appropriately in arm_mpu_regions.c.
 */
#ifdef CONFIG_ARM_MPU
#include <cmsis_core.h>
void disable_mpu_rasr_xn(void)
{
	uint32_t index;

	/*
	 * Kept the max index as 8(irrespective of soc) because the sram would
	 * most likely be set at index 2.
	 */
	for (index = 0U; index < 8; index++) {
		MPU->RNR = index;
#if defined(CONFIG_ARMV8_M_BASELINE) || defined(CONFIG_ARMV8_M_MAINLINE)
		if (MPU->RBAR & MPU_RBAR_XN_Msk) {
			MPU->RBAR ^= MPU_RBAR_XN_Msk;
		}
#else
		if (MPU->RASR & MPU_RASR_XN_Msk) {
			MPU->RASR ^= MPU_RASR_XN_Msk;
		}
#endif /* CONFIG_ARMV8_M_BASELINE || CONFIG_ARMV8_M_MAINLINE */
	}
}
#endif /* CONFIG_ARM_MPU */

// Функция обеспечивает:
// 1. Независимость регистра
// 2. Свободу разделителя: ., _,
// s1 всегда параметр из таблицы констант
#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))
int tpfCaseCmpN(const char *s1, const char *s2, unsigned int length)
{
    register const unsigned char *p1 = (const unsigned char *) s1;
    register const unsigned char *p2 = (const unsigned char *) s2;
    unsigned char c1, c2;

    if(p1==p2) return 0;

    do{
        c1 = *p1;
        c2 = *p2;

        if(c1 == '_') {
            if(c2 != '\0') c2 = c1;    // Доступен любой разделитель (кроме '\0')
                // ++p1;
                // ++p2;
                // continue;
        }

        c1 = TOLOWER (c1);
        c2 = TOLOWER (c2);

        if(c1 == '\0') break;
        if(--length == 0) return c1 - c2;

        ++p1;
        ++p2;
    } while (c1 == c2);

    return c1 - c2;
}

int tpfCaseCmp(const char *s1, const char *s2)
{
    return tpfCaseCmpN(s1, s2, TPF_MAX_NAME_LENGTH);
}


const struct tPrgParm *tpfGetElement(const char *name)
{
    STRUCT_SECTION_FOREACH(tPrgParm, p) {
        if(tpfCaseCmp(p->name, name) == 0) {
            return p;
        }
    }
    return (const struct tPrgParm *)0;
}

static bool is_not_default_value(const struct tPrgParm *p)
{
    switch(p->type) {
        case PRG_TYPE_BOOL:
        case PRG_TYPE_UINT8:
        case PRG_TYPE_INPUT:
        case PRG_TYPE_OUTPUT:
            return *(uint8_t*)p->value_ptr != *(uint8_t*)p->default_ptr;
        case PRG_TYPE_INT32:
        case PRG_TYPE_CURRENT:
            return *(int32_t*)p->value_ptr != *(int32_t*)p->default_ptr;
        case PRG_TYPE_UINT32:
        case PRG_TYPE_VOLTAGE:
        case PRG_TYPE_BITFIELD:
            return *(uint32_t*)p->value_ptr != *(uint32_t*)p->default_ptr;
        case PRG_TYPE_STR64:
            return strcmp((const char*)p->value_ptr, (const char*)p->default_ptr) != 0;
        case PRG_TYPE_TIME:
            const k_timeout_t* t = (k_timeout_t*)(p->value_ptr);
            const k_timeout_t* t_default = (k_timeout_t*)(p->default_ptr);
            uint32_t ms = k_ticks_to_ms_floor32(t->ticks);
            uint32_t ms_default = k_ticks_to_ms_floor32(t_default->ticks);
            return ms != ms_default;
            // return k_ticks_to_ms_floor32(*(k_timeout_t*)p->value_ptr) != k_ticks_to_ms_floor32(*(k_timeout_t*)p->default_ptr);
            // return true;
        default:
            LOG_ERR("Unknown type %d", p->type);
            return true;
    }
}

// Перетворюємо значення вигляду "a,b,e" в значення (1 << 0) | (1 << 1) | (1 << 4)
// a=0, b=1, c=2, d=3, e=4, f=5, g=6, h=7, ...
// Сприймаємо тільки a..z, решту символів пропускаємо (26 значень)
t_tpf_BITFIELD from_bit_field(const char* value)
{
    t_tpf_BITFIELD result = 0;
    while(*value) {
        if(isalpha(*value)) {
            result |= 1 << (tolower(*value) - 'a');
        }
        ++value;
    }
    return result;
}

// Зворотня функція до from_bit_field
// Перетворюємо значення вигляду (1 << 0) | (1 << 1) | (1 << 4) в "a,b,e"
// Виводимо тільки ті біти, які встановлені
void to_bit_field(t_tpf_BITFIELD value, char* buffer, size_t buffer_size)
{
    char* p = buffer;
    for(int i = 0; i < 26; i++) {
        if(value & (1 << i)) {
            if(p != buffer) {
                *p++ = ',';
            }
            // *p++ = '0' + i;
            *p++ = 'a' + i;
            // *p++ = ',';
        }
    }
    *p = '\0';
}

/*
Чи можна для типу даних список ("Вибір декількох значень") та ("Чекбокси") зробити так:

Якшо значення в квадратних дужках, то нехай працює як зараз - перелік значень через кому
"[a,b,c,d]", де a,b,c,d - це варіанти вибору з розділу "Варіанти вибору"
Або якшо з завершальною комою, то нехай і без дужок працює як зараз: "a,b,c,d,"

Якшо поле має цифрове значення, то це зачення сприймається як бітова маска. Тобто:

  0  =>  []
  1  =>  [0]
  2  =>  [1]
  3  =>  [0,1]
  7  =>  [0,1,2]
 15  =>  [0,1,2,3]
 28  =>  [2,3,4]
 31  =>  [0,1,2,3,4]
 63  =>  [0,1,2,3,4,5]
127  =>  [0,1,2,3,4,5,6]
255  =>  [0,1,2,3,4,5,6,7]

Тобто [a,b,...c]  <=>  (1 << a) | (1 << b) | .... (1 << b)

Переваги:
1. значення буде числове, а відповідно менше місця займатиме в json
2. послідовність значень буде строго однакова для всіх варіантів вибору
*/

#define FLASH_PAGE_SIZE 4096

bool tpfPatchInMem(const char *name, const char *value)
{
    LOG_DBG("tpfPatchInMem: %s=%s\n", name, value);
    const struct tPrgParm *iter_prgparm;
    const char **sub_vars;
    char sub_value[32];
    const char *p;
    int i;

    // unsigned int offset;
    // void * ramPointer;

    // const char **sub_vars;
    // char sub_value[32];
    // const char *p;
    // int i;
    uint8_t buf[FLASH_PAGE_SIZE];

    iter_prgparm = tpfGetElement(name);
    if(iter_prgparm == 0) {
        LOG_ERR("tpfPatchInMem: %s not found", name);
        return false;
    }
    LOG_DBG("tpfPatchInMem: [%s]=[%s] @0x%08x\n", iter_prgparm->name, value, (unsigned)iter_prgparm->value_ptr);

    // Copy data from flash to RAM. Aligned to FLASH_PAGE_SIZE
    uint32_t address = (uint32_t)iter_prgparm->value_ptr & ~(FLASH_PAGE_SIZE - 1);
    LOG_DBG("tpfPatchInMem: address=0x%08x", address);
    flash_read(rom_flash_dev, address, buf, FLASH_PAGE_SIZE);
    uint32_t offset = (uint32_t)iter_prgparm->value_ptr - address;
    LOG_DBG("tpfPatchInMem: offset=0x%08x", offset);
    void *ramPointer = (void *)(buf + offset);

    switch(iter_prgparm->type){
        case PRG_TYPE_BOOL:
            // false = 0, true = 1
            // DEBUG_PRINT("  >> Patch as BOOL");
            ((t_tpf_BOOL*)ramPointer)[0] = (strcmp(value, "true") == 0) ? 1 : 0;
            break;
        case PRG_TYPE_UINT8:	// 8мі бітне беззнакове
            ((t_tpf_UINT8*)ramPointer)[0] = atoi(value);
            break;
        // case PRG_TYPE_INT16:	// Значение, равное машинному слову, знаковое (для PIC32 - это 31 бит + знак)
        //     ((t_tpf_INT16*)ramPointer)[0] = atol(value);
        //     break;
        // case PRG_TYPE_UINT16:	// Значение, равное машинному слову, беззнаковое (для PIC32 - это 32 бита)
        //     ((t_tpf_UINT16*)ramPointer)[0] = atol(value);
        //     break;
        case PRG_TYPE_INT32:	// Значение, равное машинному слову, знаковое (для PIC32 - это 31 бит + знак)
            ((t_tpf_INT32*)ramPointer)[0] = atoll(value);
            break;
        case PRG_TYPE_UINT32:	// Значение, равное машинному слову, беззнаковое (для PIC32 - это 32 бита)
            ((t_tpf_UINT32*)ramPointer)[0] = atoll(value);
            break;
        // case PRG_TYPE_INT64:	// Значение 64 бит знаковое
        //     // DEBUG_PRINT("  >> Patch as INT64");
        //     ((t_tpf_INT64*)ramPointer)[0] = atoll(value);
        //     break;
        // case PRG_TYPE_STR16:	// Строковое значение (до 16ти символов)
        //     // DEBUG_PRINT("  >> Patch as STR16");
        //     strncpy((char *)ramPointer, value, 16);
        //     break;
        case PRG_TYPE_STR64:	// Строковое значение (до 64х символов)
            // DEBUG_PRINT("  >> Patch as STR64");
            strncpy((char *)ramPointer, value, 64);
            break;
        // case PRG_TYPE_BOOL:		// Булевое значение (1 бит) * Я пока не придумал механизма хранения значений
        //     DEBUG_PRINT("  >> Patch as BOOL");
        //     ((t_tpf_INT32*)ramPointer)[0] = atoi(value);
        //     break;
        case PRG_TYPE_TIME_MS:		// Значение, используется в таймерах, интервалах времени и т.п.
            // DEBUG_PRINT("  >> Patch as TIME");
            // ((t_tpf_TIME_MS*)ramPointer)[0] = (t_tpf_TIME)(atof(value) * 32.0 + 0.0);
            LOG_ERR("PRG_TYPE_TIME_MS not implemented");
            break;
        // case PRG_TYPE_PIN4:
        //     // DEBUG_PRINT("  >> Patch as PIN4");
        //     ((t_tpf_UINT32*)ramPointer)[0] = strtoul(value, NULL, 16);  //  Странно что strtoll меньше места занимает чем strtoul
        //     break;
		// case PRG_TYPE_SIGNALS:
		// 	// DEBUG_PRINT("  >> Patch as SIGNALS");
		// 	if(strcasecmp(value, "off") == 0){
		// 		((t_tpf_SIGNALS*)ramPointer)[0] =  PRG_SIGNALS_OFF;
		// 	} else if(strcasecmp(value, "lights") == 0){
		// 		((t_tpf_SIGNALS*)ramPointer)[0] =  PRG_SIGNALS_LIGHTS;
		// 	} else if(strcasecmp(value, "siren") == 0){
		// 		((t_tpf_SIGNALS*)ramPointer)[0] =  PRG_SIGNALS_SIREN;
		// 	} else if(strcasecmp(value, "both") == 0){
		// 		((t_tpf_SIGNALS*)ramPointer)[0] =  PRG_SIGNALS_BOTH;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_ALERTS:
		// 	// DEBUG_PRINT("  >> Patch as ALERTS");
		// 	if(strcasecmp(value, "off") == 0){
		// 		((t_tpf_ALERTS*)ramPointer)[0] =  PRG_ALERTS_OFF;
		// 	} else if(strcasecmp(value, "sms") == 0){
		// 		((t_tpf_ALERTS*)ramPointer)[0] =  PRG_ALERTS_SMS;
		// 	} else if(strcasecmp(value, "call") == 0){
		// 		((t_tpf_ALERTS*)ramPointer)[0] =  PRG_ALERTS_CALL;
		// 	} else if(strcasecmp(value, "both") == 0){
		// 		((t_tpf_ALERTS*)ramPointer)[0] =  PRG_ALERTS_BOTH;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_INPUT:
		// 	// DEBUG_PRINT("  >> Patch as INPUT");
		// 	if(strcasecmp(value, "off") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_OFF;
		// 	} else if(strcasecmp(value, "kofr1") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_KOFR1;
		// 	} else if(strcasecmp(value, "kofr2") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_KOFR2;
		// 	} else if(strcasecmp(value, "stand") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_STAND;
		// 	// } else if(strcasecmp(value, "brake") == 0){
		// 	// 	((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_BRAKE;
		// 	} else if(strcasecmp(value, "zone1") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_ZONE1;
		// 	} else if(strcasecmp(value, "zone2") == 0){
		// 		((t_tpf_INPUT*)ramPointer)[0] = INPUTS_FUN_ZONE2;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_INPUT_CON:
		// 	// DEBUG_PRINT("  >> Patch as INPUT_CON");
		// 	if(strcasecmp(value, "nc") == 0){
		// 		((t_tpf_INPUT_CON*)ramPointer)[0] = INPUTS_CON_NC;
		// 	} else if(strcasecmp(value, "no") == 0){
		// 		((t_tpf_INPUT_CON*)ramPointer)[0] = INPUTS_CON_NO;
		// 	} else if(strcasecmp(value, "p") == 0){
		// 		((t_tpf_INPUT_CON*)ramPointer)[0] = INPUTS_CON_P;
		// 	} else if(strcasecmp(value, "n") == 0){
		// 		((t_tpf_INPUT_CON*)ramPointer)[0] = INPUTS_CON_N;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_INPUT_IMP:
		// 	// DEBUG_PRINT("  >> Patch as INPUT_IMP");
		// 	if((*value == '\0') || (strcasecmp(value, "0") == 0)){
		// 		((t_tpf_INPUT_IMP*)ramPointer)[0] = INPUTS_IMP_0K;
		// 	} else if(strcasecmp(value, "r") == 0){
		// 		((t_tpf_INPUT_IMP*)ramPointer)[0] = INPUTS_IMP_100K;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_OUTPUT:
		// 	// DEBUG_PRINT("  >> Patch as OUTPUT");
		// 	if(strcasecmp(value, "off") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_OFF;
		// 	} else if(strcasecmp(value, "siren") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_SIREN;
		// 	} else if(strcasecmp(value, "lights") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_LIGHTS;
		// 	} else if(strcasecmp(value, "disable") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_DISABLE;
		// 	} else if(strcasecmp(value, "enable") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_ENABLE;
		// 	} else if(strcasecmp(value, "disablei") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_DISABLE_I;
		// 	} else if(strcasecmp(value, "enablei") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_ENABLE_I;
		// 	} else if(strcasecmp(value, "horn") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_SIGNAL;
		// 	} else if(strcasecmp(value, "channel1") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_DOP_1;
		// 	} else if(strcasecmp(value, "channel2") == 0){
		// 		((t_tpf_OUTPUT*)ramPointer)[0] = OUTS_FUN_DOP_2;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		// case PRG_TYPE_ACT:
		// 	// DEBUG_PRINT("  >> Patch as ACT");
		// 	if(strcasecmp(value, "off") == 0){
		// 		((t_tpf_ACT*)ramPointer)[0] =  PRG_ACT_OFF;
		// 	} else if(strcasecmp(value, "arm") == 0){
		// 		((t_tpf_ACT*)ramPointer)[0] =  PRG_ACT_ARM;
		// 	} else if(strcasecmp(value, "disarm") == 0){
		// 		((t_tpf_ACT*)ramPointer)[0] =  PRG_ACT_DISARM;
		// 	} else if(strcasecmp(value, "both") == 0){
		// 		((t_tpf_ACT*)ramPointer)[0] =  PRG_ACT_BOTH;
		// 	} else {
		// 		return false;
		// 	}
		// 	break;
		case PRG_TYPE_VOLTAGE:		// Значение, используется в зарядке, в режимах потребления, и т.д.
            // DEBUG_PRINT("  >> Patch as VOLTAGE");
            ((t_tpf_VOLTAGE*)ramPointer)[0] = (t_tpf_VOLTAGE)(atof(value) * 1000.0);
            break;

        case PRG_TYPE_CURRENT:		// Значение, используется в зарядке, в режимах потребления, и т.д.
            // DEBUG_PRINT("  >> Patch as CURRENT (%d -> %d)", ((t_tpf_CURRENT*)ramPointer)[0], (t_tpf_CURRENT)(atof(value) * 1000.0));
            ((t_tpf_CURRENT*)ramPointer)[0] = (t_tpf_CURRENT)(atof(value) * 1000.0);
            break;

		// case PRG_TYPE_TEMPERATURE:
        //     ((t_tpf_TEMPERATURE*)ramPointer)[0] = (t_tpf_TEMPERATURE)(atof(value) * 10.0 + 0.5);
        //     break;
		// case PRG_TYPE_TILTANGLE: {		// Значение, используется в датчике наклона.
        //     ((t_tpf_TILTANGLE*)ramPointer)[0] = (t_tpf_TILTANGLE)(atof(value) * 10.0 + 0.5);
        //     break; }
        case PRG_TYPE_INPUT:
            ((t_tpf_INPUT*)ramPointer)[0] = io_input_value_to_func(value);
            break;
        case PRG_TYPE_OUTPUT:
            ((t_tpf_OUTPUT*)ramPointer)[0] = io_output_value_to_func(value);
            break;
        case PRG_TYPE_TIME:
            LOG_ERR("TPF: TPF_CONV_TIME not implemented");
            float f = atof(value);
            ((t_tpf_TIME*)ramPointer)[0] = K_MSEC((int)(f*1000.0f));
            break;

        case PRG_TYPE_BITFIELD:	// Значення типу "a,b,c" => (1<<0)|(1<<1)|(1<<4)
            ((t_tpf_BITFIELD*)ramPointer)[0] = from_bit_field(value);
            break;


        case PRG_TYPE_META:
            //  TODO:
            sub_vars = (const char **)iter_prgparm->value_ptr;
            p = value;
            while(*sub_vars){
                // Копируем часть значения до пробела (или конца строки)
                i = 0;
                while((*p != ' ') && (*p != '\0') && (i < (sizeof(sub_value) - 1) )){
                    sub_value[i] = *p;
                    p++;
                    i++;
                }
                sub_value[i] = '\0';
                if(*p == ' ') p++;
				if(!tpfPatchInMem(*sub_vars, sub_value)) {
					return false;
				}
                sub_vars++;
            }

            break;
    }

    // Write data back to flash
    flash_erase(rom_flash_dev, address, FLASH_PAGE_SIZE);
    flash_write(rom_flash_dev, address, buf, FLASH_PAGE_SIZE);

	return true;
}

#include <zephyr/sys/printk.h>

// extern void function_in_sram(void);

#if 0
static int test_tmc(void)
{
    LOG_DBG("test_tmc\n");

	// disable_mpu_rasr_xn();
    // uint32_t address = FMC_DTFSH_BASE;
    // uint8_t buf[FMC_FLASH_PAGE_SIZE];
    // flash_read(rom_flash_dev, address, buf, FMC_FLASH_PAGE_SIZE);

    // LOG_HEXDUMP_DBG(buf, 16, "readed TMC of constants");


    LOG_DBG("\nFMC Sample Code Completed.\n");

    return 0;
}
#endif
// SYS_INIT(test_tmc, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_OBJECTS);

// SYS_INIT(test_tmc, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);


#include <sys/types.h>
#include "zephyr/kernel.h"
#include "zephyr/data/json.h"

// TODO: Move to global config
// #define CFG_VERSION "y.0.6"

struct data_item {
    const char *k;
    const char *v;
};

// TODO: Use incompile calcilation of tpf parametes count
#define MAX_DATA_ITEMS 2000

struct publish_payload {
    const char *version;
    const char *hardware;
    uint32_t timestamp;
    struct data_item data[MAX_DATA_ITEMS];
    size_t data_len;
};

static const struct json_obj_descr json_data_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct data_item, k, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct data_item, v, JSON_TOK_STRING),
};

static const struct json_obj_descr json_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct publish_payload, version, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct publish_payload, hardware, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct publish_payload, timestamp, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_OBJ_ARRAY(struct publish_payload, data, MAX_DATA_ITEMS,
        data_len, json_data_descr, ARRAY_SIZE(json_data_descr)),
};


// (const tPrgParm *iter_prgparm, bool def, char * dest, unsigned int max_length)
// const struct tPrgParm *
int tpf_item_to_string(const struct tPrgParm *iter_prgparm, char* buf, size_t len, bool def)
{
    if(len) buf[0] = '\0';
    const void *value_ptr = def ? (iter_prgparm->default_ptr) : (iter_prgparm->value_ptr);
    switch(iter_prgparm->type){
        case PRG_TYPE_BOOL:
            snprintf(buf, len, "%s", ((t_tpf_BOOL*)value_ptr)[0] ? "true" : "false");
            break;
        case PRG_TYPE_UINT8:	// 8мі бітне беззнакове
            snprintf(buf, len, "%u", ((t_tpf_UINT8*)value_ptr)[0]);
            break;
        case PRG_TYPE_INT32:	// Значение, равное машинному слову, знаковое (для PIC32 - это 31 бит + знак)
            snprintf(buf, len, "%d", ((t_tpf_INT32*)value_ptr)[0]);
            break;
        case PRG_TYPE_UINT32:	// Значение, равное машинному слову, беззнаковое (для PIC32 - это 32 бита)
            snprintf(buf, len, "%u", ((t_tpf_UINT32*)value_ptr)[0]);
            break;
        case PRG_TYPE_STR64:	// Строковое значение (до 64х символов)
            snprintf(buf, len, "%s", (char *)value_ptr);
            break;
        case PRG_TYPE_TIME_MS:		// Значение, используется в таймерах, интервалах времени и т.п.
            snprintf(buf, len, "%d", ((t_tpf_TIME_MS*)value_ptr)[0]);
            break;
        case PRG_TYPE_TIME: {
            const k_timeout_t* t = (k_timeout_t*)(value_ptr);
            uint32_t ms = k_ticks_to_ms_floor32(t->ticks);
            // LOG_DBG("tpf_item_to_string: %d", ms);
            snprintf(buf, len, "%.3f", ms/1000.0);
            // snprintf(buf, len, "%d", ms);
            break;
        }
        case PRG_TYPE_VOLTAGE:		// Значение, используется в зарядке, в режимах потребления, и т.д.
            snprintf(buf, len, "%d", ((t_tpf_VOLTAGE*)value_ptr)[0]);
            break;
        case PRG_TYPE_CURRENT:		// Значение, используется в зарядке, в режимах потребления, и т.д.
            snprintf(buf, len, "%d", ((t_tpf_CURRENT*)value_ptr)[0]);
            break;
        case PRG_TYPE_INPUT:
            snprintf(buf, len, "%s", io_input_func_to_value(((enum io_input_func*)value_ptr)[0]));
            break;
        case PRG_TYPE_OUTPUT:
            snprintf(buf, len, "%s", io_output_func_to_value(((enum io_output_func*)value_ptr)[0]));
            break;
        case PRG_TYPE_BITFIELD:	// Значення типу "a,b,d" => (1<<0)|(1<<1)|(1<<4)
            to_bit_field(((t_tpf_BITFIELD*)value_ptr)[0], buf, len);
            break;
        default:
            return -1;
    }
    return 0;
}

int tpf_dump_as_json(char *buffer, size_t buffer_size, bool include_defaults, uint32_t timestamp)
{
    struct publish_payload* p = k_malloc(sizeof(struct publish_payload));

    if(p == NULL) {
        LOG_ERR("tpf_dump_as_json: k_alloc failed");
        return -1;
    }

    p->version = CONFIG_APP_CONFIG_VERSION;
    p->hardware = CONFIG_APP_VERSION_STRING; // "fx500-02";
    p->timestamp = timestamp;

    #define STRINGS_REPR_SIZE 8096

    // buffer memory for string representations
    char *strs = (char*)k_malloc(STRINGS_REPR_SIZE);
    if(strs == NULL) {
        k_free(p);
        return -1;
    }
    char *s = strs;
    size_t si = STRINGS_REPR_SIZE-1;  // Available data + \0

    int i = 0;
    STRUCT_SECTION_FOREACH(tPrgParm, prg) {
        if(!include_defaults && !is_not_default_value(prg)) {
            continue;
        }
        int ret = tpf_item_to_string(prg, s, si, false);
        if(ret < 0) {
            LOG_ERR("tpf_item_to_string (%s) failed. Skiped.", prg->name);
            continue;
        }
        p->data[i].k = prg->name;
        p->data[i].v = s;
        si -= strlen(s) + 1;
        s += strlen(s) + 1;
        i++;
    }
    p->data_len = i;

    // LOG_ERR("tpf_dump_as_json: data_len=%d", p->data_len);

    int ret = json_obj_encode_buf(
        json_descr, ARRAY_SIZE(json_descr),
        p, buffer, buffer_size);

    k_free(strs);
    k_free(p);
    return ret;
}
