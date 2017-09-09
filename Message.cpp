//
// Created by root on 01/09/17.
//

#include "Message.h"

Message::Message(Order order, string path)
{
    set_order(order);
    set_path(path);
}

Message::Message() {

}

void Message::appendIdPath(char id) {
    stringstream ss;
    string temp = get_path();

    ss << id;
}

/* serialization functions */

char * Message::serialize() {
    char *buffer;
    size_t path_len = strlen(m_path.c_str());

    buffer = (char *) malloc(sizeof(char) + (sizeof(char) * path_len));

    if(m_order == Attack)
    {
        memcpy(buffer, "1", sizeof(char));
    } else {
        memcpy(buffer, "0", sizeof(char));
    }

    memcpy(buffer + sizeof(char), m_path.c_str(), path_len);

    return buffer;
}

void Message::deserialize(char *buffer) {
    char *order = (char *) malloc(sizeof(char) * 1);
    size_t path_len = strlen(buffer) - sizeof(char);
    char *path = (char *) malloc(sizeof(char) * path_len);

    stringstream ss;

    memcpy(order, buffer, sizeof(char));
    memcpy(path, buffer + sizeof(char), path_len);

    if(*order == '1')
    {
        m_order = Attack;
    }
    else
    {
        m_order = Retreat;
    }

    ss << path;
    ss >> m_path;
}

/* getters and setters */

Order Message::get_order()
{
    return m_order;
}

void Message::set_order(Order order)
{
    m_order = order;
}

string Message::get_path()
{
    return m_path;
}

void Message::set_path(string path)
{
    m_path = path;
}
