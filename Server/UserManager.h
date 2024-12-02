#pragma once

#include <vector>

#include "User.h"

class UserManager
{
public:
	static void AddUser(int id, std::string name);
	static void RemoveUser(int id);
	static std::vector<User*> GetUserList();


protected:
	static std::vector<User*> userList;
};

