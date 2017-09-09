//
// Created by Rodrigo Parente on 29/08/17.
//

#include "Lieutenant.h"

Lieutenant::Lieutenant(uint32_t id, uint16_t port, int max_lieutenants, bool traitor)
    : mIdentifier(id), mPort(port), mMaxNumberLieutenants(max_lieutenants), isTraitor(traitor)
{
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

    cout << "# starting lieutenant #" << mIdentifier << endl;
    cout << "# lieutenant address: 10.0.0." << mIdentifier << endl;

    if((listenfd = createListenSock()) < 0)
    {
        cerr << "Error: creating listening socket. Quiting... " << endl;
        exit(0);
    }

    /* create and connect() to all hosts */

    if(acquireConnections() < 0)
    {
        cerr << "Error: acquiring connections with lieutenants. Quiting... " << endl;
        exit(1);
    }

    /* server accept() loop*/

    if(acceptConnections(listenfd) <  0)
    {
        cerr << "Error: accepting connections from lieutenants. Quiting... " << endl;
        exit(2);
    }
}

int Lieutenant::createListenSock()
{
    int tmpSock;                     // temporary socket
    struct sockaddr_in server_addr;  // fill in information about connection

    cout << "\n# creating listening socket..." << endl;

    // initialize socket structure

    memset(&server_addr, 0, sizeof(server_addr));                // make sure the struct is empty
    server_addr.sin_family = AF_INET;                            // IPv4 family
    server_addr.sin_addr.s_addr = inet_addr(mHostIp.c_str());
    server_addr.sin_port = htons(mPort);

    // socket()

    if((tmpSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        cerr << "Error: creating listening socket." << endl;
        return -1;
    }

    // bind()

    if(bind(tmpSock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Error: binding the socket.\n" << endl;
        return -1;
    }

    // listen()

    if(listen(tmpSock, this->mNumberAccept) < 0)
    {
        cerr << "Error: on listen.\n" << endl;
        return -1;
    }

    return tmpSock;
}

int Lieutenant::acquireConnections()
{
    int target_host;                // target host_info socket
    int target_host_id;             // target host_info identifier
    string target_address;          // target host_info address

    struct sockaddr_in client_addr;

    cout << "\n# create and connect() to all hosts...\n" << endl;

    for (target_host_id = (mIdentifier + 1); target_host_id <= mMaxNumberLieutenants; target_host_id++) {
        target_address = "10.0.0." + to_string(target_host_id);

        cout << "trying to connect() to host_info " << target_address << "..." << endl;

        memset(&client_addr, 0, sizeof(client_addr));               // make sure the struct is empty
        client_addr.sin_family = AF_INET;                           // IPV4 family
        client_addr.sin_addr.s_addr = inet_addr(target_address.c_str());
        client_addr.sin_port = htons(mPort);


        if((target_host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            cerr << "error creating client socket." << endl;
            return -1;
        }

        if(connect(target_host, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0)
        {
            cerr << "error connect()ing socket." << endl;
            return -1;
        }

        cout << "connected to host_info " << target_address << endl;

        string message = to_string(mIdentifier);

        // send hello message to host_info 10.0.0.X
        if(send(target_host, message.c_str(), strlen(message.c_str()), 0) < 0)
        {
            cerr << "error send()ing message to host_info 10.0.0." << target_host_id << endl;
            return -1;
        }

        cout << "message sent to host_info 10.0.0." << target_host_id << endl;

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[target_host_id] = target_host;
    }

    return 1;
}

int Lieutenant::acceptConnections(int listen_socket)
{
    int connections = 0;

    char buffer[256];
    int nbytes = 0;

    int target_host;                    // clients socket()s
    struct sockaddr_in client_addr;
    socklen_t addrlen;                  // sockaddr_in size

    cout << "\n# server accept() loop...\n" << endl;
    cout << "\n mNumberAccept: " << mNumberAccept << endl;

    while (connections < mNumberAccept) // loop until have acquire all connections
    {
        cout << "waiting for a new connection...\n" << endl;

        if((target_host = accept(listen_socket, (struct sockaddr *) &client_addr, &addrlen)) < 0)
        {
            cerr << "accept() failed." << endl;
            return -1;
        }

        if((nbytes = recv(target_host, buffer, sizeof(buffer), 0)) < 0)
        {
            cerr << "error recv()ing message." << endl;
            return -1;
        }

        buffer[nbytes] = '\0';

        cout << "accept()ed a new connection from host_info 10.0.0." << buffer << endl;

        /* TODO: add socket to socket m_storage class */
        mSocketStorage[atoi(buffer)] = target_host;

        connections++;
    }

    return 1;
}

void Lieutenant::clean_up()
{
    for (int i = 0; i < mMaxNumberLieutenants; i++) {
        if(i != mIdentifier)
            close(mSocketStorage[i]);
    }
    free(mSocketStorage);
}