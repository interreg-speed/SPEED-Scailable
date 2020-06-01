file(GLOB_RECURSE M3_SOURCES "m3/*.c")

set(APP_SOURCES "m3_api_esp_wasi.c" "app_main.cpp")

idf_component_register(SRCS ${APP_SOURCES} ${M3_SOURCES}
                       INCLUDE_DIRS ".")

# Added -Dd_m3MaxFunctionStackHeight=256 
target_compile_options(${COMPONENT_LIB} PRIVATE -w -Wno-error -O3 -ffast-math -DESP32 -Dd_m3MaxFunctionStackHeight=356 -Dd_m3LogOutput=false -DBUILD_WASI=uvwasi )

# Disable harmless warnings
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-missing-field-initializers)
