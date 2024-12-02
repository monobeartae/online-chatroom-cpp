#pragma once

#define BUFSIZE     1024

#include <string>
#include <vector>

class PacketManager
{
public:
    static std::vector<int> GetTargetSockets(char Packet[]);
    static char* GetClientIDPacket(int socket_id);
    static void GetNewContactPacket(char recvPacket[], int clientSocket, char toSendPacket[]);
    static void GetAllClientsPacket(char Packet[]);
    static void GetDeleteUserPacket(char Packet[], int socketID);

    //static char* GetNewClientPacket(int socket_id, std::string name);

    static void DecodePacket(char* recPacket, char decodedPacket[], char PacketID[]);
protected:
    // ADD DATA INTO PACKETSTRING
    static int packet_add_data(char Buffer[], const char DataName[], const int Value);
    static int packet_add_data(char Buffer[], const char DataName[], const char Value[]);

    // READ DATA FROM PACKETSTRING
    static int packet_parser_get_data(const char Packet[], const char DataName[], std::string& DataString);
    static int packet_parser_get_data(const char Packet[], const char DataName[], int& Data);
    static int packet_parser_get_data(const char Packet[], const char DataName[], std::vector<int>& Data);

    // ?????
    static int packet_parser_data(const char Packet[], const char DataName[]);
    static int packet_parser_data(const char Packet[], const char DataName[], char Buffer[], int& BufferSize);

    // ENCODE DECODE PACKET STRING TO PACKET,  PACKET TO PACKET STRING
    static  int packet_encode(char Packet[], const int MaxBufferSize, const char PacketID[], const char PacketData[], int PacketLength);
    static  int packet_decode(const char Packet[], char PacketID[], char PacketData[]);
};

