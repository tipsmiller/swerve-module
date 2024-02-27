add_library(Vesc ""
        VescCan.cpp
        VescCan.h
        CanSocket.cpp
        CanSocket.h
        buffer.cpp
        buffer.h
        datatypes.h
)
target_include_directories(Vesc PUBLIC .)
target_link_libraries(Vesc -latomic)
