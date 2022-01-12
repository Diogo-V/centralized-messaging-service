#include "connect.h"


/**
 * Setups our udp socket.
 */
void Connect::init_socket_udp(const string& ds_ip, const string& ds_port) {

    int errcode;

    /* Creates udp socket for internet */
    this->_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(this->_fd_udp != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    /* Uses its URL to consult DNS and get the server to which we want to send messages */
    errcode = getaddrinfo(ds_ip.c_str(), ds_port.c_str(), &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call")

}


/**
 * @brief Setups our tcp socket.
 */
void Connect::init_socket_tcp(const string& ds_ip, const string& ds_port) {

    /* Creates tcp subgroup for internet */
    this->_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    assert_(this->_fd_tcp != -1, "Could not create tcp socket")

    /* Inits TCP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a TCP server's IP address */
    errcode = getaddrinfo(ds_ip.c_str(), ds_port.c_str(), &hints, &res);
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
int TimerON(int sd) {
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
int TimerOFF(int sd) {
    struct timeval timeval{};
    memset((char *) &timeval, 0, sizeof(timeval)); /* Clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
                      (struct timeval *) &timeval, sizeof(struct timeval)));
}
