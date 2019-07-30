#include "ScaleComponent.h"
#include <iostream>
#include <Windows.h>
#include "../Constants.h"
#include <stdio.h>
#include "../framework.h"
#include <chrono>
#include <thread>
#include <tchar.h>
#include <GL/freeglut.h>

namespace TLAC::Components
{

	ScaleComponent::ScaleComponent()
	{
	}

	ScaleComponent::~ScaleComponent()
	{
	}

	const char* ScaleComponent::GetDisplayName()
	{
		return "scale_component";
	}

	void ScaleComponent::Initialize(ComponentsManager*)
	{
		{
			DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001404ACD24, 7, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001404ACD24 + 0) = 0x44;
			*((byte*)0x00000001404ACD24 + 1) = 0x8B;
			*((byte*)0x00000001404ACD24 + 2) = 0x0D;
			*((byte*)0x00000001404ACD24 + 3) = 0xD1;
			*((byte*)0x00000001404ACD24 + 4) = 0x08;
			*((byte*)0x00000001404ACD24 + 5) = 0xD0;
			*((byte*)0x00000001404ACD24 + 6) = 0x00;
			VirtualProtect((BYTE*)0x00000001404ACD24, 7, oldProtect, &bck);
		}
		{
			DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001404ACD2B, 7, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001404ACD2B + 0) = 0x44;
			*((byte*)0x00000001404ACD2B + 1) = 0x8B;
			*((byte*)0x00000001404ACD2B + 2) = 0x05;
			*((byte*)0x00000001404ACD2B + 3) = 0xC6;
			*((byte*)0x00000001404ACD2B + 4) = 0x08;
			*((byte*)0x00000001404ACD2B + 5) = 0xD0;
			*((byte*)0x00000001404ACD2B + 6) = 0x00;
			VirtualProtect((BYTE*)0x00000001404ACD2B, 7, oldProtect, &bck);
		}
		{
			DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001405030A0, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001405030A0 + 0) = 0x90;
			*((byte*)0x00000001405030A0 + 1) = 0x90;
			*((byte*)0x00000001405030A0 + 2) = 0x90;
			*((byte*)0x00000001405030A0 + 3) = 0x90;
			*((byte*)0x00000001405030A0 + 4) = 0x90;
			*((byte*)0x00000001405030A0 + 5) = 0x90;
			VirtualProtect((BYTE*)0x00000001404ACD2B, 6, oldProtect, &bck);
		}
	}

	void ScaleComponent::Update()
	{
		uiAspectRatio = (float*)UI_ASPECT_RATIO;
		uiWidth = (float*)UI_WIDTH_ADDRESS;
		uiHeight = (float*)UI_HEIGHT_ADDRESS;
		fb1Height = (int*)FB1_HEIGHT_ADDRESS;
		fb1Width = (int*)FB1_WIDTH_ADDRESS;
		//fb2Height = (int*)FB2_HEIGHT_ADDRESS;
		//fb2Width = (int*)FB2_WIDTH_ADDRESS;
		fbAspectRatio = (double*)FB_ASPECT_RATIO;
		RECT hWindow;
		GetClientRect(TLAC::framework::DivaWindowHandle, &hWindow);
		*uiAspectRatio = (float)(hWindow.right - hWindow.left) / (float)(hWindow.bottom - hWindow.top);
		*fbAspectRatio = (double)(hWindow.right - hWindow.left) / (double)(hWindow.bottom - hWindow.top);
		*uiWidth = hWindow.right - hWindow.left;
		*uiHeight = hWindow.bottom - hWindow.top;
		*fb1Width = hWindow.right - hWindow.left;
		*fb1Height = hWindow.bottom - hWindow.top;
		//*fb2Width = hWindow.right - hWindow.left;
		//*fb2Height = hWindow.bottom - hWindow.top;

		*((int*)0x00000001411AD608) = 0;

		*((int*)0x0000000140EDA8E4) = *(int*)0x0000000140EDA8BC;
		*((int*)0x0000000140EDA8E8) = *(int*)0x0000000140EDA8C0;

		*(float*)0x00000001411A1900 = 0;
		*(float*)0x00000001411A1904 = (float)*(int*)0x0000000140EDA8BC;
		*(float*)0x00000001411A1908 = (float)*(int*)0x0000000140EDA8C0;
		//*((int*)0x00000001411AD5F8) = hWindow.right - hWindow.left;
		//*((int*)0x00000001411AD5FC) = hWindow.bottom - hWindow.top;

		//*((int*)0x00000001411ABB48) = hWindow.right - hWindow.left;
		//*((int*)0x00000001411ABB4C) = hWindow.bottom - hWindow.top;

		//*((int*)0x00000001411ABB5C) = (int)(*((int*)0x00000001411ABB54) * ((float)8 / (float)9)); //wtf??
	}

	void ScaleComponent::UpdateInput()
	{
		return;
	}
}