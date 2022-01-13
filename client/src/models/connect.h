#ifndef PROJETO_RC_39_V2_CONNECT_H
#define PROJETO_RC_39_V2_CONNECT_H

#include "../misc/helpers.h"

#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fstream>

#define TIMEOUT_TIME_S 15
#define UDP_N_TRIES 3
#define MAX_SENT_FILE_DATA 300


using namespace std;


/**
 * Performs a connection (by udp or tcp) to our server and gets a response.
 */
class Connect {

    private:

        /**
         * @brief File descriptor for a tcp connection.
         */
        int _fd_tcp{};

        /**
         * @brief File descriptor for a udp connection.
         */
        int _fd_udp{};

        /**
         * @brief Server's ip address.
         */
        string _ip;

        /**
         * @brief Server's port.
         */
        string _port;

        /**
         * @brief Stores result from getaddrinfo and uses it to set up our socket
         */
        struct addrinfo *res{};

    private:

        /**
         * @brief Setups our udp socket.
         */
        void init_socket_udp();

        /**
         * @brief Setups our tcp socket.
         */
        void init_socket_tcp();

        /**
         * @brief Activates a timer to wait about 5 seconds. It is used as a measure of backup in case UDP
         * is not able to send the message to our server.
         *
         * @param sd socket to be timed
         *
         * @return manipulates socket options
         */
        static int TimerON(int sd);

        /**
         * @brief Disables a timer previously activated for an input socket.
         *
         * @param sd socket that was timed
         *
         * @return manipulates socket options
         */
        static int TimerOFF(int sd);

    public:

        /**
         * @brief Connect class constructor.
         *
         * @param ip ip of the server
         * @param port port of the server
         */
        explicit Connect(const string& ip, const string& port);


        /**
         * @brief Gets server's id address.
         *
         * @return server's ip address
         */
        string getIP();

        /**
         * @brief Gets server's port.
         *
         * @return server's port
         */
        string getPort();

        /**
         * @brief Gets udp socket used to communicate with the server.
         *
         * @return udp socket
         */
        int getSocketUDP() const;

        /**
         * @brief Gets tcp socket used to communicate with the server.
         *
         * @return tcp socket
         */
        int getSocketTCP() const;

        /**
         * @brief Sends a valid command by UDP to our server and gets a response.
         *
         * @param request request to be sent to the server
         *
         * @return server's response
         */
        string sendByUDP(const string& request);

        /**
         * @brief Sends a valid command by TCP to our server and gets a response.
         *
         * @param request request to be sent to the server
         *
         * @return server's response
         */
        string sendByTCP(const string& request);

        /**
         * @brief Sends a valid command by TCP with a file to our server and gets a response.
         *
         * @param request request to be sent to the server
         * @param filepath path to the file that is going to be sent
         * @param filename name of the file to be sent
         *
         * @return server's response
         */
        string sendByTCPWithFile(const string& request, const string& filepath, const string& filename);

        /**
         * @brief Cleans and frees everything related to the Connection.
         */
        void clean();

};

#endif
