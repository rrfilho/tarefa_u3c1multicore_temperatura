# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")
cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(pico_sdk_import.cmake)
project(main C CXX ASM)
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/sensors)

add_executable(
    ${PROJECT_NAME} 
    main.c
    libs/bootsel.c libs/leds.c libs/display.c libs/threads.c
    sensors/i2c_interface.c sensors/humidity_sensor.c sensors/pressure_sensor.c
)

target_link_libraries(
    ${PROJECT_NAME} 
    pico_stdlib
    pico_multicore
    hardware_i2c
)

# Add the standard include files to the build
target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})

