cmake_minimum_required(VERSION 3.20)

# Окремий CMake-проєкт для домашньої роботи 03. Його можна збирати як
# через root CMakeLists.txt, так і напряму з цієї директорії.
project(multi_target CXX)

# Код домашньої роботи збирається з тим самим стандартом, що й лекційні приклади.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Один executable для вибору боєприпасу й точки скиду за сценарієм із файлу.
add_executable(multi_target src/main.cpp)

# Базові compiler warnings допомагають побачити підозрілі місця до runtime.
target_compile_options(multi_target PRIVATE -Wall -Wextra -pedantic)
