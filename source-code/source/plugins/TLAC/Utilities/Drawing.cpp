#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "Drawing.h"
#include "../Constants.h"

namespace TLAC::Utilities
{
#pragma pack(push, 1)
	Drawing::FontInfo*(*Drawing::getFont)(FontInfo* fi, uint32_t id) = (FontInfo*(*)(FontInfo* fi, uint32_t id))0x140196510;
	void(*Drawing::fontSize)(FontInfo* fi, float width, float height) = (void(*)(FontInfo* fi, float width, float height))0x140199e60;
	
	void Drawing::drawText(DrawParams* dtParam, drawTextFlags flags, std::string str)
	{
		((void(*)(DrawParams*, uint32_t, const char*, int64_t))0x140198500)(dtParam, flags, str.c_str(), str.length());
	}
	void Drawing::drawTextW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(DrawParams*, uint32_t, const wchar_t**))0x140198380)(dtParam, flags, &ptrs[0]);
	}

	void Drawing::drawTextFormattedW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(uint32_t, const wchar_t**, DrawParams*))0x1404c2aa0)(flags, &ptrs[0], dtParam);
	}

	void Drawing::drawTextWithSpritesW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(uint32_t, const wchar_t**, DrawParams*))0x1404c2cf0)(flags, &ptrs[0], dtParam);
	}


	void(*Drawing::fillRectangle)(DrawParams* dtParam, const RectangleBounds &rect) = (void(*)(DrawParams* dtParam, const RectangleBounds &rect))0x140198d80;

	// draws only a border -- use fillRectangle to fill contained pixels
	void Drawing::drawRectangle(DrawParams* dtParam, const RectangleBounds &rect)
	{
		((void(*)(DrawParams*, const RectangleBounds&))0x140198320)(dtParam, rect);
	}

	// draws only a border -- use fillRectangle to fill contained pixels
	void Drawing::drawRectangle(DrawParams* dtParam, const RectangleBounds &rect, float thickness)
	{
		uint32_t oldFillColour = dtParam->fillColour;
		dtParam->fillColour = dtParam->colour;

		// yes this seems pretty dodgy, but sega does it this way so... I guess it's the only way
		RectangleBounds tempRect = { rect.x, rect.y, thickness, rect.height }; // left side
		fillRectangle(dtParam, tempRect);

		tempRect.x = rect.x + rect.width - thickness; // right side
		fillRectangle(dtParam, tempRect);

		tempRect = { rect.x + thickness, rect.y, rect.width - (thickness * 2), thickness }; // top side
		fillRectangle(dtParam, tempRect);

		tempRect.y = rect.y + rect.height - thickness; // left side
		fillRectangle(dtParam, tempRect);

		dtParam->fillColour = oldFillColour;
	}


	void Drawing::drawLine(DrawParams* dtParam, const Point &p1, const Point &p2)
	{
		((void(*)(DrawParams*, const RectangleBounds&))0x140198080)(dtParam, { p1.x, p1.y, p2.x, p2.y });
	}
	// draw from the top left corner of rect to the bottom left
	void Drawing::drawLine(DrawParams* dtParam, const RectangleBounds &rect)
	{
		drawLine(dtParam, { rect.x, rect.y }, { rect.x + rect.width, rect.y + rect.height });
	}

	void Drawing::drawPolyline(DrawParams* dtParam, const std::vector<Point> points)
	{
		((void(*)(DrawParams*, const Point*, uint64_t))0x1401980e0)(dtParam, points.data(), points.size());
	}

	/*
	int Drawing::findAetDebugFileId(std::string name)
	{
	AetDebugFileInfo* aetArray = *(AetDebugFileInfo**)AET_DEBUG_ARRAY_POINTER_ADDRESS;
	AetDebugFileInfo* aetArrayEndAddress = *(AetDebugFileInfo**)(AET_DEBUG_ARRAY_POINTER_ADDRESS + 0x08);

	int id = 0;
	while (&aetArray[id] < aetArrayEndAddress)
	{
	if (name == aetArray[id].name2.GetCharBuf()) // dwgui enum uses name2, so this should too I guess
	return aetArray[id].gameId;
	else
	id++;
	}
	return -1;
	}
	*/

	// gets a file ID for use with createAetLayer
	// returns -1 if the file was not found
	// note: names are a little different to in 2dauth test -- it seems like they have "_MAIN" appended
	int Drawing::findAetFileId(std::string name)
	{
		AetFileInfo* aetArray = *(AetFileInfo**)AET_ARRAY_POINTER_ADDRESS;
		AetFileInfo* aetArrayEndAddress = *(AetFileInfo**)(AET_ARRAY_POINTER_ADDRESS + 0x08);

		int id = 0;
		while (&aetArray[id] < aetArrayEndAddress)
		{
			if (name == aetArray[id].name.GetCharBuf()) // dwgui enum uses name2, so this should too I guess
				return aetArray[id].id1;
			else
				id++;
		}
		return -1;
	}

	// draw an aet layer (with all settings)
	// aetSpeedCallback is actually a pointer to a class or struct with the callback address at offset +0x8
	int Drawing::createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, int32_t unk2, const char* animation, const char* animation2, float animationInTime, float animationOutTime, const Point &scale, const void* aetSpeedCallback)
	{
		return ((int(*)(int32_t, uint32_t, createAetFlags, const char*, const Point&, int32_t, const char*, const char*, float, float, const Point&, const void*))0x14013be60)(fileId, drawLayer, flags, name, loc, unk2, animation, animation2, animationInTime, animationOutTime, scale, aetSpeedCallback);
	}
	// draw an aet layer (with animation timing override)
	int Drawing::createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, float animationInTime, float animationOutTime)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, animationInTime, animationOutTime, *(Point*)0, 0);
	}
	// draw an aet layer (with scale)
	int Drawing::createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, const Point &scale)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, -1, -1, scale, 0);
	}
	// draw an aet layer
	int Drawing::createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, -1, -1, *(Point*)0, 0);
	}

	void Drawing::destroyAetLayer(int &layer)
	{
		if (layer != 0)
		{
			((void(*)(int layer))0x14019d570)(layer);
			layer = 0;
		}
	}
#pragma pack(pop)
}