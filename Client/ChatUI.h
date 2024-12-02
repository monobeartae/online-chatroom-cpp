#pragma once

#include "SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif


//Include IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI/imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

#include "Settings.h"
#include "ChatInfo.h"

#define BUFSIZE     1024


class ChatUI : public CSingletonTemplate<ChatUI>

{
	friend CSingletonTemplate<ChatUI>;
public:

	enum CHAT_ACTION {
		NO_ACTION,
		SEND_MESSAGE,
		CREATE_GROUP,

		CA_TOTAL
	};

	void Init();
	void Update();
	void Render();
	void Destroy();

	CHAT_ACTION CheckForInputUpdate();

	char* GetEnteredMessage();
	char* GetEnteredGroupName();
	int GetMessageLength();
	int GetGroupNameLength();
	std::vector<std::string> GetSelectedMembers();

	void ClearChatBar();
	void ResetCreateGroupChatUI();

private:
	
	char message[BUFSIZE];
	char group_name[BUFSIZE];
	std::vector<std::string> temp_members;

	CSettings* cSettings;
	ChatInfo* chatInfo;

	bool bCreateGroup;

	ImGuiWindowFlags window_flags;

	std::vector<Contact*> GetUnAddedMembersList();
	void ClearGroupNameBar();
	void ResetTempMembers();
};


