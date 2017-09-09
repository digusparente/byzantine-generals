//
// Created by Rodrigo Parente on 04/09/17.
//

#include "General.h"

General::General(uint32_t id, uint16_t port, int numberSoldiers, Order order, bool traitor)
    : mIdentifier(id), mPort(port), mMaxNumberSoldiers(numberSoldiers), mOrder(order), isTraitor(traitor)
{

    mSocketStorage = (int *) malloc(sizeof(int) * mMaxNumberSoldiers);
    mSocketStorage[mIdentifier] = -1; /* does not have a socket for yourself */

    srand(time(nullptr));
}

General::~General()
{
    cleanup();
}

void General::init()
{


    if(acquireConnections() < 0)
    {
        // cerr << "Error: acquiring connections with lieutenants. Quiting... " << endl;
        writeToFile("Error: acquiring connections with lieutenants. Quiting... ");
        exit(1);
    }
}

int General::acquireConnections()
{
    int targetSocket;         // target host_info socket
    int targetId;             // target host_info identifier
    string targetAddress;     // target host_info address

    struct sockaddr_in lieutenantAddr;

    // cout << "\n# create a connect()ion to all lieutenants...\n" << endl;
    writeToFile("\n# create a connect()ion to all lieutenants...\n");

    for (targetId = (mIdentifier + 1); targetId <= mMaxNumberSoldiers; targetId++) {
        targetAddress = "10.0.0." + to_string(targetId);

        // cout << "trying to connect() to host " << targetAddress << endl;

        string output = "trying to connect() to host " + targetAddress;
        writeToFile(output);

        memset(&lieutenantAddr, 0, sizeof(lieutenantAddr));                 // make sure the struct is empty
        lieutenantAddr.sin_family = AF_INET;                                // IPV4 family
        lieutenantAddr.sin_addr.s_addr = inet_addr(targetAddress.c_str());
        lieutenantAddr.sin_port = htons(mPort);

        if((targetSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            // cerr << "error creating lieutenant socket." << endl;
            writeToFile("error creating lieutenant socket.");
            return -1;
        }

        if(connect(targetSocket, (struct sockaddr *) &lieutenantAddr, sizeof(lieutenantAddr)) < 0)
        {
            writeToFile("error connect()ing socket.");
            return -1;
        }

        output = "connected to host " + targetAddress;
        writeToFile(output);

        // send hello message to host 10.0.0.X
        if(send(targetSocket, "0", sizeof(char), 0) < 0)
        {
            // cerr << "error send()ing message to host 10.0.0." << targetId << endl;
            output = "error send()ing message to host 10.0.0." + targetId;
            writeToFile(output);
            return -1;
        }

        mSocketStorage[targetId] = targetSocket;
    }

    return 1;
}

void General::run()
{
    for (int i = 1; i < mMaxNumberSoldiers; i++)
        sendMessage(getOrder(), mSocketStorage[i]);
}

void General::sendMessage(char const *buffer, int socket)
{
    if(send(socket, buffer, sizeof(buffer), 0) < 0)
        // cerr << "error send()ing message" << endl;
        writeToFile("error send()ing message");
}

char const *General::getOrder()
{
    char const *msg;

    if(isTraitor){
        msg = ((rand() % 2) == 0) ? "1" : "0"; /* random the order because general is a traitor */
    } else {
        if(mOrder == Attack)
            msg = "0";
        else if(mOrder == Retreat)
            msg = "1";
    }

    return msg;
}

void General::cleanup()
{
    for (int i = 0; i < mMaxNumberSoldiers; i++) {
        if(i != mIdentifier)
            close(mSocketStorage[i]);
    }
    free(mSocketStorage);
}

void General::writeToFile(string output)
{
    file = fopen("log/general.txt", "a");
    if(file == nullptr){
        cerr << "Error: opening file." << endl;
        exit(1);
    }

    fprintf(file, "%s\n", output.c_str());
    fclose(file);
}

void General::printVariables() {
    cout << "ID: " << mIdentifier << endl;

    cout << "Port: " << mPort << endl;

    cout << "Max Lieutenant: " << mMaxNumberSoldiers << endl;

    if(mOrder == Attack){
        cout << "Order: Attack" << endl;
    } else {
        cout << "Order: Retreat" << endl;
    }

    if(isTraitor) {
        cout << "isTraitor: YES" << endl;
    } else {
        cout << "isTraitor: NO" << endl;
    }
}
