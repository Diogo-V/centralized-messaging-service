#ifndef PROJETO_RC_39_V2_CONNECT_H
#define PROJETO_RC_39_V2_CONNECT_H

#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>


using namespace std;


/**
 * Performs a connection (by udp or tcp) to our server and gets a response.
 */
class Connect {

    private:

        /**
         * @brief File descriptor for a tcp connection.
         */
        int _fd_tcp;

        /**
         * @brief File descriptor for a udp connection.
         */
        int _fd_udp;

    private:

        /**
         * @brief Setups our udp socket.
         */
        void init_socket_udp(const string& ds_ip, const string& ds_port);

        /**
         * @brief Setups our tcp socket.
         */
        void init_socket_tcp(const string& ds_ip, const string& ds_port);

    public:

        /**
         * @brief Connect class constructor.
         *
         * @param ip ip of the server
         * @param port port of the server
         */
        explicit Connect(char* ip, char* port);

};

#endif
