//
// Created by Rodrigo Parente on 29/08/17.
//

#ifndef BYZANTINE_GENERALS_LIEUTENANT_H
#define BYZANTINE_GENERALS_LIEUTENANT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <vector>
#include "Order.h"

#define GENERAL 1

using namespace std;

class Soldier
{
public:
    Soldier(uint32_t id, uint16_t port, int numberSoldiers, Order order, bool traitor);
    ~Soldier();

    void init();
    void run();

private:
    string      mProgName;

    uint32_t    mIdentifier;                // host_info id
    uint16_t    mPort;                      // host_info port
    int         mMaxNumberSoldiers;      // max number of m_lieutenants
    Order       mOrder;
    bool        isTraitor;                  // if the host_info is to be treated as a traitor or not

    int         mNumberConnections;         // max number of connect()ions
    int         mNumberAccept;              // max number of accept()ed connections

    string      mHostIp;                    // host_info ip - 10.0.0.X

    int         *mSocketStorage;            // store host socket info

    FILE *file;

    void actAsGeneral();
    void actAsLieutenant();

    /*  */

    void discoverLieutenants();
    int createListenSock();
    int acquireConnections();
    int acceptConnections(int listenSocket);

    /* */

    char const *getOrder();

    void sendMessage(char const *buffer, int socket);

    /* Utilities functions */

    void printError(string msg, int errorsv);
    void writeToFile(string output);

    /* cleanup */

    void cleanup();
};

#endif //BYZANTINE_GENERALS_LIEUTENANT_H
