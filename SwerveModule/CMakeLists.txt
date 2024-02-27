add_library(SwerveModule "" SwerveModule.cpp SwerveModule.h)
target_include_directories(SwerveModule PUBLIC .)
target_link_libraries(SwerveModule Vesc)