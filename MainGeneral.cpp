#include <iostream>
#include "Order.h"
#include "General.h"

using namespace std;

int main(int argc, char const *argv[])
{
	if(argc != 4)
	{
		cerr << "Usage: " << argv[0] << " <max-number-lieutenant> <order> <is-traitor>" << endl;
		exit(1);
	}

	uint32_t identifier = 1;
	uint16_t port = 31337;

	int maxLieutenant = atoi(argv[1]);

	Order order;
    order = strcmp(argv[2], "0") == 0 ? Attack : Retreat;
	
	bool isTraitor;
    isTraitor = strcmp(argv[3], "0") == 0 ? false : true;

    General general(identifier, port, maxLieutenant, order, isTraitor);

//    general.printVariables();
    general.init();
    general.run();

	return 0;
}