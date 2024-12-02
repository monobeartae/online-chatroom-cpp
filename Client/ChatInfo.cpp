#include "ChatInfo.h"

#include <iostream>

ChatInfo::ChatInfo() : currChat(NULL), myContact(nullptr)
{
}

void ChatInfo::InitChats()
{
	AddChat("Global");
	AddContactToChat(myContact->GetName(), "Global");
	SetCurrentChat("Global");
}

void ChatInfo::AddChat(std::string _name, int num_memb, std::vector<std::string> contact_namelist)
{
	Chat* chat = new Chat(_name);
	for (int i = 0; i < num_memb; i++)
	{
		chat->AddContact(FindContact(contact_namelist.at(i)));
	}
	chatList.push_back(chat);
}

void ChatInfo::AddChat(std::string _name, int num_memb, std::vector<int> contact_list)
{
	Chat* chat = new Chat(_name);
	for (int i = 0; i < num_memb; i++)
	{
		chat->AddContact(FindContact(contact_list.at(i)));
	}
	chatList.push_back(chat);
}

void ChatInfo::AddChat(std::string _name, std::string contact_name)
{
	Chat* chat = new Chat(_name);
	chat->AddContact(FindContact(contact_name));
	chatList.push_back(chat);
}

void ChatInfo::AddChat(std::string _name)
{
	chatList.push_back(new Chat(_name));
}

void ChatInfo::AddContact(int id, std::string name)
{
	Contact* newUser = new Contact(id, name);
	contactList.push_back(newUser);
	FindChat("Global")->AddContact(newUser);
	
	std::cout << "Adding New Contact..Name: " << name << "Socket ID: " << id << std::endl;
}

void ChatInfo::AddMessage(std::string name, std::string text)
{
	
	currChat->AddMessage(name, text);
}

void ChatInfo::AddMessageToChat(std::string name, std::string chatName, std::string text)
{
	FindChat(chatName)->AddMessage(name, text);
}

void ChatInfo::AddContactToChat(std::string contactName, std::string chatName)
{
	Chat* chat = FindChat(chatName);
	chat->AddContact(FindContact(contactName));
}

void ChatInfo::RemoveContact(int _id)
{
	for (int i = 0; i < chatList.size(); i++)
	{
		Chat* chat = chatList.at(i);
		Contact* user = chat->FindContactByID(_id);
		if (user != nullptr)
		{
			chat->RemoveContact(user);
			if (chat->GetNumMembers() == 1 && chat->GetName() != "Global")
			{
				std::cout << "Chat " << chat->GetName() << "Has only one remaining member, deketing chat" << std::endl;
				if (currChat == chat)
					currChat = FindChat("Global");
				delete chat;
				chatList.at(i) = nullptr;
				chatList.erase(std::remove(begin(chatList), end(chatList), nullptr),
					end(chatList));
			}
		}
	}

	for (int i = 0; i < contactList.size(); i++)
	{
		if (contactList.at(i)->GetID() == _id)
		{
			delete contactList.at(i);
			contactList.at(i) = nullptr;
			contactList.erase(std::remove(begin(contactList), end(contactList), nullptr),
				end(contactList));
		}
	}

}

Contact* ChatInfo::GetMyContact()
{
	return myContact;
}

Chat* ChatInfo::GetCurrentChat()
{
	return currChat;
}

std::vector<Contact*> ChatInfo::GetContactsList()
{
	return contactList;
}

std::vector<Chat*> ChatInfo::GetChatsList()
{
	return chatList;
}

void ChatInfo::SetMyContactName(std::string name)
{
	if (myContact == nullptr)
	{
		myContact = new Contact(-1, name);

		// FOR TESTING
		contactList.push_back(myContact);
		return;
	}
	myContact->SetName(name);
	
}

void ChatInfo::SetMyContactID(int _id)
{
	if (myContact == nullptr)
	{
		myContact = new Contact(_id, "Unknown");

		// FOR TESTING
		contactList.push_back(myContact);
		return;
	}
	myContact->SetID(_id);
}

void ChatInfo::SetCurrentChat(std::string name)
{
	for (int i = 0; i < chatList.size(); i++)
	{
		if (chatList.at(i)->GetName() == name)
		{
			printf("Found Chat, Setting Current Chat...");
			currChat = chatList.at(i);
			return;
		}
	}
	printf("Contact with this name is not found.");
}

int ChatInfo::FindContactIDByName(std::string name)
{
	return FindContact(name)->GetID();
}

std::string ChatInfo::FindContactNameByID(int id)
{
	return FindContact(id)->GetName();
}

Contact* ChatInfo::FindContact(std::string _name)
{
	for (int i = 0; i < contactList.size(); i++)
	{
		Contact* contact = contactList.at(i);
		if (contact->GetName() == _name)
			return contact;
	}
	return nullptr;
}

Contact* ChatInfo::FindContact(int _id)
{
	for (int i = 0; i < contactList.size(); i++)
	{
		Contact* contact = contactList.at(i);
		if (contact->GetID() == _id)
			return contact;
	}
	return nullptr;
}

Chat* ChatInfo::FindChat(std::string _name)
{
	for (int i = 0; i < chatList.size(); i++)
	{
		Chat* chat = chatList.at(i);
		if (chat->GetName() == _name)
			return chat;
	}
	return nullptr;
}
