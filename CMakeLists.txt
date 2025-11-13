cmake_minimum_required(VERSION 3.25)
project(explore C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
add_executable(version version.c)
target_include_directories(version
    PRIVATE /Users/balakrishna/source/libuv/libuv/include/)
target_link_directories(version
  PRIVATE /Users/balakrishna/source/libuv/libuv/build/)

target_link_libraries(version libuv.a)


add_executable(timer timer.c)
target_include_directories(timer
    PRIVATE /Users/balakrishna/source/libuv/libuv/include/)
target_link_directories(timer
  PRIVATE /Users/balakrishna/source/libuv/libuv/build/)

target_link_libraries(timer libuv.a)


add_executable(echo-server echo-server.c)
target_include_directories(echo-server
    PRIVATE /Users/balakrishna/source/libuv/libuv/include/)
target_link_directories(echo-server
  PRIVATE /Users/balakrishna/source/libuv/libuv/build/)

if(ENABLE_ASAN)
  target_compile_options(echo-server PRIVATE -fsanitize=address)
  target_link_options(echo-server PRIVATE -fsanitize=address)
endif()
target_link_libraries(echo-server libuv.a)

add_executable(client client.c)
target_include_directories(client
    PRIVATE /Users/balakrishna/source/libuv/libuv/include/)
target_link_directories(client
  PRIVATE /Users/balakrishna/source/libuv/libuv/build/)

if(ENABLE_ASAN)
  target_compile_options(client PRIVATE -fsanitize=address)
  target_link_options(client PRIVATE -fsanitize=address)
endif()

target_link_libraries(client libuv.a)


add_executable(server server-for-client.c)
target_include_directories(server
    PRIVATE /Users/balakrishna/source/libuv/libuv/include/)
target_link_directories(server
  PRIVATE /Users/balakrishna/source/libuv/libuv/build/)

if(ENABLE_ASAN)
  target_compile_options(server PRIVATE -fsanitize=address)
  target_link_options(server PRIVATE -fsanitize=address)
endif()
target_link_libraries(server libuv.a)
