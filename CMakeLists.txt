cmake_minimum_required(VERSION 3.7.0)

project(sharedmemory)

set (CMAKE_C_FLAGS "-D_GNU_SOURCE -Wall -std=c99")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)

#build lib server
add_library(server "")

target_sources(server PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib/server.c   
  )

target_include_directories(server PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib
  )

#build lib client
add_library(client "")

target_sources(client PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib/client.c   
  )

target_include_directories(client PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib
  )

#build led_process
add_executable(led_process "")

target_link_libraries(led_process PUBLIC server hardware)

target_sources(led_process
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/led_process.c
  )

target_include_directories(led_process 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib  
  )

install(TARGETS led_process DESTINATION bin)

#built_button_process
add_executable(button_process "")

target_link_libraries(button_process PUBLIC client hardware)

target_sources(button_process
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/button_process.c
  )

target_include_directories(button_process 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib  
  )
  
install(TARGETS button_process DESTINATION bin)

#build launch_processes
add_executable(launch_processes "")

target_sources(launch_processes
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/launch_processes.c
  )
 
install(TARGETS launch_processes DESTINATION bin)