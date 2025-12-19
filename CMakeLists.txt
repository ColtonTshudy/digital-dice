cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME lcd-stm32f4)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Compile Flags
string(APPEND CMAKE_EXE_LINKER_FLAGS "-u _printf_float")

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB GRAPHICS_SOURCES 
    "${CMAKE_HOME_DIRECTORY}/External/Graphics/stm32f4xx/*.c"
    "${CMAKE_HOME_DIRECTORY}/External/Graphics/*.c"
    "${CMAKE_HOME_DIRECTORY}/External/Graphics/lcd/ili9341/*.c"
    "${CMAKE_HOME_DIRECTORY}/External/Graphics/Fonts/*.c"
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${GRAPHICS_SOURCES}
    ${CMAKE_HOME_DIRECTORY}/Src/debugger.c
    # ${CMAKE_HOME_DIRECTORY}/External/Graphics/App/TouchCalib/appTouchCalib.c
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/App/Paint/appPaint.c
    ${CMAKE_HOME_DIRECTORY}/Src/app_main.c
    ${CMAKE_HOME_DIRECTORY}/Src/app_stats.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_HOME_DIRECTORY}/External/Graphics
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/stm32f4xx
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/Fonts
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/App
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/lcd/ili9341
    ${CMAKE_HOME_DIRECTORY}/External/Graphics/lcd/ili9341
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
