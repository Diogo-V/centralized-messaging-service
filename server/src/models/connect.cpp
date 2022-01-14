#include "connect.h"


/**
 * Setups our udp socket.
 */
void Connect::init_socket_udp() {

    struct addrinfo hints{};  /* Used to request info from DNS to get our "endpoint" */
    int errcode;  /* Holds current error */

    /* Creates udp subgroup for internet */
    this->_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(this->getSocketUDP() != -1, "Could not create udp socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a UDP server's IP address */
    errcode = getaddrinfo(nullptr, this->getPort().c_str(), &hints, (struct addrinfo**) &this->_res);
    assert_(errcode == 0, "Failed getaddrinfo call for udp socket")

    /* Binds sockets to our specified port and tells our SO that this channel if for this program */
    int err = bind(this->getSocketUDP(), this->_res.ai_addr, this->_res.ai_addrlen);
    assert_(err == 0, "Failed to bind udp socket")

}


/**
 * @brief Setups server tcp socket.
 */
void Connect::init_socket_tcp() {

    int errcode;  /* Holds current error */
    struct addrinfo hints{};  /* Used to request info from DNS to get our "endpoint" */

    /* Creates tcp subgroup for internet */
    this->_fd_udp = socket(AF_INET, SOCK_STREAM, 0);
    assert_(this->getSocketTCP() != -1, "Could not create tcp socket")

    /* Inits TCP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a TCP server's IP address */
    errcode = getaddrinfo(nullptr, this->getPort().c_str(), &hints, &this->_res);
    assert_(errcode == 0, "Failed getaddrinfo call for tcp")

    /* Binds sockets to our specified port and tells our SO that this channel if for this program */
    int err = bind(this->getSocketTCP(), this->_res.ai_addr, this->_res.ai_addrlen);
    assert_(err == 0, "Failed to bind tcp socket")

    /* Prepares socket to receive connections */
    assert_(listen(this->getSocketTCP(),TCP_N_CONNECTIONS) != -1, "Could not prepare tcp socket")

}


/**
 * @brief Connect class constructor.
 *
 * @param port port of the server
 */
Connect::Connect(const string& port) {
    this->_port = port;
    this->init_socket_udp();
    this->init_socket_tcp();
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
 * @brief Gets a temporary socket used to communicate with the connected client.
 *
 * @return temporary tcp socket
 */
int Connect::getSocketTmpTCP() {
    return this->_tmp_fd_tcp;
}


/**
 * @brief Sets temporary tcp socket.
 *
 * @param new_fd new temporary tcp socket
 */
void Connect::setSocketTmpTCP(int new_fd) {
    this->_tmp_fd_tcp = new_fd;
}


/**
 * @brief Gets set of file descriptors that are being watched.
 *
 * @return set of file descriptors
 */
fd_set* Connect::getFDS() {
    return &this->_fds;
}


/**
 * @brief Gets currently connected client's ip.
 *
 * @return client's ip
 */
string Connect::getClientIP() {
    return this->_client_ip;
}


/**
 * @brief Gets currently connected client's port.
 *
 * @return client's port
 */
string Connect::getClientPort() {
    return this->_client_port;
}


/**
 * @brief Sets currently connected client's ip.
 *
 * @param ip new ip
 */
void Connect::setClientIP(const string& ip) {
    this->_client_ip = ip;
}


/**
 * @brief Sets currently connected client's port.
 *
 * @param port new port
 */
void Connect::setClientPort(const string& port) {
    this->_client_port = port;
}


/**
 * @brief Gets client's address length.
 *
 * @return addrlen
 */
socklen_t* Connect::getAddrLen() {
    return &this->_addrlen;
}


/**
 * @brief Sets new value for addr len;
 *
 * @param size new size for addrlen
 */
void Connect::setAddrLen(socklen_t size) {
    this->_addrlen = size;
}


/**
 * @brief Gets client's address information.
 *
 * @return addr
 */
struct sockaddr_in* Connect::getAddr() {
    return &this->_addr;
}


/**
 * @brief Cleans previous information in addr and addrlen.
 */
void Connect::cleanAddr() {
    bzero(this->getAddr(), sizeof(struct sockaddr_in));
    this->setAddrLen(sizeof *this->getAddr());
}


/**
 * @brief Receives a valid command by a client in UDP socket.
 *
 * @return client's request
 */
string Connect::receiveByUDP() {

    char buffer[MAX_REQUEST_SIZE]{'\0'};

    /* Receives message from client */
    ssize_t n = recvfrom(this->getSocketUDP(), buffer, MAX_REQUEST_SIZE, 0,
                         (struct sockaddr *) this->getAddr(), this->getAddrLen());
    assert_(n != -1, "Failed to receive message")

    /* Saves currently connected client */
    setClientIP(inet_ntoa((*this->getAddr()).sin_addr));
    setClientPort(to_string(ntohs((*this->getAddr()).sin_port)));

    /* Removes \n at the end of the buffer. Makes things easier down the line */
    buffer[strlen(buffer) - 1] = '\0';

    return buffer;

}


/**
 * @brief Send a response to a client in UDP socket.
 *
 * @param response response that is going to be sent back to the client.
 */
void Connect::replyByUDP(const string& response) {

    /* Sends response back to client */
    ssize_t n = sendto(this->getSocketUDP(), response.c_str(), response.size(), 0,
                       (struct sockaddr*) this->getAddr(), *this->getAddrLen());
    assert_(n != -1, "Failed to send message")

}


/**
 * @brief Receives a valid command by a client in TCP socket.
 *
 * @return client's request
 */
string Connect::receiveByTCP() {

    char buffer[MAX_REQUEST_SIZE];  // Temporary buffer to receive all the information
    string request{'\0'};

    /* Creates temporary socket to connect to client. Keeps main channel active */
    int tmp_fd = accept(this->getSocketTCP(),(struct sockaddr*) this->getAddr(), this->getAddrLen());
    assert_(tmp_fd != -1, "Could not create temporary tcp socket")
    this->setSocketTmpTCP(tmp_fd);

    /* Keeps on reading until everything has been read from the client */
    do {
        ssize_t nr = read(this->getSocketTmpTCP(), buffer, MAX_REQUEST_SIZE);
        assert_(nr != -1, "Failed to read from temporary socket")
        if (nr == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */
        request.append(buffer, strlen(buffer));
    } while (buffer[strlen(buffer) - 1] != '\n');

    /* Removes \n at the end of the buffer. Makes things easier down the line */
    request[request.length() - 1] = '\0';
    return request;

}


/**
 * @brief Send a response to a client in TCP socket.
 *
 * @param response response that is going to be sent back to the client.
 */
void Connect::replyByTCP(const string& response) {

    /* We are going to send until we send everything */
    int remaining = (int) response.length();
    int sent;

    /* Keeps sending messages to client until everything is sent */
    char* ptr = const_cast<char *>(&response[0]);
    while (remaining > 0) {
        assert_((sent = write(this->getSocketTmpTCP(), ptr, MAX_REQUEST_SIZE)) > 0, "Could not send message to client")
        remaining -= sent; ptr += sent;
    }

    close(this->getSocketTmpTCP());  /* Closes file descriptor to avoid errors */

}


/**
 * @brief Receives a valid command by a client in TCP socket with a file.
 *
 * @return client's request
 */
string Connect::receiveByTCPWithFile() {

//    char buffer[100];  /* Holds temporarily the information sent to the socket */
//    string response{};  /* Used to build the server's response */
//
//    /* Initializes and setups fd_udp to be a valid socket */
//    init_socket_tcp();
//
//    /* Creates connection between server and client */
//    assert_(connect(this->getSocketTCP(), res->ai_addr, res->ai_addrlen) != -1, "Could not connect to sever")
//
//    uint16_t nw;  /* Used to keep track of how many bytes we have sent to the server */
//    auto n = (ssize_t) initial_request.length();  /* Sends request size */
//
//    /* Keeps sending messages to sever until everything is sent. Only sends initial request */
//    char* ptr = const_cast<char *>(&initial_request[0]);
//    while (n > 0) {
//        assert_((nw = write(this->getSocketTCP(), ptr, MAX_REQUEST_SIZE)) > 0, "Could not send message to server")
//        n -= nw; ptr += nw;
//    }
//
//    /* Since we have already finished sending the initial request to the server, we have to send
//     * the input file now */
//    ifstream file(string(filepath), ifstream::in | ifstream::binary);
//
//    string req = " " + string(filename) + " " + to_string(file.tellg());
//
//    int file_length = (int) file.tellg();  /* Sends request size */
//
//    /* Keeps sending messages to sever until everything is sent */
//    filebuf* file_p = file.rdbuf();
//
//    /* Sends Filename and filesize */
//    int bytes_sent;
//    assert_((bytes_sent = write(this->getSocketTCP(), req.c_str(), MAX_REQUEST_SIZE)) > 0, "Could not send message to server")
//
//    /* Then sends the data*/
//    while (file_length > 0) {
//        assert_((bytes_sent = write(this->getSocketTCP(), file_p, MAX_RECEIVED_FILE_DATA)) > 0, "Could not send data message to server")
//        file_length -= bytes_sent; file_p += bytes_sent;
//    }
//
//    /* Close file*/
//    file.close();
//
//    /* Keeps on reading until everything has been read from the server */
//    n = 0;
//    do {
//        n += read(this->getSocketTCP(), buffer, sizeof buffer);
//        assert_(n != -1, "Failed to retrieve response from server")
//        response.append(buffer);
//    } while (n < MAX_REQUEST_SIZE);
//
//    /* Removes \n from end of response. Makes things easier down the line */
//    response[response.length() - 1] = '\0';
//
//    /* Closes TCP connection */
//    close(this->getSocketTCP());

}


/**
 * @brief Cleans and frees everything related to the Connection.
 */
void Connect::clean() {
    freeaddrinfo(&this->_res);
    close(this->getSocketTCP());
    close(this->getSocketUDP());
}
