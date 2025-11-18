zephyr_include_directories(include)
zephyr_include_directories(include/tpf)
add_subdirectory_ifdef(CONFIG_TPF_MODULE src)

# TODO: Remove this later
# include(${ZEPHYR_BASE}/samples/net/common/common.cmake)
