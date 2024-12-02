#pragma once
#include <string>

class User
{
public:
	User();
	User(int _id);
	User(int _id, std::string _name);

	int GetID();
	std::string GetName();
	void SetID(int _id);
	void SetName(std::string _name);
private:
	int socket_id;
	std::string name;
};

