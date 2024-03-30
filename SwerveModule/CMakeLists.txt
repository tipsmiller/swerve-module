add_library(SwerveModule "" SwerveModule.cpp SwerveModule.h
        SwerveKinematics.h)
target_include_directories(SwerveModule PUBLIC .)
target_link_libraries(SwerveModule Vesc)