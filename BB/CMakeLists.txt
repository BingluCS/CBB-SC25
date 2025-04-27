cmake_minimum_required(VERSION 3.10)
project(BurstBuffer)
set(CMAKE_CXX_STANDARD 11)

message(STATUS "Install directory: ${CMAKE_INSTALL_PREFIX}")

add_library(bb SHARED BurstBuffer.cpp)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/lib/libbb.so
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/include/BurstBuffer.h
    COMMENT "Deleting installed files..."
)

install(TARGETS bb DESTINATION lib)
install(FILES BurstBuffer.h DESTINATION include)