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


class LoginUI : public CSingletonTemplate<LoginUI>

{
	friend CSingletonTemplate<LoginUI>;
public:
	void Init();
	void Update();
	void Render();
	void Destroy();

	char* GetEnteredName();
	int GetNameLength();

	void ClearBar();
	
private:
	char name[BUFSIZE];

	CSettings* cSettings;
	ChatInfo* chatInfo;
	ImGuiWindowFlags window_flags;
};

