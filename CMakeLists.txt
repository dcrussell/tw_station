cmake_minimum_required(VERSION 3.13)
project("TinyWeather" C)

set(CMAKE_C_STANDARD 11)

#add_subdirectory(external)
#add_subdirectory(test)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This is a hack to get CLion to recognize that
# iom328p.h is included in the io.h. As far as I know,
# this won't do anything when compiling for off target
# using gcc.
add_definitions(
        -D__AVR_ATmega328P__
        -DF_CPU=1000000
)
add_compile_options(
        -I/usr/lib/avr/include
)
file(GLOB SRC_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})
