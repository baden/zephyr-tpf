zephyr_library()

zephyr_library_sources(tpf.c io.c)

zephyr_linker_sources(SECTIONS sections-rom.ld)
# zephyr_linker_sources(RODATA sections-rom.ld)
zephyr_iterable_section(
    NAME tPrgParm
    KVMA RAM_REGION
    GROUP RODATA_REGION
    SUBALIGN CONFIG_LINKER_ITERABLE_SUBALIGN
)
