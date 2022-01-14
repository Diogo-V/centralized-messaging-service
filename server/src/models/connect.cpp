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
    errcode = getaddrinfo(nullptr, this->getPort().c_str(), &hints, &this->_res);
    assert_(errcode == 0, "Failed getaddrinfo call for udp socket")

    /* Binds sockets to our specified port and tells our SO that this channel if for this program */
    int err = bind(this->getSocketUDP(), this->_res->ai_addr, this->_res->ai_addrlen);
    assert_(err == 0, "Failed to bind udp socket")

}


/**
 * @brief Setups server tcp socket.
 */
void Connect::init_socket_tcp() {

    int errcode;  /* Holds current error */
    struct addrinfo hints{};  /* Used to request info from DNS to get our "endpoint" */

    /* Creates tcp subgroup for internet */
    this->_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
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
    int err = bind(this->getSocketTCP(), this->_res->ai_addr, this->_res->ai_addrlen);
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
    string request;
    memset(buffer, 0, MAX_REQUEST_SIZE);

    /* Creates temporary socket to connect to client. Keeps main channel active */
    this->_tmp_fd_tcp = accept(this->getSocketTCP(),(struct sockaddr*) this->getAddr(), this->getAddrLen());
    assert_(this->_tmp_fd_tcp != -1, "Could not create temporary tcp socket")



    /* Keeps on reading until everything has been read from the client */
    char* ptr=&buffer[0];
    do {
        ssize_t nr = read(this->_tmp_fd_tcp, ptr, MAX_REQUEST_SIZE);
        assert_(nr != -1, "Failed to read from temporary socket")
        if (nr == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */
        request.append(buffer, strlen(buffer));
        ptr += nr;
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

}


/**
 * @brief Send a response with a file to a client in TCP socket.
 *
 * @param file file that is being sent
 * @param file_length size of the file
 */
void Connect::replyByTCPWithFile(ifstream& file, int file_length) {

    char file_data[MAX_REQUEST_SIZE];  /* Temporary buffer to hold file information */
    int bytes_sent;  /* Tells us how many bytes have been read from each iteration */

    /* Sends file data to server bit by but */
    while (file_length > 0) {
        file.read(file_data, MAX_REQUEST_SIZE);
        assert_((bytes_sent = write(this->getSocketTCP(), file_data, MAX_REQUEST_SIZE)) > 0, "Could not send data message to server")
        file_length -= bytes_sent;
        memset(file_data, 0, MAX_REQUEST_SIZE);
    }

}


/**
 * @brief Receives a valid command by a client in TCP socket with a file.
 *
 * @param file_name name of the file
 * @param file_size size of the file
 */
void Connect::receiveByTCPWithFile(const string& file_name, const int& file_size) {

    char buffer[MAX_REQUEST_SIZE];  /* Auxiliary buffer */
    memset(buffer, 0, MAX_REQUEST_SIZE);
    ssize_t received;
    long remaining = 0;

    /* Gets the file path */
    char *project_directory = get_current_dir_name();
    string new_file_path = string(project_directory) + "/server/files/" + file_name ;

    /* Creates a new file */
    ofstream file(string(new_file_path), ofstream::out | ofstream::binary);

    /* Keeps reading the file data*/
    do {

        /* Reads from socket and puts in buffer */
        received = read(this->getSocketTmpTCP(), buffer, MAX_REQUEST_SIZE);
        assert_(received != -1, "Failed to read from temporary socket")

        /* Writes from buffer to file */
        file.write(buffer, strlen(buffer));

        /* Cleans buffer for next iteration */
        memset(buffer, 0, MAX_REQUEST_SIZE);

        if (received == 0) break;  /* If a client closes a socket, we need to ignore */
        remaining += received;

    } while (remaining < file_size);  /* Does this until we got the whole file */

    file.close();

}


/**
 * @brief Cleans and frees everything related to the Connection.
 */
void Connect::clean() {
    freeaddrinfo(this->_res);
    close(this->getSocketTCP());
    close(this->getSocketUDP());
}
