//
// Created by Rodrigo Parente on 29/08/17.
//

#include "Soldier.h"

Soldier::Soldier(uint32_t id, uint16_t port, int numberSoldiers, Order order, bool traitor)
    : mIdentifier(id), mPort(port), mMaxNumberSoldiers(numberSoldiers), mOrder(order), isTraitor(traitor)
{
    mProgName = (mIdentifier == GENERAL) ? "general" : "lieutenant_" + to_string(mIdentifier) ;

    mNumberConnections = mMaxNumberSoldiers - mIdentifier;
    mNumberAccept  = mMaxNumberSoldiers - mNumberConnections - 1;
    mHostIp   = "10.0.0." +  to_string(mIdentifier);

    mSocketStorage = (int *) malloc(sizeof(int) * mMaxNumberSoldiers);
    mSocketStorage[mIdentifier - 1] = -1; /* does not have a socket for yourself */
}
Soldier::~Soldier()
{
    cleanup();
}

void Soldier::init()
{
    discoverLieutenants();
}

void Soldier::run()
{
    writeToFile("\n# Running...\n");

    if(mIdentifier == GENERAL)
        actAsGeneral();
    else
        actAsLieutenant();

    writeToFile("\n# Code completed...");
}

void Soldier::actAsGeneral()
{
    for (int i = 1; i < mMaxNumberSoldiers; i++){
        writeToFile("\tMessage sent to host " + to_string(i+1));
        sendMessage(getOrder(), mSocketStorage[i]);
    }

    writeToFile("\n\tOrder sent to all lieutenants. Waiting response...");

    cleanup();
}

void Soldier::actAsLieutenant()
{
    int i = 0;
    int nbytes = 0;
    char buffer[256];

    fd_set master;      // master file descriptor list
    fd_set read_fds;    // temp file descriptor list for select()
    int fdmax;          // maximum file descriptor number

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    fdmax = 0;

    for (i = 0; i < mMaxNumberSoldiers; i++) {
        if(i == GENERAL || i == mIdentifier)
            continue; /* skip our socket and GENERAL */

        FD_SET(mSocketStorage[i], &master); /* add socket to master set */

        if(mSocketStorage[i] > fdmax)
            fdmax = mSocketStorage[i]; /* keep track of the biggest file descriptor */
    }

    /* while rounds not finished */
    bool stopCondition = false;
    while(!stopCondition) {
        read_fds = master; /* copy of master set */
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
            printError("Error: on select().", errno);
            exit(1);
        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) /* got data to read */
            {
                if ((nbytes = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
                    if (nbytes == 0) /* connection closed() */
                        writeToFile("\tConnection closed with host " + to_string(i));
                    else
                        printError("\tError: recv()ing message.", errno);

                    close(i);           /* close socket */
                    FD_CLR(i, &master); /* remove from master set */
                } else /* recv() actual data */
                {
                    buffer[nbytes] = '\0';

                    string message = (strcmp(buffer, "0") == 0) ? "Attack" : "Retreat";

                    writeToFile("\tOrder received: " + message);

                    stopCondition = true;
                }
            }
        }
    }

    cleanup();
}

void Soldier::discoverLieutenants()
{
    // socket

    int listenfd;     // listening socket()

    /* starting point of the program */

    writeToFile("# Starting " + mProgName + " with address: 10.0.0." + to_string(mIdentifier));

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

int Soldier::createListenSock()
{
    int tmpSock;                     // temporary socket
    struct sockaddr_in server_addr;  // fill in information about connection

    writeToFile("\n# Creating listening socket...\n");

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
        printError("\tError: bind()ing the socket.", errno);
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

int Soldier::acquireConnections()
{
    int target_host;                // target host_info socket
    int target_host_id;             // target host_info identifier
    string targetAddress;          // target host_info address

    struct sockaddr_in client_addr;

    writeToFile("\n# Create and connect() to all hosts...\n");

    for (target_host_id = (mIdentifier + 1); target_host_id <= mMaxNumberSoldiers; target_host_id++) {
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

        writeToFile("\tConnect()ed to host " + targetAddress);

        string message = to_string(mIdentifier);

        // send hello message to host_info 10.0.0.X
        if(send(target_host, message.c_str(), strlen(message.c_str()), 0) < 0)
        {
            printError("\tError: send()ing hello message to host " + to_string(target_host_id), errno);
            return -1;
        }

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[target_host_id - 1] = target_host;
    }

    return 1;
}

int Soldier::acceptConnections(int listenSocket)
{
    int connections = 0;

    char buffer[256];
    int nbytes = 0;

    int newfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    writeToFile("\n# Server accept() loop...\n");

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

        string output = "\tAccept()ed a new connection from host 10.0.0.";
        output.append(buffer);

        writeToFile(output);

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[atoi(buffer) - 1] = newfd;

        connections++;
    }

    close(listenSocket); /* closing listening socket */

    return 1;
}

char const *Soldier::getOrder()
{
    char const *order;

    if(isTraitor)
        order = ((rand() % 2) == 0) ? "1" : "0"; /* random the order because lieutenant is a traitor */
    else
    {
        if(mOrder == Attack)
            order = "0";
        else if(mOrder == Retreat)
            order = "1";
    }

    return order;
}

void Soldier::sendMessage(char const *buffer, int socket)
{
    if(send(socket, buffer, sizeof(buffer), 0) < 0)
        printError("Error: send()ing message.", errno);
}

void Soldier::printError(string msg, int errorsv)
{
    char *errnoMessage = strerror(errorsv);
    msg.append(" ");
    msg.append(errnoMessage);

    writeToFile(msg);
}

void Soldier::writeToFile(string output) {
    string fileName = "log/" + mProgName + ".txt";
    file = fopen(fileName.c_str(), "a");

    if(file == nullptr){
        cerr << "Error: opening file." << endl;
        exit(1);
    }

    fprintf(file, "%s\n", output.c_str());
    fclose(file);
}

void Soldier::cleanup()
{
    for (int i = 0; i < mMaxNumberSoldiers; i++) {
        if(i != mIdentifier)
            close(mSocketStorage[i]);
    }
    free(mSocketStorage);
}
