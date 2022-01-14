#ifndef PROJETO_RC_39_V2_CONNECT_H
#define PROJETO_RC_39_V2_CONNECT_H

#include "../misc/helpers.h"

#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fstream>

#define MAX_REQUEST_SIZE 300
#define MAX_RECEIVED_FILE_DATA 300
#define TEXT_MAX_SIZE 240
#define FILENAME_MAX_SIZE 24
#define TCP_N_CONNECTIONS 5


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
         * @brief Holds temporary tcp file descriptor that is going to be used by a client.
         */
        int _tmp_fd_tcp{};

        /**
         * @brief File descriptor for a udp connection.
         */
        int _fd_udp{};

        /**
         * @brief Server's port.
         */
        string _port;

        /**
         * @brief Stores result from getaddrinfo and uses it to set up our socket.
         */
        struct addrinfo* _res{};

        /**
         * @brief Holds size of message sent from client.
         */
        socklen_t _addrlen;

        /**
         * @brief Describes internet socket address. Holds sender info.
         */
        struct sockaddr_in _addr;

        /**
         * @brief Holds file descriptors in select.
         */
        fd_set _fds;

        /**
         * @brief Saves currently connect client's ip.
         */
        string _client_ip;

        /**
         * @brief Saves currently connect client's port.
         */
        string _client_port;

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
         * @brief Gets client's address length.
         *
         * @return addrlen
         */
        socklen_t* getAddrLen();

        /**
         * @brief Sets new value for addr len;
         *
         * @param size new size for addrlen
         */
        void setAddrLen(socklen_t size);

        /**
         * @brief Gets client's address information.
         *
         * @return addr
         */
        struct sockaddr_in* getAddr();

    public:

        /**
         * @brief Connect class constructor.
         *
         * @param port port of the server
         */
        explicit Connect(const string& port);

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
         * @brief Gets a temporary socket used to communicate with the connected client.
         *
         * @return temporary tcp socket
         */
        int getSocketTmpTCP();

        /**
         * @brief Sets temporary tcp socket.
         *
         * @param new_fd new temporary tcp socket
         */
        void setSocketTmpTCP(int new_fd);

        /**
         * @brief Gets set of file descriptors that are being watched.
         *
         * @return set of file descriptors
         */
        fd_set* getFDS();

        /**
         * @brief Gets currently connected client's ip.
         *
         * @return client's ip
         */
        string getClientIP();

        /**
         * @brief Gets currently connected client's port.
         *
         * @return client's port
         */
        string getClientPort();

        /**
         * @brief Sets currently connected client's ip.
         *
         * @param ip new ip
         */
        void setClientIP(const string& ip);

        /**
         * @brief Sets currently connected client's port.
         *
         * @param port new port
         */
        void setClientPort(const string& port);

        /**
         * @brief Cleans previous information in addr and addrlen.
         */
        void cleanAddr();

        /**
         * @brief Receives a valid command by a client in UDP socket.
         *
         * @return client's request
         */
        string receiveByUDP();

        /**
         * @brief Send a response to a client in UDP socket.
         *
         * @param response response that is going to be sent back to the client.
         */
        void replyByUDP(const string& response);

        /**
         * @brief Receives a valid command by a client in TCP socket.
         *
         * @return client's request
         */
        string receiveByTCP();

        /**
         * @brief Send a response to a client in TCP socket.
         *
         * @param response response that is going to be sent back to the client.
         */
        void replyByTCP(const string& response);

        /**
         * @brief Receives a valid command by a client in TCP socket with a file.
         *
         * @return client's request
         */
        string receiveByTCPWithFile();

        /**
         * @brief Cleans and frees everything related to the Connection.
         */
        void clean();

};

#endif
