cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(rp-midi C CXX ASM)
message(STATUS "Project Name: ${PROJECT_NAME}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()


add_executable(${PROJECT_NAME} src/main.c usb_descriptors.c)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
pico_add_extra_outputs(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib tinyusb_board tinyusb_device)