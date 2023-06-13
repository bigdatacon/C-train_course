# напишите CMakeLists.txt для сборки программы
# добавьте одну цель - negate
# она зависит от файлов main.cpp, img_lib.h, img_lib.cpp, ppm_image.c, ppm_image.cpp
cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable("hello_cmake.exe" "main.cpp" "img_lib.h" "img_lib.cpp" "ppm_image.h" "ppm_image.cpp")
