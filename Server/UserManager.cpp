#include "UserManager.h"

std::vector<User*> UserManager::userList;

void UserManager::AddUser(int id, std::string name)
{
    userList.push_back(new User(id, name));
}

void UserManager::RemoveUser(int id)
{
    for (int i = 0; i < userList.size(); i++)
    {
        if (userList.at(i)->GetID() == id)
        {
            delete userList.at(i);
            userList.at(i) = nullptr;
        }
    }
    userList.erase(std::remove(begin(userList), end(userList), nullptr),
        end(userList));
}

std::vector<User*> UserManager::GetUserList()
{
    return userList;
}
