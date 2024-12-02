#pragma once
#include "SingletonTemplate.h"
#include <vector>
#include <string>
#include <iostream>

struct Text
{
	std::string senderName;
	std::string text;
	//char* time;
	
	Text(std::string _name, std::string _message)// char* _time)
	{
		text = _message;
		//time = _time;
		senderName = _name;
	}
};


class Contact
{
public:
	Contact() : client_id(-1), name("Unknown")
	{
	}
	Contact(int _id, std::string _name) : client_id(_id), name(_name)
	{

	}
	int GetID()
	{
		return client_id;
	}
	std::string GetName()
	{
		return name;
	}
	void SetName(std::string _name)
	{
		name = _name;
	}
	void SetID(int _id)
	{
		client_id = _id;
	}
	
private:
	int client_id;
	std::string name;
};

class Chat
{
public:
	Chat(std::string _name) :name(_name)
	{

	}
	std::vector<Contact*> GetContacts()
	{
		return contacts;
	}
	std::vector<Text*> GetChatLog()
	{
		return chatLog;
	}
	std::string GetName()
	{
		return name;
	}
	void AddContact(Contact* who)
	{
		contacts.push_back(who);
	}
	void AddMessage(std::string _name, std::string text)
	{
		chatLog.push_back(new Text(_name, text));
	}

	void RemoveContact(Contact* who)
	{
		for (int i = 0; i < contacts.size(); i++)
		{
			if (contacts.at(i) == who)
			{
				std::cout << "Found Contact " << who->GetName() << " in Chat " << name << ". Removing Contact now." << std::endl;
				//delete contacts.at(i);
				contacts.at(i) = nullptr;
			}
		}
		contacts.erase(std::remove(begin(contacts), end(contacts), nullptr),
			end(contacts));
	}

	int GetNumMembers()
	{
		return contacts.size();
	}
	Contact* FindContactByID(int _id)
	{
		for (int i = 0; i < contacts.size(); i++)
		{
			if (contacts.at(i)->GetID() == _id)
				return contacts.at(i);
		}
		return nullptr;
	}
protected:
	std::string name;
	std::vector<Contact*> contacts;
	std::vector<Text*> chatLog;
};

class ChatInfo : public CSingletonTemplate<ChatInfo>
{
	friend CSingletonTemplate<ChatInfo>;
public:
	ChatInfo();

	void InitChats();

	void AddChat(std::string _name, int num_memb, std::vector<std::string> contact_namelist);
	void AddChat(std::string _name, int num_memb, std::vector<int> contact_list);
	void AddChat(std::string _name, std::string contact_name);
	void AddChat(std::string _name);
	void AddContact(int id, std::string name);
	void AddMessage(std::string name, std::string text);
	void AddMessageToChat(std::string name, std::string chatName, std::string text);
	void AddContactToChat(std::string contactName, std::string chatName);

	void RemoveContact(int _id);

	Contact* GetMyContact();
	Chat* GetCurrentChat();
	std::vector<Contact*> GetContactsList();
	std::vector<Chat*> GetChatsList();

	void SetMyContactName(std::string name);
	void SetMyContactID(int _id);
	void SetCurrentChat(std::string name);

	int FindContactIDByName(std::string name);
	std::string FindContactNameByID(int id);
	Chat* FindChat(std::string _name);

protected:
	Chat* currChat;
	Contact* myContact;
	std::vector<Contact*> contactList;
	std::vector<Chat*> chatList;

	Contact* FindContact(std::string _name);
	Contact* FindContact(int _id);

};