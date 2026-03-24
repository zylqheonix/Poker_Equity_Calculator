cmake_minimum_required(VERSION 3.16)

project(PokerSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(poker_core
    card.cpp
    range.cpp
    evaluator.cpp
    simulator.cpp
)

target_include_directories(poker_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(poker_server
    server_main.cpp
)

target_include_directories(poker_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
)

target_link_libraries(poker_server
    PRIVATE poker_core
)
