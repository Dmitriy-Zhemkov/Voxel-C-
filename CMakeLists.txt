cmake_minimum_required(VERSION 3.20)
project(CubeWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к сторонним библиотекам (можно заменить find_package)
# extern/ директория больше не используется — libы приходят из vcpkg

add_compile_definitions(SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders\")


file(GLOB_RECURSE SRC src/*.cpp src/*.h)
add_executable(cubeworld ${SRC})

# Линкуем GLFW, GLEW, GLM (пример для add_subdirectory)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(cubeworld PRIVATE glfw GLEW::GLEW glm::glm)

# Включаем путь к заголовкам Engine
target_include_directories(cubeworld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Под Windows добавляем опцию для консольного окна
if (WIN32)
    target_link_libraries(cubeworld PRIVATE opengl32)
endif()