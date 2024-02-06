cmake_minimum_required(VERSION 3.10)

project(bare_oled)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 23)

list(APPEND SRC_FILES 
    src/main.cpp
    src/display/display_font.cpp
    src/display/icons/display_icons_misc.cpp
    src/tools/bare_defs.cpp
    )

add_executable(bare_oled ${SRC_FILES})

#target_link_libraries(bare_oled bcm2835)
