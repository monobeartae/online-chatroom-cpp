#include "Settings.h"

#include <iostream>
using namespace std;

CSettings::CSettings(void)
	: pWindow(NULL)
	, logl_root(NULL)
{
}


CSettings::~CSettings(void)
{
}


/**
@brief Convert an index number of a tile to a coordinate in UV Space
*/
//float CSettings::ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset)
//{
//	float fResult = 0.0f;
//	if (sAxis == x)
//	{
//		fResult = -1.0f + (float)iIndex*TILE_WIDTH + TILE_WIDTH / 2.0f + fOffset;
//	}
//	else if (sAxis == y)
//	{
//		if (bInvert)
//			fResult = 1.0f - (float)(iIndex + 1)*TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
//		else
//			fResult = -1.0f + (float)iIndex*TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
//	}
//	else if (sAxis == z)
//	{
//		// Not used in here
//	}
//	else
//	{
//		cout << "Unknown axis" << endl;
//	}
//	return fResult;
//}

void CSettings::ToggleMousePointer(bool show)
{
	bDisableMousePointer = !show;
	bShowMousePointer = show;
	if (!show)
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//// Update the specifications of the map
//void CSettings::UpdateSpecifications(void)
//{
//	TILE_WIDTH = 2.0f / WINDOW_NUM_TILES_XAXIS;	// 0.0625f;
//	TILE_HEIGHT = 2.0f / WINDOW_NUM_TILES_YAXIS;	// 0.08333f;
//
//	MICRO_STEP_XAXIS = TILE_WIDTH / NUM_STEPS_PER_TILE_XAXIS;
//	MICRO_STEP_YAXIS = TILE_HEIGHT / NUM_STEPS_PER_TILE_YAXIS;
//}
