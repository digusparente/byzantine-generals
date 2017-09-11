#include <iostream>

#define SERVER_PORT 31337 /* TODO: receive through command line? */

#include "Soldier.h"

using namespace std;


int main(int argc, char const *argv[])
{
    if(argc != 5)
    {
        cerr << "Usage: " << argv[0] << "<identifier> <lieutenants> <order> <traitor>" << endl;
        exit(0);
    }

    uint32_t identifier = strtol(argv[1], (char **) nullptr, 10);
    uint16_t maxLieutenant = strtol(argv[2], (char **) nullptr, 10);

    Order order;
    if(strcmp(argv[3], "0") == 0)
        order = Wait;
    else if(strcmp(argv[3], "1") == 0)
        order = Attack;
    else if(strcmp(argv[3], "2") == 0)
        order = Retreat;

    bool isTraitor = strcmp(argv[4], "1") == 0 ? true : false;

    Soldier soldier(identifier, SERVER_PORT, maxLieutenant, order, isTraitor);

    soldier.init();
    soldier.run();

    cout << "\nFIM." << endl;

    return 0;
}