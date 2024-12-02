#include "ChatUI.h"

#include "Settings.h"
#include "KeyboardController.h"
#include "imgui_internal.h"

#include <string>
#include <iostream>
#include <cstring>


void ChatUI::Init()
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

	temp_members.push_back(chatInfo->GetMyContact()->GetName());

	bCreateGroup = false;

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	window_flags |= ImGuiWindowFlags_NoCollapse;

}

void ChatUI::Update()
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


	// Chats List
	std::vector<Chat*> chatsList = chatInfo->GetChatsList();
	ImGuiWindowFlags contactsWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoCollapse;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	ImGui::Begin("Chats", NULL, contactsWindowFlags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.3f, windowHeight * 0.96f));
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add DM"))
		{
			std::vector<Contact*> userList = chatInfo->GetContactsList();
			for (int i = 0; i < userList.size(); i++)
			{
				Contact* user = userList.at(i);
				if (user == chatInfo->GetMyContact())
					continue;
				if (ImGui::MenuItem(user->GetName().c_str()))
				{
					if (chatInfo->FindChat(user->GetName()) == nullptr)
					{
						chatInfo->AddChat(user->GetName());
						chatInfo->AddContactToChat(user->GetName(), user->GetName());
						chatInfo->AddContactToChat(chatInfo->GetMyContact()->GetName(), user->GetName());
					}
					else
					{
						//chat alr exists
						chatInfo->SetCurrentChat(user->GetName());
					}
				}
			}
			ImGui::EndMenu();
		}
		else if (ImGui::BeginMenu("Add GroupChat"))
		{
			// GrpChat Name
			std::vector<Contact*> userList = GetUnAddedMembersList();
			ImGui::TextColored(ImVec4(0.6f, 0.7f, 1.0f, 1.0f), "Enter Chat Name:");
			ImGui::ActivateItem(ImGui::InputText("", group_name, IM_ARRAYSIZE(group_name)));
			// Selected Members List
			ImGui::TextColored(ImVec4(0.6f, 0.7f, 1.0f, 1.0f), "Selected Members:");
			for (int i = 0; i < temp_members.size(); i++)
			{
				ImGui::TextColored(ImVec4(0.7f, 1.0f, 0.7f, 1.0f), temp_members.at(i).c_str());
			}
			// Add Members Menu
			ImGui::TextColored(ImVec4(0.6f, 0.7f, 1.0f, 1.0f), "Add Member:");
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
			for (int i = 0; i < userList.size(); i++)
			{
				if (userList.at(i) != chatInfo->GetMyContact())
				{
					if (ImGui::MenuItem(userList.at(i)->GetName().c_str()))
					{
						temp_members.push_back(userList.at(i)->GetName());
					}
				}
			}
			ImGui::PopItemFlag();
		
			if (ImGui::Button("Create", ImVec2(windowWidth * 0.08f, windowHeight * 0.04f))
				&& temp_members.size() > 2)
			{
				
				bCreateGroup = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	for (int i = 0; i < chatsList.size(); i++)
	{
		Chat* chat = chatsList.at(i);
		if (ImGui::Button(chat->GetName().c_str(), ImVec2(windowWidth * 0.3f, windowHeight * 0.075f)))
			chatInfo->SetCurrentChat(chat->GetName());
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	// Chat Log
	std::vector<Text*> chatLog = chatInfo->GetCurrentChat()->GetChatLog();
	ImGuiWindowFlags chatWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse;
	ImGuiWindowFlags messageWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImGui::Begin(chatInfo->GetCurrentChat()->GetName().c_str(), NULL, chatWindowFlags);
	ImGui::SetWindowPos(ImVec2(windowWidth * 0.3f, 0.0f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.6f, windowHeight * 0.9f));
	float totalHeightAvail = windowHeight * 0.85f;
	float messageSpacing = windowHeight * 0.025f;
	for (int i = chatLog.size() - 1; i > -1; i--)
	{
		float textHeight = windowHeight * 0.1f;
		Text* text = chatLog.at(i);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		std::string windowName = "MessageID" + std::to_string(i);
		ImGui::Begin(windowName.c_str(), NULL, messageWindowFlags);
		if (text->senderName == chatInfo->GetMyContact()->GetName())
		{
			ImGui::SetWindowPos(ImVec2(windowWidth * 0.5f, windowHeight * 0.9f - (textHeight + messageSpacing) * (chatLog.size() - i)));
			ImGui::SetWindowSize(ImVec2(windowWidth * 0.4f, textHeight));
			ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "You");
		}
		else
		{
			ImGui::SetWindowPos(ImVec2(windowWidth * 0.3f, windowHeight * 0.9f - (textHeight + messageSpacing) * (chatLog.size() - i)));
			ImGui::SetWindowSize(ImVec2(windowWidth * 0.4f, textHeight));
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.8f, 1.0f), text->senderName.c_str());
		}
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), text->text.c_str());
		ImGui::End();
		ImGui::PopStyleColor();
		totalHeightAvail -= textHeight + messageSpacing;
		if (totalHeightAvail <= textHeight + messageSpacing)
			break;
	}
	ImGui::End();
	ImGui::PopStyleColor();

	// Message Enter Bar
	ImGuiWindowFlags enterBarWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.4f, 1.0f));
	ImGui::Begin("EnterBar", NULL, enterBarWindowFlags);
	ImGui::SetWindowPos(ImVec2(windowWidth * 0.3f, windowHeight * 0.9f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.6f, windowHeight * 0.096f));
	ImGui::InputText(" ENTER", message, IM_ARRAYSIZE(message));
	ImGui::End();
	ImGui::PopStyleColor();


	// Chat Members List
	ImGuiWindowFlags membersWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	ImGui::Begin("Members", NULL, membersWindowFlags);
	ImGui::SetWindowPos(ImVec2(windowWidth * 0.9f, 0.0f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.1f, windowHeight * 1.0f));
	std::vector<Contact*> members = ChatInfo::GetInstance()->GetCurrentChat()->GetContacts();
	for (int i = 0; i < members.size(); i++)
	{
		if (members.at(i) == chatInfo->GetMyContact())
			ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), members.at(i)->GetName().c_str());
		else
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), members.at(i)->GetName().c_str());
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	// User Info
	ImGuiWindowFlags userinfoWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	//ImGui::PushStyleColor(ImGuiCol_, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::Begin("UserInfo", NULL, userinfoWindowFlags);
	ImGui::SetWindowPos(ImVec2(0.0f, windowHeight * 0.95f));
	ImGui::SetWindowSize(ImVec2(windowWidth * 0.3f, windowHeight * 0.04f));
	Contact* myInfo = chatInfo->GetMyContact();
	std::string user_text = myInfo->GetName() + " ID: " + std::to_string(myInfo->GetID());
	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), user_text.c_str());
	ImGui::End();
	//ImGui::PopStyleColor();

	

	ImGui::End();
}

void ChatUI::Render()
{

	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ChatUI::Destroy()
{
	// Cleanup IMGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

ChatUI::CHAT_ACTION ChatUI::CheckForInputUpdate()
{
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_ENTER) && GetMessageLength() > 0)
		return SEND_MESSAGE;
	else if (bCreateGroup)
		return CREATE_GROUP;


	return NO_ACTION;
}

char* ChatUI::GetEnteredMessage()
{
	return message;
}

char* ChatUI::GetEnteredGroupName()
{
	return group_name;
}

int ChatUI::GetMessageLength()
{
	return strlen(message);
}

int ChatUI::GetGroupNameLength()
{
	return strlen(group_name);
}

std::vector<std::string> ChatUI::GetSelectedMembers()
{
	return temp_members;
}

void ChatUI::ClearChatBar()
{
	memset(message, '\0', BUFSIZE);
}

void ChatUI::ResetCreateGroupChatUI()
{
	ClearGroupNameBar();
	ResetTempMembers();
	bCreateGroup = false;
}

void ChatUI::ClearGroupNameBar()
{
	memset(group_name, '\0', BUFSIZE);
}

void ChatUI::ResetTempMembers()
{
	temp_members.clear();
	temp_members.push_back(chatInfo->GetMyContact()->GetName());
}

std::vector<Contact*> ChatUI::GetUnAddedMembersList()
{
	std::vector<Contact*> userList = chatInfo->GetContactsList();
	for (int i = 0; i < userList.size(); i++)
	{
		for (int t = 0; t < temp_members.size(); t++)
		{
			if (temp_members.at(t) == userList.at(i)->GetName())
			{
				userList.at(i) = nullptr;
				break;
			}
		}
	}
	userList.erase(std::remove(begin(userList), end(userList), nullptr),
		end(userList));

	return userList;

}
