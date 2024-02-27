cmake_minimum_required(VERSION 3.5)
project(swerve-testing)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

add_executable(swerve-testing main.cpp)

add_subdirectory(Vesc)
add_subdirectory(SwerveModule)
target_link_libraries(swerve-testing PUBLIC Vesc SwerveModule)
