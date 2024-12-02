/**
 Application
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include <stdio.h>
#include <Windows.h>

// Include SingletonTemplate
#include "SingletonTemplate.h"



struct GLFWwindow;

class CSettings;

class Application : public CSingletonTemplate<Application>
{
	friend CSingletonTemplate<Application>;
public:
	// Initialise this class instance
	bool Init(void);
	// Run this class instance
	void Run(void);
	// Destroy this class instance
	void Destroy(void);

	// Get window height
	int GetWindowHeight(void) const;
	// Get window width
	int GetWindowWidth(void) const;
	
protected:

	CSettings* cSettings;

	// Constructor
	Application(void);

	// Destructor
	virtual ~Application(void);

	// Update input devices
	void UpdateInputDevices(void);
	void PostUpdateInputDevices(void);

	void SendMessagePacket(SOCKET& ConnectSocket, int& Return);
	void SendCreateChatPacket(SOCKET& ConnectSocket, int& Return);
};
