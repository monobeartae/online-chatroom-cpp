#include "PacketManager.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "ChatInfo.h"
#include "ChatUI.h"
#include "LoginUI.h"

char* PacketManager::GetMessagePacket()
{
    char toSend[BUFSIZE];
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    packet_add_data(packet, "SenderID", ChatInfo::GetInstance()->GetMyContact()->GetID());
    packet_add_data(packet, "ChatName", ChatInfo::GetInstance()->GetCurrentChat()->GetName().c_str());
    packet_add_data(packet, "Message", ChatUI::GetInstance()->GetEnteredMessage());
    std::vector<Contact*> targetList = ChatInfo::GetInstance()->GetCurrentChat()->GetContacts();
    for (int i = 0; i < targetList.size(); i++)
    {
        packet_add_data(packet, "Target", targetList.at(i)->GetID());
        std::cout << "**CLIENT: Adding Conact: " << targetList.at(i)->GetName() << "To Message Packet Targets" << std::endl;
    }
    packet_encode(toSend, BUFSIZE, "MSG", packet, strlen(packet));
    return toSend;
}

char* PacketManager::GetNamePacket()
{
    char toSend[BUFSIZE];
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    packet_add_data(packet, "ClientName", LoginUI::GetInstance()->GetEnteredName());
   
    packet_encode(toSend, BUFSIZE, "CLIENTNAME", packet, strlen(packet));
    return toSend;
}

char* PacketManager::GetCreateChatPacket()
{
    char toSend[BUFSIZE];
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    packet_add_data(packet, "ChatName", ChatUI::GetInstance()->GetEnteredGroupName());
    std::vector<std::string> targetList = ChatUI::GetInstance()->GetSelectedMembers();
    for (int i = 0; i < targetList.size(); i++)
    {
        int id = ChatInfo::GetInstance()->FindContactIDByName(targetList.at(i));
        packet_add_data(packet, "Target", id);
        std::cout << "**CLIENT: Adding Member: " << targetList.at(i) << "To Create Chat Packet Targets" << std::endl;
    }
    packet_encode(toSend, BUFSIZE, "CREATECHAT", packet, strlen(packet));
    return toSend;
}

void PacketManager::DecodePacket(char* recPacket)
{
    std::cout << "**RECEIVED PACKET FROM SERVER, CHECKING TYPE..." << std::endl;
    char packet_id[BUFSIZE];
    char packet[BUFSIZE];
 
    packet_decode(recPacket, packet_id, packet);
    std::string packetType(packet_id);
    if (packetType == "MSG")
    {
        std::cout << "RECEIVED MESSAGE PACKET, UPDATING CHATINFO" << std::endl;
        std::string text, chatName;
        int sender_id;
        std::vector<int> users;
        packet_parser_get_data(packet, "SenderID", sender_id);
        packet_parser_get_data(packet, "Message", text);
        packet_parser_get_data(packet, "ChatName", chatName);
        packet_parser_get_data(packet, "Target", users);

        if (users.size() == 2 && chatName != "Global" && sender_id != ChatInfo::GetInstance()->GetMyContact()->GetID())
        {
            chatName = ChatInfo::GetInstance()->FindContactNameByID(sender_id);
        }

        if (ChatInfo::GetInstance()->FindChat(chatName) == nullptr)
            ChatInfo::GetInstance()->AddChat(chatName, users.size(), users);

      
        ChatInfo::GetInstance()->AddMessageToChat(ChatInfo::GetInstance()->FindContactNameByID(sender_id), chatName, text);

    }
    else if (packetType == "CREATECHAT")
    {
        std::string group_name;
        std::vector<int> membersList;

        packet_parser_get_data(packet, "ChatName", group_name);
        packet_parser_get_data(packet, "Target", membersList);

        ChatInfo::GetInstance()->AddChat(group_name, membersList.size(), membersList);
        ChatInfo::GetInstance()->SetCurrentChat(group_name);
    }
    else if (packetType == "SOCKET")
    {
        std::cout << "RECEIVED SOCKET PACKET, Setting Client ID" << std::endl;
        int socket_id;
        packet_parser_get_data(packet, "SOCKETID", socket_id);
        ChatInfo::GetInstance()->SetMyContactID(socket_id);
    }
    else if (packetType == "CONTACT")
    {
        std::cout << "RECEIVED CONTACT PACKET, UPDATING CONTACTS" << std::endl;
        std::vector<std::string> contactName;
        std::vector<int> contactID;
        packet_parser_get_data(packet, "SOCKETID", contactID);
        packet_parser_get_data(packet, "ClientName", contactName);
        if (contactID.size() != contactName.size())
            std::cout << "ContactList Packet Error: Contact ID and Name Count does not match, is the packet sent correct?" << std::endl;
        for (int i = 0; i < contactID.size(); i++)
        {
            std::cout << "New User has entered, adding contact and adding to global chat" << std::endl;
            ChatInfo::GetInstance()->AddContact(contactID.at(i), contactName.at(i));
        }
    }
    else if (packetType == "DELETEUSER")
    {
        std::cout << "RECEIVED DELETEUSER PACKET, REMOVING CONTACT FROM CHATS AND CONTACTS" << std::endl;
        int socketID;
        packet_parser_get_data(packet, "SOCKETID", socketID);
        ChatInfo::GetInstance()->RemoveContact(socketID);
    }
}

int PacketManager::packet_add_data(char Buffer[], const char DataName[], const int Value)
{
   // sprintf_s(Buffer, _msize(Buffer), "%s %s=%d", Buffer, DataName, Value);
    sprintf(Buffer, "%s %s=%d", Buffer, DataName, Value);
    return strlen(Buffer);
}

int PacketManager::packet_add_data(char Buffer[], const char DataName[], const char Value[])
{
    //sprintf_s(Buffer, _msize(Buffer), "%s %s=\"%s\"", Buffer, DataName, Value);
    sprintf(Buffer, "%s %s=\"%s\"", Buffer, DataName, Value);
    return strlen(Buffer);
}

int PacketManager::packet_parser_get_data(const char Packet[], const char DataName[], std::string& DataString)
{
    const char* Str;

    if (NULL == (Str = strstr(Packet, DataName)))
    {
        return 0;
    }

    int Len = strlen(Str);
    const char* Pos = Str;
    for (int i = 0; i < Len; ++i)
    {
        if ('=' == *(Pos + i))
        {
            for (int j = (i + 1); j < Len; ++j)
            {
                if ((' ' == *(Pos + j)) || ('\n' == *(Pos + j)) || ('\0' == *(Pos + j)) || '>' == *(Pos + j))
                    break;
                if ('"' == *(Pos + j))
                    continue;
                DataString.push_back(*(Pos + j));
            }
            return atoi(DataString.c_str());
        }
    }

    return 0;
}

int PacketManager::packet_parser_get_data(const char Packet[], const char DataName[], int& Data)
{
    const char* Str;

    if (NULL == (Str = strstr(Packet, DataName)))
    {
        return 0;
    }

    std::string DataString;

    int Len = strlen(Str);
    const char* Pos = Str;
    for (int i = 0; i < Len; ++i)
    {
        if ('=' == *(Pos + i))
        {
            for (int j = (i + 1); j < Len; ++j)
            {
                if ((' ' == *(Pos + j)) || ('\n' == *(Pos + j)) || ('\0' == *(Pos + j)) || '>' == *(Pos + j))
                    break;
                if ('"' == *(Pos + j))
                    continue;
                DataString.push_back(*(Pos + j));
            }
            Data = atoi(DataString.c_str());
            return 0;
        }
    }

    return 0;
}

int PacketManager::packet_parser_get_data(const char Packet[], const char DataName[], std::vector<std::string>& Data)
{
    std::string DataString = "";
    std::string packet_string(Packet);
    size_t index = packet_string.find(DataName);
    while (index != std::string::npos)
    {
        int i, j;
        for (i = index; i < packet_string.length(); i++)
        {
            if ('=' == packet_string[i])
            {
                for (j = i + 1; j < packet_string.length(); j++)
                {
                    if ((' ' == packet_string[j]) || ('\n' == packet_string[j]) || ('\0' == packet_string[j]) || '>' == packet_string[j])
                        break;
                    if ('"' == packet_string[j])
                        continue;
                    DataString.push_back(packet_string[j]);
                }
                break;
            }

        }
        Data.push_back(DataString);
        DataString = "";
        packet_string = packet_string.substr(j, packet_string.length() - j);
        index = packet_string.find(DataName);
    }

    return 0;
}

int PacketManager::packet_parser_get_data(const char Packet[], const char DataName[], std::vector<int>& Data)
{
    std::string DataString = "";
    std::string packet_string(Packet);
    size_t index = packet_string.find(DataName);
    while (index != std::string::npos)
    {
        int i, j;
        for (i = index; i < packet_string.length(); i++)
        {
            if ('=' == packet_string[i])
            {
                for (j = i + 1; j < packet_string.length(); j++)
                {
                    if ((' ' == packet_string[j]) || ('\n' == packet_string[j]) || ('\0' == packet_string[j]) || '>' == packet_string[j])
                        break;
                    if ('"' == packet_string[j])
                        continue;
                    DataString.push_back(packet_string[j]);
                }
                break;
            }

        }
        Data.push_back(atoi(DataString.c_str()));
        std::cout << "Adding Target ID" << atoi(DataString.c_str()) << "to target List" << std::endl;
        DataString = "";
        packet_string = packet_string.substr(j, packet_string.length() - j);
        index = packet_string.find(DataName);
    }

    return 0;
}

int PacketManager::packet_parser_data(const char Packet[], const char DataName[])
{
    std::string DataString;
   // int ReturnLength;

    return packet_parser_get_data(Packet, DataName, DataString);
}

int PacketManager::packet_parser_data(const char Packet[], const char DataName[], char Buffer[], int& BufferSize)
{
    std::string DataString;
    int ReturnLength;

    ReturnLength = packet_parser_get_data(Packet, DataName, DataString);
    strcpy(Buffer, DataString.c_str());
    BufferSize = ReturnLength;

    return ReturnLength;
}

int PacketManager::packet_encode(char Packet[], const int MaxBufferSize, const char PacketID[], const char PacketData[], int PacketLength)
{
    int packetLength = strlen(PacketID) + strlen(PacketData) + 7;
    sprintf(Packet, "<%s %03d %s>", PacketID, PacketLength, PacketData);
    return packetLength;
}

int PacketManager::packet_decode(const char Packet[], char PacketID[], char PacketData[])
{
    int PacketDataLength;
    int PacketLength = strlen(Packet);
    int Pos = 1;

    for (int j = 0; Pos < PacketLength; ++Pos)
    {
        if (' ' == Packet[Pos])
        {
            PacketID[j] = '\0';
            ++Pos;
            break;
        }
        PacketID[j++] = Packet[Pos];
    }

    strcpy(PacketData, &Packet[Pos]);
    PacketDataLength = strlen(PacketData);

    return PacketDataLength;
}
