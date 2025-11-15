cmake_minimum_required(VERSION 3.16)
project(restaurant_booking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(restaurant_booking
    src/main.cpp
    src/ReservationSystem.cpp
)

target_include_directories(restaurant_booking PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

