/**
 Application.cpp
 @brief This file contains the Application class for this project, which initialises and control the OpenGL environment
*/
#include "Application.h"

//Include the standard C++ headers

#include <stdlib.h>

#include "Settings.h"
#include "MouseController.h"
#include "KeyboardController.h"
#include "ChatUI.h"
#include "LoginUI.h"
#include "PacketManager.h"

#include <string.h>
#include <conio.h>

/// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define BUFSIZE     1024
#define PORT_NUMBER 7890
#define IP_ADDRESS  "127.0.0.1"

// Include GLEW
#ifndef GLEW_STATIC
	#include <GL/glew.h>
	#define GLEW_STATIC
#endif

#include <iostream>
using namespace std;

// Include GLFW
#include <GLFW/glfw3.h>

/**
 @brief Define an error callback
 @param error The error code
 @param description The details about the error
 */
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

/**
 @brief Define the key input callback
 @param window The window to receive the the instructions
 @param key The key code
 @param scancode The scan code
 @param action The action to take
 @param mods The modifications
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	CKeyboardController::GetInstance()->Update(key, action);
}

/**
 @brief Callback function when the OpenGL window is repositioned
 @param window The window to receive the the instructions
 @param xpos integer value of the new x position of the window
 @param ypos integer value of the new y position of the window
*/
void repos_callback(GLFWwindow* window, int xpos, int ypos)
{
	// Store the window width position
	CSettings::GetInstance()->iWindowPosX = xpos;
	CSettings::GetInstance()->iWindowPosY = ypos;
	// Update the glViewPort
	glViewport(0, 0, CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight);
}

/**
 @brief Callback function when the OpenGL window is resized
 @param window The window to receive the the instructions
 @param w integer value of the new width of the window
 @param h integer value of the new height of the window
 */
void resize_callback(GLFWwindow* window, int width, int height)
{
	// Store the window width and height
	
	CSettings::GetInstance()->iWindowWidth = width;
	CSettings::GetInstance()->iWindowHeight = height;
	// Update the glViewPort
	glViewport(0, 0, width, height);
}

/**
 @brief Callback function whenever the mouse has events
 @param window The window to receive the the instructions
 @param button An integer value of the mouse button causing the event
 @param action An integer value of the action caused by the mouse button
 @param mods An integer value storing the mods of the event
 */
void MouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods)
{
	// Send the callback to the mouse controller to handle
	if (action == GLFW_PRESS)
		CMouseController::GetInstance()->UpdateMouseButtonPressed(button);
	else
		CMouseController::GetInstance()->UpdateMouseButtonReleased(button);
}

/**
 @brief Callback function whenever the mouse has events
 @param window The window to receive the the instructions
 @param xoffset A double value of the mouse scroll offset in the x-axis
 @param yoffset A double value of the mouse scroll offset in the y-axis
 */
void MouseScrollCallbacks(GLFWwindow* window, double xoffset, double yoffset)
{
	CMouseController::GetInstance()->UpdateMouseScroll(xoffset, yoffset);
}

/**
@brief Destructor
*/
Application::~Application(void)
{
}

/**
 @brief Initialise this class instance
 */
bool Application::Init(void)
{
	// glfw: initialize and configure
	// ------------------------------
	//Initialize GLFW
	if (!glfwInit())
	{
		// return false if the GLFW was not initialised successfully
		return false;
	}

	// Get the CSettings instance
	cSettings = CSettings::GetInstance();

	// Set the file location for the digital assets
	// This is backup, in case filesystem cannot find the current directory
	cSettings->logl_root = "C:/Users/tohdj/Documents/2021_2022_SEM1/DM2213 2D Game Creation/Teaching Materials/NYP_Framework";

	//Set the GLFW window creation hints - these are optional
	if (cSettings->bUse4XAntiliasing == true)
		glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	//Create a window and create its OpenGL context
	//cSettings->pWindow = glfwCreateWindow(	cSettings->iWindowWidth, cSettings->iWindowHeight, "NYP Framework", glfwGetPrimaryMonitor(), NULL);
	cSettings->pWindow = glfwCreateWindow(cSettings->iWindowWidth, cSettings->iWindowHeight, "NYP Framework", NULL, NULL);
	//If the window couldn't be created, then return false
	if (cSettings->pWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	// Set OpenGL window position
	glfwSetWindowPos(cSettings->pWindow, cSettings->iWindowPosX, cSettings->iWindowPosY);

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(cSettings->pWindow);

	//Sets the callback functions for GLFW
	//Set the window position callback function
	glfwSetWindowPosCallback(cSettings->pWindow, repos_callback);
	//Set the window resize resize callback function
	glfwSetWindowSizeCallback(cSettings->pWindow, resize_callback);
	//Set the frame buffer resize callback function
	glfwSetFramebufferSizeCallback(cSettings->pWindow, resize_callback);
	//Set the error callback function
	glfwSetErrorCallback(error_callback);
	// Set the keyboard callback function
	glfwSetKeyCallback(cSettings->pWindow, key_callback);
	//Set the mouse button callback function
	glfwSetMouseButtonCallback(cSettings->pWindow, MouseButtonCallbacks);
	//Set the mouse scroll callback function
	glfwSetScrollCallback(cSettings->pWindow, MouseScrollCallbacks);

	// Additional customisation for the GLFW environment
	// Disable the cursor
	if (cSettings->bDisableMousePointer == true)
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (cSettings->bShowMousePointer == false)
			glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glewExperimental is a variable that is already defined by GLEW. You must set it to GL_TRUE before calling glewInit(). 
	glewExperimental = GL_TRUE;
	// Initialize GLEW
	GLenum glewInitErr = glewInit();
	//If GLEW hasn't initialized, then return false
	if (glewInitErr != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInitErr));
		glfwTerminate();
		return false;
	}


	return true;
}

/**
@brief Run this class instance
*/ 
void Application::Run(void)
{
	
	double dElapsedTime = 0.0;
	double dDelayTime = 0.0;
	double dTotalFrameTime = 0.0;

	int          Port = PORT_NUMBER;
	char         IPAddress[16] = IP_ADDRESS;
	WSADATA      WsaData;
	SOCKET       ConnectSocket;
	SOCKADDR_IN  ServerAddr;

	int          ClientLen = sizeof(SOCKADDR_IN);

	fd_set       ReadFds, TempFds;
	TIMEVAL      Timeout; // struct timeval timeout;

	char         Message[BUFSIZE];
	int          MessageLen;
	int          Return;

	printf("Destination IP Address [%s], Port number [%d]\n", IPAddress, Port);

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		printf("WSAStartup() error!");
		return;
	}

	ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == ConnectSocket)
	{
		printf("socket() error");
		return;
	}

	///----------------------
	/// The sockaddr_in structure specifies the address family,
	/// IP address, and port of the server to be connected to.
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = inet_addr(IPAddress);

	///----------------------
	/// Connect to server.
	Return = connect(ConnectSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (Return == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		printf("Unable to connect to server: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	FD_ZERO(&ReadFds);
	FD_SET(ConnectSocket, &ReadFds);


	memset(Message, '\0', BUFSIZE);
	MessageLen = 0;

	LoginUI::GetInstance()->Init();
	ChatInfo::GetInstance()->SetMyContactID(-1);

	// LOGIN UI WHILE LOOP
	while (!glfwWindowShouldClose(cSettings->pWindow)
		&& (!CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE)))
	{
		LoginUI::GetInstance()->Update();
		if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_ENTER)
			&& LoginUI::GetInstance()->GetNameLength() > 0)
		{
			Return = send(ConnectSocket, PacketManager::GetNamePacket(), strlen(PacketManager::GetNamePacket()), 0);
			if (Return == SOCKET_ERROR)
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return;
			}
			std::cout << "**SENDING PACKET TO SERVER" << std::endl;
			ChatInfo::GetInstance()->SetMyContactName(LoginUI::GetInstance()->GetEnteredName());
			ChatInfo::GetInstance()->InitChats();
			LoginUI::GetInstance()->ClearBar();
		}
		else
		{
			TempFds = ReadFds;
			Timeout.tv_sec = 0;
			Timeout.tv_usec = 1000;

			if (SOCKET_ERROR == (Return = select(0, &TempFds, 0, 0, &Timeout)))
			{ // Select() function returned error.
				closesocket(ConnectSocket);
				printf("select() error\n");
				return;
			}
			else if (0 > Return)
			{
				printf("Select returned error!\n");
			}
			else if (0 < Return)
			{
				memset(Message, '\0', BUFSIZE);
				printf("Select Processed... Something to read\n");
				Return = recv(ConnectSocket, Message, BUFSIZE, 0);
				if (0 > Return)
				{ // recv() function returned error.
					closesocket(ConnectSocket);
					printf("Exceptional error :Socket Handle [%d]\n", ConnectSocket);
					return;
				}
				else if (0 == Return)
				{ // Connection closed message has arrived.
					closesocket(ConnectSocket);
					printf("Connection closed :Socket Handle [%d]\n", ConnectSocket);
					return;
				}
				else
				{ 
					// Message received.
					PacketManager::DecodePacket(Message);
				}
			}
		}
		LoginUI::GetInstance()->Render();

		if (ChatInfo::GetInstance()->GetMyContact()->GetID() != -1
			&& ChatInfo::GetInstance()->GetMyContact()->GetName() != "Unknown")
		{
			LoginUI::GetInstance()->Destroy();
			break;
		}

		glfwSwapBuffers(cSettings->pWindow);

		// Perform Post Update Input Devices
		PostUpdateInputDevices();

		// Poll events
		glfwPollEvents();

		// Update Input Devices
		UpdateInputDevices();
	}

	
	ChatUI::GetInstance()->Init();
	// MAIN UPDATE LOOP
	while (!glfwWindowShouldClose(cSettings->pWindow)
	 && (!CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE)))
	{
		// UPDATE CHATUI, CHECK FOR INPUTS
		ChatUI::GetInstance()->Update();
		switch (ChatUI::GetInstance()->CheckForInputUpdate())
		{
		case ChatUI::SEND_MESSAGE:
			SendMessagePacket(ConnectSocket, Return);
			break;
		case ChatUI::CREATE_GROUP:
			SendCreateChatPacket(ConnectSocket, Return);
			break;
		default:
			TempFds = ReadFds;
			Timeout.tv_sec = 0;
			Timeout.tv_usec = 1000;

			if (SOCKET_ERROR == (Return = select(0, &TempFds, 0, 0, &Timeout)))
			{ // Select() function returned error.
				closesocket(ConnectSocket);
				printf("select() error\n");
				return;
			}
			else if (0 > Return)
			{
				printf("Select returned error!\n");
			}
			else if (0 < Return)
			{
				memset(Message, '\0', BUFSIZE);
				printf("Select Processed... Something to read\n");
				Return = recv(ConnectSocket, Message, BUFSIZE, 0);
				if (0 > Return)
				{ // recv() function returned error.
					closesocket(ConnectSocket);
					printf("Exceptional error :Socket Handle [%d]\n", ConnectSocket);
					return;
				}
				else if (0 == Return)
				{ // Connection closed message has arrived.
					closesocket(ConnectSocket);
					printf("Connection closed :Socket Handle [%d]\n", ConnectSocket);
					return;
				}
				else
				{ // Message received.
					//printf("Bytes received   : %d\n", Return);
					//printf("Message received : %s\n", Message);
					PacketManager::DecodePacket(Message);
				}
			}
		}


		ChatUI::GetInstance()->Render();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(cSettings->pWindow);

		// Perform Post Update Input Devices
		PostUpdateInputDevices();

		// Poll events
		glfwPollEvents();

		// Update Input Devices
		UpdateInputDevices();

		// Calculate the elapsed time since the last frame
		//dElapsedTime = cStopWatch.GetElapsedTime();
	}
		
}

/**
 @brief Destroy this class instance
 */
void Application::Destroy(void)
{
	ChatUI::GetInstance()->Destroy();

	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(cSettings->pWindow);
	//Finalize and clean up GLFW
	glfwTerminate();
}

/**
 @brief Get window height
 */
int Application::GetWindowHeight(void) const
{
	return cSettings->iWindowHeight;
}

/**
 @brief Get window width
 */
int Application::GetWindowWidth(void) const
{
	return cSettings->iWindowWidth;
}

/**
 @brief Constructor
 */
Application::Application(void)
	//: cFPSCounter(NULL)
{
}

/**
@brief Get updates from the input devices
*/
void Application::UpdateInputDevices(void)
{
	// Update Mouse Position
	double dMouse_X, dMouse_Y;
	glfwGetCursorPos( cSettings->pWindow, &dMouse_X, &dMouse_Y);
	CMouseController::GetInstance()->UpdateMousePosition( dMouse_X, dMouse_Y);
}

/**
 @brief End updates from the input devices. This method is not used anymore
 */
void Application::PostUpdateInputDevices(void)
{
	//CKeyboardController::GetInstance()->PostUpdate();
}

void Application::SendMessagePacket(SOCKET& ConnectSocket, int& Return)
{
	Return = send(ConnectSocket, PacketManager::GetMessagePacket(), strlen(PacketManager::GetMessagePacket()), 0);
	if (Return == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	std::cout << "**SENDING PACKET TO SERVER" << std::endl;
	ChatUI::GetInstance()->ClearChatBar();
}

void Application::SendCreateChatPacket(SOCKET& ConnectSocket, int& Return)
{
	Return = send(ConnectSocket, PacketManager::GetCreateChatPacket(), strlen(PacketManager::GetCreateChatPacket()), 0);
	if (Return == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	std::cout << "**SENDING PACKET TO SERVER" << std::endl;
	ChatUI::GetInstance()->ResetCreateGroupChatUI();
}
