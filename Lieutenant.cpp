//
// Created by Rodrigo Parente on 29/08/17.
//

#include "Lieutenant.h"

Lieutenant::Lieutenant(uint32_t id, uint16_t port, int max_lieutenants, bool traitor)
    : mIdentifier(id), mPort(port), mMaxNumberLieutenants(max_lieutenants), isTraitor(traitor)
{
    mProgName = (mIdentifier == GENERAL) ? "general" : "lieutenant_" + to_string(mIdentifier) ;

    mNumberConnections = mMaxNumberLieutenants - mIdentifier;
    mNumberAccept  = mMaxNumberLieutenants - mNumberConnections - 1;
    mHostIp   = "10.0.0." +  to_string(mIdentifier);

    mSocketStorage = (int *) malloc(sizeof(int) * mMaxNumberLieutenants);
    mSocketStorage[mIdentifier] = -1; /* does not have a socket for yourself */
}
Lieutenant::~Lieutenant()
{
    clean_up();
}

void Lieutenant::init()
{
    discoverLieutenants();
}

void Lieutenant::run()
{

}

void Lieutenant::discoverLieutenants()
{
    // socket

    int listenfd;     // listening socket()

    /* starting point of the program */

    writeToFile("# starting " + mProgName + " with address: 10.0.0." + to_string(mIdentifier));

    if((listenfd = createListenSock()) < 0)
    {
        writeToFile("\nERROR: creating listening socket. Quiting... ");
        exit(0);
    }

    /* create and connect() to all hosts */

    if(acquireConnections() < 0)
    {
        writeToFile("\nERROR: acquiring connections with lieutenants. Quiting... ");
        exit(1);
    }

    /* server accept() loop*/

    if(acceptConnections(listenfd) <  0)
    {
        writeToFile("\nERROR: accepting connections from lieutenants. Quiting... ");
        exit(2);
    }
}

int Lieutenant::createListenSock()
{
    int tmpSock;                     // temporary socket
    struct sockaddr_in server_addr;  // fill in information about connection

    writeToFile("\n# creating listening socket...\n");

    // initialize socket structure

    memset(&server_addr, 0, sizeof(server_addr));                // make sure the struct is empty
    server_addr.sin_family = AF_INET;                            // IPv4 family
    server_addr.sin_addr.s_addr = inet_addr(mHostIp.c_str());
    server_addr.sin_port = htons(mPort);

    // socket()

    if((tmpSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printError("\tError: creating listening socket.", errno);
        return -1;
    }

    // bind()

    if(bind(tmpSock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printError("\tError: binding the socket.", errno);
        return -1;
    }

    // listen()

    if(listen(tmpSock, this->mNumberAccept) < 0)
    {
        printError("\tError: on listen().", errno);
        return -1;
    }

    return tmpSock;
}

int Lieutenant::acquireConnections()
{
    int target_host;                // target host_info socket
    int target_host_id;             // target host_info identifier
    string targetAddress;          // target host_info address

    struct sockaddr_in client_addr;

    writeToFile("\n# create and connect() to all hosts...\n");

    for (target_host_id = (mIdentifier + 1); target_host_id <= mMaxNumberLieutenants; target_host_id++) {
        targetAddress = "10.0.0." + to_string(target_host_id);

        memset(&client_addr, 0, sizeof(client_addr));               // make sure the struct is empty
        client_addr.sin_family = AF_INET;                           // IPV4 family
        client_addr.sin_addr.s_addr = inet_addr(targetAddress.c_str());
        client_addr.sin_port = htons(mPort);


        if((target_host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            printError("\tError: creating client socket with " + targetAddress + ".", errno);
            return -1;
        }

        if(connect(target_host, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0)
        {
            printError("\tError: connect()ing to socket.", errno);
            return -1;
        }

        writeToFile("\tconnected to host " + targetAddress);

        string message = to_string(mIdentifier);

        // send hello message to host_info 10.0.0.X
        if(send(target_host, message.c_str(), strlen(message.c_str()), 0) < 0)
        {
            printError("\tError: send()ing hello message to host " + to_string(target_host_id), errno);
            return -1;
        }

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[target_host_id] = target_host;
    }

    return 1;
}

int Lieutenant::acceptConnections(int listenSocket)
{
    int connections = 0;

    char buffer[256];
    int nbytes = 0;

    int newfd   ;                    // clients socket()s
    struct sockaddr_in addr;
    socklen_t addrlen;                  // sockaddr_in size

    writeToFile("\n# server accept() loop...\n");

    while (connections < mNumberAccept) // loop until have acquire all connections
    {
        if((newfd = accept(listenSocket, (struct sockaddr *) &addr, &addrlen)) < 0)
        {
            printError("\tError: accept() failed.", errno);
            return -1;
        }

        if((nbytes = recv(newfd, buffer, sizeof(buffer), 0)) < 0)
        {
            printError("\tError: recv()ing message.", errno);
            return -1;
        }

        buffer[nbytes] = '\0';

        string output = "\taccept()ed a new connection from host 10.0.0.";
        output.append(buffer);

        writeToFile(output);

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[atoi(buffer)] = newfd;

        connections++;
    }

    return 1;
}

void Lieutenant::printError(string msg, int errorsv)
{
    char *errnoMessage = strerror(errorsv);
    msg.append(" ");
    msg.append(errnoMessage);

    writeToFile(msg);
}

void Lieutenant::writeToFile(string output) {
    string fileName = "log/" + mProgName + ".txt";
    file = fopen(fileName.c_str(), "a");

    if(file == nullptr){
        cerr << "Error: opening file." << endl;
        exit(1);
    }

    fprintf(file, "%s\n", output.c_str());
    fclose(file);
}

void Lieutenant::clean_up()
{
    for (int i = 0; i < mMaxNumberLieutenants; i++) {
        if(i != mIdentifier)
            close(mSocketStorage[i]);
    }
    free(mSocketStorage);
}
