zephyr_include_directories(include)
add_subdirectory_ifdef(CONFIG_TPF_API, src)

# TODO: Remove this later
# include(${ZEPHYR_BASE}/samples/net/common/common.cmake)
