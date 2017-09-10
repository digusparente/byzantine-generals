//
// Created by Rodrigo Parente on 29/08/17.
//

#ifndef BYZANTINE_GENERALS_LIEUTENANT_H
#define BYZANTINE_GENERALS_LIEUTENANT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <vector>

#define GENERAL 1

using namespace std;

class Lieutenant
{
public:
    Lieutenant(uint32_t id, uint16_t port, int max_lieutenants, bool traitor);
    ~Lieutenant();

    void init();
    void run();

private:
    string      mProgName;

    uint32_t    mIdentifier;                // host_info id
    uint16_t    mPort;                      // host_info port
    int         mMaxNumberLieutenants;      // max number of m_lieutenants
    bool        isTraitor;                  // if the host_info is to be treated as a traitor or not

    int         mNumberConnections;         // max number of connect()ions
    int         mNumberAccept;              // max number of accept()ed connections

    string      mHostIp;                    // host_info ip - 10.0.0.X

    int         *mSocketStorage;            // store host socket info

    FILE *file;

    void discoverLieutenants();
    int createListenSock();
    int acquireConnections();
    int acceptConnections(int listen_socket);

    /* Utilities functions */

    void printError(string msg, int errorsv);

    void writeToFile(string output);

    void clean_up();
};

#endif //BYZANTINE_GENERALS_LIEUTENANT_H
