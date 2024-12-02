#include "PacketManager.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "UserManager.h"


std::vector<int> PacketManager::GetTargetSockets(char Packet[])
{

    std::vector<int> targets;
    packet_parser_get_data(Packet, "Target", targets);

    return targets;
}

char* PacketManager::GetClientIDPacket(int socket_id)
{
    char toSend[BUFSIZE];
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    packet_add_data(packet, "SOCKETID", socket_id);
  
    packet_encode(toSend, BUFSIZE, "SOCKET", packet, strlen(packet));
    return toSend;
}

void PacketManager::GetNewContactPacket(char recvPacket[], int clientSocket, char toSendPacket[])
{
    std::string username;
    packet_parser_get_data(recvPacket, "ClientName", username);

    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    packet_add_data(packet, "SOCKETID", clientSocket);
    packet_add_data(packet, "ClientName", username.c_str());

    packet_encode(toSendPacket, BUFSIZE, "CONTACT", packet, strlen(packet));
    UserManager::AddUser(clientSocket, username);
    std::cout << "Adding User to SERVER UserList" << std::endl;
}

void PacketManager::GetAllClientsPacket(char Packet[])
{
   
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
    std::vector<User*> usersList = UserManager::GetUserList();
    for (int i = 0; i < usersList.size(); i++)
    {
        packet_add_data(packet, "SOCKETID", usersList.at(i)->GetID());
        packet_add_data(packet, "ClientName", usersList.at(i)->GetName().c_str());
        std::cout << "**SERVER SIDE: Adding Existing User to Packet: " << usersList.at(i)->GetName() << std::endl;
    }

    packet_encode(Packet, BUFSIZE, "CONTACT", packet, strlen(packet));
   
}

void PacketManager::GetDeleteUserPacket(char Packet[], int socketID)
{
    char packet[BUFSIZE];
    memset(packet, '\0', BUFSIZE);
   
    packet_add_data(packet, "SOCKETID", socketID);

    packet_encode(Packet, BUFSIZE, "DELETEUSER", packet, strlen(packet));
}

void PacketManager::DecodePacket(char* recPacket, char decodedPacket[], char PacketID[])
{
    std::cout << "RECEIVED PACKET, CHECKING TYPE..." << std::endl;
   
    packet_decode(recPacket, PacketID, decodedPacket);
    std::cout << "Packet Decoded: " << decodedPacket << std::endl;

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
