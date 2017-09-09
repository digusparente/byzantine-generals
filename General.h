//
// Created by Rodrigo Parente on 04/09/17.
//

#ifndef BYZANTINE_GENERALS_SOLDIER_H
#define BYZANTINE_GENERALS_SOLDIER_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "Order.h"

using namespace std;

class General
{
public:
    General(uint32_t id, uint16_t port, int numberSoldiers, Order order, bool traitor);
    ~General();

    void init();
    void run();

    void printVariables();

private:
    uint32_t    mIdentifier;
    uint16_t    mPort;
    int         mMaxNumberSoldiers;
    Order       mOrder;

    bool        isTraitor;

    int         *mSocketStorage;

    FILE        *file;

    char const *getOrder();
    int acquireConnections();

    void sendMessage(char const *buffer, int socket);
    void writeToFile(string output);

    void cleanup();
};


#endif //BYZANTINE_GENERALS_SOLDIER_H
