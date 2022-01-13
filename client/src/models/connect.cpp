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
int Connect::getSocketUDP() const {
    return this->_fd_udp;
}


/**
 * @brief Gets tcp socket used to communicate with the server.
 *
 * @return tcp socket
 */
int Connect::getSocketTCP() const {
    return this->_fd_tcp;
}


/**
 * @brief Sends a valid command by UDP to our server and gets a response.
 *
 * @param request request to be sent to the server
 *
 * @return server's response
 */
string Connect::sendByUDP(const string& request) {

    int tries = UDP_N_TRIES;  /* Number of tries that are going to be executed */
    bool try_again;  /* Checks if we should try again */
    char buffer[100]{'\0'};  /* Holds temporarily the information sent to the socket */
    string response{};  /* Used to build the server's response */

    do {  /* We use a loop to allow retrying to send the message in case it fails */

        /* Sends message to server */
        ssize_t n = sendto(this->getSocketUDP(), request.c_str(), request.length(), 0, res->ai_addr, res->ai_addrlen);
        assert_(n != -1, "Failed to send message with UDP")

        /* Read from socket until it finds an \n and stores it in response */
        Connect::TimerON(this->getSocketUDP());
        n = recvfrom(this->getSocketUDP(), buffer, sizeof buffer, 0, nullptr, nullptr);
        Connect::TimerOFF(this->getSocketUDP());
        response.append(buffer);
        memset(buffer, 0, sizeof buffer);

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
            return "INVALID";  // TODO: do this better
        }

    } while (response.back() != '\n' && try_again && tries > 0);

    /* Removes \n from end of response. Makes things easier down the line */
    response[response.length() - 1] = '\0';

    return response;

}


/**
 * @brief Sends a valid command by TCP to our server and gets a response.
 *
 * @param request request to be sent to the server
 *
 * @return server's response
 */
string Connect::sendByTCP(const string& request) {

    char buffer[MAX_REQUEST_SIZE];  /* Holds temporarily the information sent to the socket */
    string response{};  /* Used to build the server's response */

    /* Initializes and setups fd_udp to be a valid socket */
    init_socket_tcp();

    /* Creates connection between server and client */
    assert_(connect(this->getSocketTCP(), res->ai_addr, res->ai_addrlen) != -1, "Could not connect to sever")

    uint16_t nw;  /* Used to keep track of how many bytes we have sent to the server */
    auto n = (ssize_t) request.length();  /* Sends request size */

    /* Keeps sending messages to sever until everything is sent */
    char* ptr = const_cast<char *>(&request[0]);
    while (n > 0) {
        assert_((nw = write(this->getSocketTCP(), ptr, MAX_REQUEST_SIZE)) > 0, "Could not send message to server")
        n -= nw; ptr += nw;
    }

    /* Keeps on reading until everything has been read from the server */
    do {
        ssize_t nr = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
        assert_(n != -1, "Failed to retrieve response from server")
        if (nr == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */
        response.append(buffer, strlen(buffer));
    } while (buffer[strlen(buffer) - 1] != '\n');

    /* Removes \n from end of response. Makes things easier down the line */
    response[response.length() - 1] = '\0';

    /* Closes TCP connection */
    close(this->getSocketTCP());

    return response;

}


/**
 * @brief Sends a valid command by TCP with a file to our server and gets a response.
 *
 * @param request request to be sent to the server
 * @param filepath path to the file that is going to be sent
 * @param filename name of the file to be sent
 *
 * @return server's response
 */
string Connect::sendByTCPWithFile(const string& initial_request, const string& filepath, const string& filename) {

    char buffer[100];  /* Holds temporarily the information sent to the socket */
    string response{};  /* Used to build the server's response */

    /* Initializes and setups fd_udp to be a valid socket */
    init_socket_tcp();

    /* Creates connection between server and client */
    assert_(connect(this->getSocketTCP(), res->ai_addr, res->ai_addrlen) != -1, "Could not connect to sever")

    uint16_t nw;  /* Used to keep track of how many bytes we have sent to the server */
    auto n = (ssize_t) initial_request.length();  /* Sends request size */

    /* Keeps sending messages to sever until everything is sent. Only sends initial request */
    char* ptr = const_cast<char *>(&initial_request[0]);
    while (n > 0) {
        assert_((nw = write(this->getSocketTCP(), ptr, MAX_REQUEST_SIZE)) > 0, "Could not send message to server")
        n -= nw; ptr += nw;
    }

    /* Since we have already finished sending the initial request to the server, we have to send
     * the input file now */
    ifstream file(string(filepath), ifstream::in | ifstream::binary);

    string req = " " + string(filename) + " " + to_string(file.tellg());

    int file_length = (int) file.tellg();  /* Sends request size */

    /* Keeps sending messages to sever until everything is sent */
    filebuf* file_p = file.rdbuf();

    /* Sends Filename and filesize */
    int bytes_sent;
    assert_((bytes_sent = write(this->getSocketTCP(), req.c_str(), MAX_REQUEST_SIZE)) > 0, "Could not send message to server")

    /* Then sends the data*/
    while (file_length > 0) {
        assert_((bytes_sent = write(this->getSocketTCP(), file_p, MAX_SENT_FILE_DATA)) > 0, "Could not send data message to server")
        file_length -= bytes_sent; file_p += bytes_sent;
    }

    /* Close file*/
    file.close();

    /* Keeps on reading until everything has been read from the server */
    n = 0;
    do {
        n += read(this->getSocketTCP(), buffer, sizeof buffer);
        assert_(n != -1, "Failed to retrieve response from server")
        response.append(buffer);
    } while (n < MAX_REQUEST_SIZE);

    /* Removes \n from end of response. Makes things easier down the line */
    response[response.length() - 1] = '\0';

    /* Closes TCP connection */
    close(this->getSocketTCP());

}


/**
 * @brief Cleans and frees everything related to the Connection.
 */
void Connect::clean() {
    freeaddrinfo(this->res);
    close(this->getSocketTCP());  // It is not needed per se but should not hurt to be sure
    close(this->getSocketUDP());
}
