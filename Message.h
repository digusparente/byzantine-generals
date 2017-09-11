//
// Created by root on 01/09/17.
//

#ifndef BYZANTINE_GENERALS_MESSAGE_H
#define BYZANTINE_GENERALS_MESSAGE_H

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>

#include "Order.h"

using namespace std;


class Message
{
public:
    Message(Order order, string path);
    Message();

    void appendIdPath(char id);

    char * serialize();
    void deserialize(char *buffer);

    Order get_order();
    void set_order(Order order);

    string get_path();
    void set_path(string path);

private:

    Order m_order;
    string m_path;
};


#endif //BYZANTINE_GENERALS_MESSAGE_H