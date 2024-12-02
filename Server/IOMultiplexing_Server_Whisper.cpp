#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include <iostream>

#include "PacketManager.h"
#include "UserManager.h"

/// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define BUFSIZE     1024
#define PORT_NUMBER 7890


void send_socket_id_packet(SOCKET ClientSocket)
{
	send(ClientSocket, PacketManager::GetClientIDPacket(ClientSocket), strlen(PacketManager::GetClientIDPacket(ClientSocket)), 0);
}

void send_users_packet(SOCKET ClientSocket)
{
	std::cout << "Sending New User list of all logged in contacts" << std::endl;
	char packet[BUFSIZE];
	PacketManager::GetAllClientsPacket(packet);
	send(ClientSocket, packet, strlen(packet), 0);
}

void send_delete_packet(fd_set Fds, SOCKET toDeleteSocket)
{
	std::cout << "Sending delete user packet to logged in contacts" << std::endl;
	char packet[BUFSIZE];
	PacketManager::GetDeleteUserPacket(packet, toDeleteSocket);
	for (int i = 0; i < Fds.fd_count; i++)
	{
		send(Fds.fd_array[i], packet, strlen(packet), 0);
	}
}

int main(int argc, char **argv)
{
	int          Port = PORT_NUMBER;
	WSADATA      WsaData;
	SOCKET       ServerSocket;
	SOCKADDR_IN  ServerAddr;

	unsigned int Index;
	int          ClientLen = sizeof(SOCKADDR_IN);
	SOCKET       ClientSocket;
	SOCKADDR_IN  ClientAddr;

	fd_set       ReadFds, TempFds;
	TIMEVAL      Timeout; // struct timeval timeout;

	char         Message[BUFSIZE];
	int          Return;

	if (2 == argc)
	{
		Port = atoi(argv[1]);
	}
	printf("Using port number : [%d]\n", Port);

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		printf("WSAStartup() error!\n");
		return 1;
	}

	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == ServerSocket)
	{
		printf("socket() error\n");
		return 1;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR *)&ServerAddr,
		sizeof(ServerAddr)))
	{
		printf("bind() error\n");
		return 1;
	}

	if (SOCKET_ERROR == listen(ServerSocket, 5))
	{
		printf("listen() error\n");
		return 1;
	}

	FD_ZERO(&ReadFds);
	FD_SET(ServerSocket, &ReadFds);

	while (1)
	{
		TempFds = ReadFds;
		Timeout.tv_sec = 5;
		Timeout.tv_usec = 0;

		if (SOCKET_ERROR == (Return = select(0, &TempFds, 0, 0, &Timeout)))
		{ // Select() function returned error.
			printf("select() error\n");
			return 1;
		}
		if (0 == Return)
		{ // Select() function returned by timeout.
			//printf("Select returned timeout.\n");
		}
		else if (0 > Return)
		{
			printf("Select returned error!\n");
		}
		else
		{
			for (Index = 0; Index < TempFds.fd_count; Index++)
			{
				if (TempFds.fd_array[Index] == ServerSocket)
				{ // New connection requested by new client.
					ClientSocket = accept(ServerSocket, (SOCKADDR *)&ClientAddr, &ClientLen);
					FD_SET(ClientSocket, &ReadFds);
					printf("New Client Accepted : Socket Handle [%d]\n", ClientSocket);

					send_socket_id_packet(ClientSocket);
				}
				else
				{ // Something to read from socket.
                    memset(Message, '\0', BUFSIZE);
					Return = recv(TempFds.fd_array[Index], Message, BUFSIZE, 0);
					if (0 == Return)
					{ // Connection closed message has arrived.
						closesocket(TempFds.fd_array[Index]);
						printf("Connection closed :Socket Handle [%d]\n", TempFds.fd_array[Index]);
						UserManager::RemoveUser(TempFds.fd_array[Index]);
						FD_CLR(TempFds.fd_array[Index], &ReadFds);
						send_delete_packet(ReadFds, TempFds.fd_array[Index]);

					}
					else if (0 > Return)
					{ // recv() function returned error.
						closesocket(TempFds.fd_array[Index]);
						printf("Exceptional error :Socket Handle [%d]\n", TempFds.fd_array[Index]);
						UserManager::RemoveUser(TempFds.fd_array[Index]);
						FD_CLR(TempFds.fd_array[Index], &ReadFds);
						send_delete_packet(ReadFds, TempFds.fd_array[Index]);
						
					}
					else
					{ 
						// PACKET RECEIVED FROM CLIENT
						char packetType[BUFSIZE];
						char packet[BUFSIZE];
						PacketManager::DecodePacket(Message, packet, packetType);
						std::string pType(packetType);
						if (pType == "MSG" || pType == "CREATECHAT") // MESSAGE/CREATE CHAT RECEIVED, JUST NEED TO HELP SEND EXACT SAME PACKET TO SPECIFIED SOCKET TARGETS
						{
							std::cout << "PACKET TYPE IS: MSG/CREATECHAT, Sending Message to Target Clients now" << std::endl;
							std::vector<int> recv_list = PacketManager::GetTargetSockets(packet);
							for (int i = 0; i < recv_list.size(); i++)
							{
								std::cout << "Sent Packet " << Message << "to: " << recv_list.at(i) << std::endl;
								send(recv_list.at(i), Message, strlen(Message), 0);
							}
						}
						else if (pType == "CLIENTNAME") // NEW USER LOGGED IN, NAME RECEIVED, SEND OTHER CONTACTS TO NEW USER, SEND NEW USER TO OTHER CONTACTS
						{
							std::cout << "PACKET TYPE IS: CLIENTNAME, New User has Entered Chat" << std::endl;
							// Send all Contacts to Client
							send_users_packet(TempFds.fd_array[Index]);
							// Send New Contact to all other Logged In Users
							char contactPacket[BUFSIZE];
							PacketManager::GetNewContactPacket(packet, TempFds.fd_array[Index], contactPacket);
							std::vector<User*> userList = UserManager::GetUserList();
							for (int i = 0; i < userList.size(); i++)
							{
								if (userList.at(i)->GetID() != TempFds.fd_array[Index])
								{
									send(userList.at(i)->GetID(), contactPacket, strlen(contactPacket), 0);
									std::cout << "Sending New User to " << ReadFds.fd_array[i] << std::endl;
								}
							}
						}
					}
				}
			}
		}
	}

	WSACleanup();

	return 0;
}
