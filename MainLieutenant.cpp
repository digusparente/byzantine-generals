#include <iostream>

#define SERVER_PORT 31337 /* TODO: receive through command line? */

#include "Lieutenant.h"

using namespace std;


int main(int argc, char const *argv[])
{
    if(argc != 3)
    {
        cerr << "Usage: " << argv[0] << "<host_info-identifier> <max-number-lieutenant>" << endl;
        exit(0);
    }

    uint32_t identifier = atoi(argv[1]);
    int max_lieutenant = atoi(argv[2]);

    Lieutenant lieutenant(identifier, SERVER_PORT, max_lieutenant, false);

    lieutenant.init();

    cout << "\nFIM." << endl;

    return 0;
}