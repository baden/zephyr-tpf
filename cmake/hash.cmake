# Функція для обчислення FNV-1a хешу
function(fnv1a_32_hash input_string output_var)
    set(FNV_PRIME_32 16777619)
    set(FNV_OFFSET_32 2166136261)
    string(LENGTH "${input_string}" str_len)
    set(hash ${FNV_OFFSET_32})
    
    math(EXPR str_max "${str_len} - 1")
    foreach(i RANGE 0 ${str_max})
        string(SUBSTRING "${input_string}" ${i} 1 char)
        if(char)
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -c "print(ord('${char}'))"
                OUTPUT_VARIABLE char_code
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NOT char_code MATCHES "^[0-9]+$")
                message(FATAL_ERROR "Не вдалося отримати валідний ASCII-код для символу '${char}' у строці '${input_string}'")
            endif()
            math(EXPR hash "(${hash} ^ ${char_code}) * ${FNV_PRIME_32}" OUTPUT_FORMAT DECIMAL)
            math(EXPR hash "${hash} & 0xFFFFFFFF" OUTPUT_FORMAT DECIMAL)
        endif()
    endforeach()
    
    math(EXPR hex_hash "${hash}" OUTPUT_FORMAT HEXADECIMAL)
    set(${output_var} ${hex_hash} PARENT_SCOPE)
endfunction()



function(generatehash_file)
    # Генерація заголовочного файлу
    # TODO: Винести в функцію в cmake/hash.cmake
    set(GENERATED_HEADER "${CMAKE_BINARY_DIR}/generated_names.h")
    file(WRITE ${GENERATED_HEADER} "/* Автозгенерований файл */\n#ifndef GENERATED_NAMES_H\n#define GENERATED_NAMES_H\n\n")

    set(HASH_MAP "")
    foreach(str ${GLOBAL_STRINGS_TO_HASH})
        fnv1a_32_hash("${str}" hash_value)
        
        string(REGEX REPLACE "0x" "" hash_decimal "${hash_value}")
        math(EXPR hash_decimal "0x${hash_decimal}" OUTPUT_FORMAT DECIMAL)
        if(DEFINED HASH_MAP_${hash_decimal})
            message(FATAL_ERROR "Колізія хешу: '${str}' та '${HASH_MAP_${hash_decimal}}' мають однаковий хеш ${hash_value}")
        endif()
        
        set(HASH_MAP_${hash_decimal} "${str}")
        list(APPEND HASH_MAP "${hash_decimal}")
        
        string(REPLACE "." "_" sanitized_str "${str}")
        file(APPEND ${GENERATED_HEADER} "#define NAME_${sanitized_str} ${hash_value}U // ${str}\n")
    endforeach()

    file(APPEND ${GENERATED_HEADER} "\n#endif /* GENERATED_NAMES_H */\n")
endfunction(generatehash_file)

