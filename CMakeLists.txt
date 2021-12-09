cmake_minimum_required(VERSION 3.19)
project(Projeto_RC_39_v2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server_UD
        server/src/main.cpp
        server/src/models/user.cpp
        server/src/models/user.h
        server/src/models/message.cpp
        server/src/models/message.h
        server/src/models/group.cpp
        server/src/models/group.h
        server/src/api.cpp
        server/src/api.h
)

add_executable(Client client/src/main.cpp client/src/models/user.cpp client/src/models/user.h)
