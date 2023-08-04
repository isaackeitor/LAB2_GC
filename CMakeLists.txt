cmake_minimum_required(VERSION 3.26.4)

project(SR VERSION 1.0)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

# Configurar SDL2 si lo estás utilizando
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
