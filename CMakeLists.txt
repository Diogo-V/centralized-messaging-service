cmake_minimum_required(VERSION 3.19)
project(Projeto_RC_39_v2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server
        server/src/main.cpp
        server/src/models/user.cpp
        server/src/models/user.h
        server/src/models/message.cpp
        server/src/models/message.h
        server/src/models/group.cpp
        server/src/models/group.h
        server/src/api.h
        server/src/api.cpp
        server/src/models/connect.cpp
        server/src/models/connect.h
        server/src/models/manager.cpp
        server/src/models/manager.h
        server/src/misc/helpers.h
        server/src/misc/helpers.cpp
)

add_executable(Client
        client/src/main.cpp
        client/src/models/manager.cpp
        client/src/models/manager.h
        client/src/misc/helpers.cpp
        client/src/misc/helpers.h
        client/src/models/user.cpp
        client/src/models/user.h
        client/src/models/connect.cpp
        client/src/models/connect.h
)
