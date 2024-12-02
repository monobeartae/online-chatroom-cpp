#include "User.h"

User::User() : socket_id(0), name("Unknown")
{
}

User::User(int _id) : socket_id(_id), name("Unkown")
{
}

User::User(int _id, std::string _name) : socket_id(_id), name(_name)
{
}

int User::GetID()
{
    return socket_id;
}

std::string User::GetName()
{
    return name;
}

void User::SetID(int _id)
{
    socket_id = _id;
}

void User::SetName(std::string _name)
{
    name = _name;
}
