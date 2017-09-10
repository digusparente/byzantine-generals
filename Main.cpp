#include <iostream>

#define SERVER_PORT 31337 /* TODO: receive through command line? */

#include "Soldier.h"

using namespace std;


int main(int argc, char const *argv[])
{
    if(argc != 5)
    {
        cerr << "Usage: " << argv[0] << "<host_info-identifier> <max-number-lieutenant>" << endl;
        exit(0);
    }

    uint32_t identifier = atoi(argv[1]);
    int maxLieutenant = atoi(argv[2]);

    Order order = strcmp(argv[2], "0") == 0 ? Attack : Retreat;

    bool isTraitor = strcmp(argv[3], "1") == 0 ? true : false;

    Soldier soldier(identifier, SERVER_PORT, maxLieutenant, order, isTraitor);

    soldier.init();
    soldier.run();

    cout << "\nFIM." << endl;

    return 0;
}