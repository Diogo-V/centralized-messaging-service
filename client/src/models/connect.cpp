#include "connect.h"


/**
 * Setups our udp socket.
 */
void Connect::init_socket_udp() {

    struct addrinfo hints{};  /* Used to request info from DNS to get our "endpoint" */
    int errcode;  /* Holds current error */

    /* Creates udp socket for internet */
    this->_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(this->_fd_udp != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    /* Uses its URL to consult DNS and get the server to which we want to send messages */
    errcode = getaddrinfo(this->getIP().c_str(), this->getPort().c_str(), &hints, &this->res);
    assert_(errcode == 0, "Failed getaddrinfo call")

}


/**
 * @brief Setups our tcp socket.
 */
void Connect::init_socket_tcp() {

    int errcode;  /* Holds current error */
    struct addrinfo hints{};  /* Used to request info from DNS to get our "endpoint" */

    /* Creates tcp subgroup for internet */
    this->_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    assert_(this->_fd_tcp != -1, "Could not create tcp socket")

    /* Inits TCP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a TCP server's IP address */
    errcode = getaddrinfo(this->getIP().c_str(), this->getPort().c_str(), &hints, &this->res);
    assert_(errcode == 0, "Failed getaddrinfo call for tcp")

}


/**
 * @brief Activates a timer to wait about 5 seconds. It is used as a measure of backup in case UDP
 * is not able to send the message to our server.
 *
 * @param sd socket to be timed
 *
 * @return manipulates socket options
 */
int Connect::TimerON(int sd) {
    struct timeval timeval{};
    memset((char *) &timeval, 0, sizeof(timeval)); /* clear time structure */
    timeval.tv_sec = TIMEOUT_TIME_S; /* Wait for 5 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
                      (struct timeval *)&timeval, sizeof(struct timeval)));
}


/**
 * @brief Disables a timer previously activated for an input socket.
 *
 * @param sd socket that was timed
 *
 * @return manipulates socket options
 */
int Connect::TimerOFF(int sd) {
    struct timeval timeval{};
    memset((char *) &timeval, 0, sizeof(timeval)); /* Clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
                      (struct timeval *) &timeval, sizeof(struct timeval)));
}


/**
 * @brief Connect class constructor.
 *
 * @param ip ip of the server
 * @param port port of the server
 */
Connect::Connect(const string& ip, const string& port) {
    this->_ip = ip;
    this->_port = port;
    this->init_socket_udp();
}


/**
 * @brief Gets server's id address.
 *
 * @return server's ip address
 */
string Connect::getIP() {
    return this->_ip;
}


/**
 * @brief Gets server's port.
 *
 * @return server's port
 */
string Connect::getPort() {
    return this->_port;
}


/**
 * @brief Gets udp socket used to communicate with the server.
 *
 * @return udp socket
 */
int Connect::getSocketUDP() {
    return this->_fd_udp;
}


/**
 * @brief Gets tcp socket used to communicate with the server.
 *
 * @return tcp socket
 */
int Connect::getSocketTCP() {
    return this->_fd_tcp;
}


/**
 * @brief Sends a valid command by UDP to our server and gets a response.
 *
 * @param request request to be sent to the server
 *
 * @return server's response
 */
string sendByUDP(const string& request) {

    int tries = UDP_N_TRIES;
    bool try_again;
    do {  /* We use a loop to allow retrying to send the message in case it fails */

        /* Sends message to server */
        n = sendto(fd_udp, req.c_str(), req.length(), 0, res->ai_addr, res->ai_addrlen);
        assert_(n != -1, "Failed to send message with UDP")

        /* Clears previous iteration's information */
        bzero(&addr, sizeof(struct sockaddr_in));
        addrlen = sizeof(addr);

        TimerON(fd_udp);
        n = recvfrom(fd_udp, res_buffer, MSG_MAX_SIZE, 0, (struct sockaddr *) &addr, &addrlen);
        TimerOFF(fd_udp);

        if (n == -1) {
            try_again = true;
            tries --;
            cout << "Retrying to send message..." << endl;
        } else {
            try_again = false;
        }

        /* We tried 3 times before and was not able to send our message to the client */
        if (try_again && tries == 0) {
            cerr << "Connection timed out and was not able to send the message." << endl;
            con = NO_CON;  // This is used to bypass the selector function down below
        }

    } while (try_again && tries > 0);

    res_buffer[strlen(res_buffer) - 1] = '\0';

    return res_buffer;

}


/**
 * @brief Sends a valid command by TCP to our server and gets a response.
 *
 * @param request request to be sent to the server
 *
 * @return server's response
 */
string sendByTCP(const string& request) {

    /* Initializes and setups fd_udp to be a valid socket */
    init_socket_tcp();

    /* Creates connection between server and client */
    assert_(connect(fd_tcp, res->ai_addr, res->ai_addrlen) != -1, "Could not connect to sever")

    uint16_t nw;  /* Used to keep track of how many bytes we have sent to the server */
    n = (ssize_t) req.length();  /* Sends request size */

    /* Keeps sending messages to sever until everything is sent */
    char* ptr = &req[0];
    while (n > 0) {
        assert_((nw = write(fd_tcp, ptr, MSG_MAX_SIZE)) > 0, "Could not send message to server")
        n -= nw; ptr += nw;
    }

    /* Keeps on reading until everything has been read from the server */
    while ((n = read(fd_tcp, res_buffer, MSG_MAX_SIZE)) != 0) {
        assert_(n != -1, "Failed to retrieve response from server")
    }

    res_buffer[strlen(res_buffer) - 1] = '\0';

    /* Closes TCP connection */
    close(fd_tcp);

    return res_buffer;

}
