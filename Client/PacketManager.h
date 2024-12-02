#pragma once

#define BUFSIZE     1024

#include <string>
#include <vector>

class PacketManager
{
public:
    static char* GetMessagePacket();
    static char* GetNamePacket();
    static char* GetCreateChatPacket();
    static void DecodePacket(char* recPacket);
protected:
    // ADD DATA INTO PACKETSTRING
    static int packet_add_data(char Buffer[], const char DataName[], const int Value);
    static int packet_add_data(char Buffer[], const char DataName[], const char Value[]);

    // READ DATA FROM PACKETSTRING
    static int packet_parser_get_data(const char Packet[], const char DataName[], std::string& DataString); // Retrieve a string
    static int packet_parser_get_data(const char Packet[], const char DataName[], int& Data); // Retrieve an int
    static int packet_parser_get_data(const char Packet[], const char DataName[], std::vector<int>& Data); // Retrieve multiple ints
    static int packet_parser_get_data(const char Packet[], const char DataName[], std::vector<std::string>& Data); // Retrieve multiple strings

    // ?????
    static int packet_parser_data(const char Packet[], const char DataName[]);
    static int packet_parser_data(const char Packet[], const char DataName[], char Buffer[], int& BufferSize);

    // ENCODE DECODE PACKET STRING TO PACKET,  PACKET TO PACKET STRING
    static  int packet_encode(char Packet[], const int MaxBufferSize, const char PacketID[], const char PacketData[], int PacketLength);
    static  int packet_decode(const char Packet[], char PacketID[], char PacketData[]);
};

