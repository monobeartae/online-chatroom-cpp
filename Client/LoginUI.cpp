#include "LoginUI.h"

#include "Settings.h"

#include <string>
#include <iostream>
#include <cstring>


void LoginUI::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); //(void)io;

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);


	cSettings = CSettings::GetInstance();
	chatInfo = ChatInfo::GetInstance();
	
	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

}

void LoginUI::Update()
{
	float windowWidth = cSettings->iWindowWidth;
	float windowHeight = cSettings->iWindowHeight;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible Window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
	ImGui::SetWindowFontScale(1.5f);

	
	// Message Enter Bar
	ImGuiWindowFlags enterBarWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::Begin("Enter Bar", NULL, enterBarWindowFlags);
	ImGui::SetWindowPos(ImVec2(windowWidth * 0.2f, windowHeight * 0.4f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.6f, windowHeight * 0.1f));
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Enter Display Name");
	ImGui::InputText(" ENTER", name, IM_ARRAYSIZE(name));
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	

	ImGui::End();
}

void LoginUI::Render()
{

	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void LoginUI::Destroy()
{
	// Cleanup IMGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

char* LoginUI::GetEnteredName()
{
	return name;
}

int LoginUI::GetNameLength()
{
	return strlen(name);
}

void LoginUI::ClearBar()
{
	memset(name, '\0', BUFSIZE);
}
