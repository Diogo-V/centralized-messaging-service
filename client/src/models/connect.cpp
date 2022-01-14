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

    /* Creates connection between server and client */
    assert_(connect(this->getSocketTCP(), res->ai_addr, res->ai_addrlen) != -1, "Could not connect to server")

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
 * @brief Sends a valid command by UDP to our server.
 *
 * @param request request to be sent to the server
 */
void Connect::sendByUDP(const string& request) {

    ssize_t n = sendto(this->getSocketUDP(), request.c_str(), request.length(), 0, res->ai_addr, res->ai_addrlen);
    assert_(n != -1, "Failed to send message with UDP")

}


/**
 * @brief Sends a valid command by TCP to our server.
 *
 * @param request request to be sent to the server
 */
void Connect::sendByTCP(const string& request) {

    auto remaining = (ssize_t) request.length();  /* Gets request size */
    int sent;

    /* We use a buffer to avoid a non-null terminated string */
    char buffer[MAX_REQUEST_SIZE];
    memset(buffer, 0, MAX_REQUEST_SIZE);
    memcpy(buffer, request.c_str(), request.length());

    /* Keeps sending messages to sever until everything is sent */
    char* ptr = const_cast<char *>(&buffer[0]);
    while (remaining > 0) {
        assert_((sent = write(this->getSocketTCP(), ptr, MAX_REQUEST_SIZE)) > 0, "Could not send message to server")
        remaining -= sent; ptr += sent;
    }

}


/**
 * @brief Sends a valid command by TCP with a file to our server.
 *
 * @param file file that has been already opened and from where we will be reading
 * @param file_length size of the file
 */
void Connect::sendByTCPWithFile(ifstream& file, int file_length) {

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
 * @brief Receives a response from the server after sending a request by UDP.
 *
 * @return server's response
 */
string Connect::receivesByUDP() {

    int tries = UDP_N_TRIES;  /* Number of tries that are going to be executed */
    bool try_again;  /* Checks if we should try again */
    //TODO: change this
    char buffer[300]{'\0'};  /* Holds temporarily the information sent to the socket */
    string response{};  /* Used to build the server's response */
    ssize_t n;

    do {  /* We use a loop to allow retrying to send the message in case it fails */

        /* Read from socket until it finds an \n and stores it in response */
        Connect::TimerON(this->getSocketUDP());
        n = recvfrom(this->getSocketUDP(), buffer, sizeof buffer, 0, nullptr, nullptr);
        Connect::TimerOFF(this->getSocketUDP());
        response.append(buffer);
        memset(buffer, 0, sizeof buffer);

        if (n == -1) {
            try_again = true;
            tries --;
            cout << "Retrying to receive message..." << endl;
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
 * @brief Receives a response from the server after sending a request by TCP.
 *
 * @return server's response
 */
string Connect::receivesByTCP() {

    ssize_t received;  /* Holds amount of bytes received */
    char buffer[MAX_REQUEST_SIZE];  /* Holds temporarily the information sent to the socket */
    string response;  /* Used to build the server's response */

    /* Keeps on reading until everything has been read from the server */
    do {
        received = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
        assert_(received != -1, "Failed to retrieve response from server")
        if (received == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */
        response.append(buffer, strlen(buffer));
    } while (buffer[strlen(buffer) - 1] != '\n');

    /* Removes \n from end of response. Makes things easier down the line */
    response[response.length() - 1] = '\0';

    return response;

}


/**
 * @brief Receives a response from the server after sending a request by TCP with files.
 *
 * @return server's response
 */
string Connect::receivesByTCPWithFile() {

    ssize_t received;  /* Holds amount of bytes received */
    char buffer[MAX_REQUEST_SIZE];  /* Holds temporarily the information sent to the socket */
    char status[4] = {0};  /* Response status */
    int n_msgs = 0;  /* Number of messages that the server is going to send us */
    string response;  /* Used to build the server's response */
    string msg;

    /* Reads first reply that will contain the information to set up the rest of the loops */
    ssize_t n = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
    assert_(n != -1, "Failed to retrieve response from server")
    sscanf(buffer, "%*s %s %d\n", status, &n_msgs);

    /* If status is not OK, we can interrupt */
    string status_str(status);
    if (strcmp(status, "OK") != 0) return "RRT " + status_str + "\n";

    /* We loop until we get all the messages */
    do {

        /* Variables in each reply */
        char msg_id[5] = {0}; string msg_id_str;
        char uid[6] = {0}; string uid_str;
        int txt_length = 0;
        char text[MAX_POST_TEXT_SIZE] = {0}; string text_str;
        char check_file;
        char filename[MAX_FILENAME_SIZE] = {0};
        int filesize = 0;

        /* Reads message from server */
        memset(buffer, 0, MAX_REQUEST_SIZE);
        received = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
        assert_(received != -1, "Failed to retrieve response from server")
        if (received == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */

        /* Gets information from response to be used to print to the user */
        sscanf(buffer, R"(%s %s %d "%240[^"]"%c)", msg_id, uid, &txt_length, text, &check_file);

        /* Converts into string to be easier to concatenate to a final string */
        msg_id_str = msg_id; uid_str = uid; text_str = text;

        /* Creates final string */
        msg = "MSG-ID: " + msg_id_str + " | USER-ID: " + uid_str + " | TEXT: \"" + text_str + "\"";

        /* If true, then we have a file incoming */
        if (check_file == ' ') {

            /* Reads file info from server */
            memset(buffer, 0, MAX_REQUEST_SIZE);
            received = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
            assert_(received != -1, "Failed to retrieve response from server")
            if (received == 0) return "CONNECTION CLOSED";  /* If a client closes a socket, we need to ignore */

            /* Gets information about attached file */
            sscanf(buffer, "/ %s %d \n", filename, &filesize);

            memset(buffer, 0, MAX_REQUEST_SIZE);
            long remaining = 0;

            /* Gets the file path */
            char *project_directory = get_current_dir_name();
            string new_file_path = string(project_directory) + "/client/files/" + filename;

            /* Creates a new file */
            ofstream file(string(new_file_path), ofstream::out | ofstream::binary);

            /* Keeps reading the file data*/
            do {

                /* Reads from socket and puts in buffer */
                received = read(this->getSocketTCP(), buffer, MAX_REQUEST_SIZE);
                assert_(received != -1, "Failed to read from temporary socket")

                /* Writes from buffer to file */
                file.write(buffer, strlen(buffer));

                /* Cleans buffer for next iteration */
                memset(buffer, 0, MAX_REQUEST_SIZE);

                if (received == 0) break;  /* If a client closes a socket, we need to ignore */
                remaining += received;

            } while (remaining < filesize);  /* Does this until we got the whole file */

            file.close();

            string filename_str(filename);
            msg += " " + filename_str;

        }

        msg += "\n";

        response += msg;  /* Appends to response */

    } while (--n_msgs > 0);

    return response;

}


/**
 * @brief Closes current TCP connection to the server.
 */
void Connect::closeTCP() {
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
