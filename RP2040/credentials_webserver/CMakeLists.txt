
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

#function(suppress_tinyusb_warnings)
#        set_source_files_properties(
#                ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/rp2040/rp2040_usb.c
#                PROPERTIES
#                COMPILE_FLAGS "-Wno-stringop-overflow -Wno-array-bounds")
#endfunction()

project(credentials_webserver C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )        

add_executable(credentials_webserver
        src/main.cpp
        src/dhcpserver.c
        src/credentials_webserver.cpp
        src/storage_handler.cpp
        src/log.cpp
        )        

target_include_directories(credentials_webserver PRIVATE        
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

target_link_libraries(credentials_webserver PUBLIC
        cyw43_driver_base
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        )

pico_enable_stdio_usb(credentials_webserver FALSE)
pico_enable_stdio_uart(credentials_webserver TRUE)

#suppress_tinyusb_warnings()

pico_add_extra_outputs(credentials_webserver)
