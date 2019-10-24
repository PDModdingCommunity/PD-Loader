#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace TLAC::Components
{
#pragma pack(push, 1)
	struct Point
	{
		float x;
		float y;
	};
	struct RectangleBounds
	{
		float x;
		float y;
		float width;
		float height;
	};

	struct RawFont
	{
		uint32_t sprId; // ?
		uint8_t width1; // glyph?
		uint8_t height1; // glyph?
		uint8_t width2; // advance?
		uint8_t height2; // advance?
		uint8_t metric08; // or flag?
		uint8_t metric09; // or flag?
		uint8_t padding0a[0x06];
		float metric08divby09;
		uint64_t _0x10;
		int64_t dataBegin;
		int64_t dataEnd;
		int64_t dataCapacityEnd;
		uint8_t padding38[0x8];
	};

	struct RawFontHolderIdk // ...SeemsPrettyPointlessTbh
	{
		RawFont* rawfont;
		uint16_t width1;
		uint16_t height1;
		uint16_t zero1; // ?
		uint16_t zero2; // ?
		uint8_t zero3; // some kind of flag, but seems unused
	};

	struct FontInfo
	{
		uint32_t fontId;
		uint8_t padding04[0x4];
		RawFont* rawfont;
		uint16_t flag10; // (zero3 != 0 && padding38[0] != 0) ? 2 : (metric08 != metric09 ? 1 : 0)
		uint8_t padding12[0x02];
		float width1;
		float height1;
		float width2;
		float height2;
		float userSizeWidth;
		float userSizeHeight;
		float userSizeWidthMultiplier; // userSizeWidth / width1
		float userSizeHeightMultiplier; // userSizeHeight / width2
		float zero1; // ?
		float zero2; // ?

		void setSize(float width, float height)
		{
			((void(*)(FontInfo* fi, float width, float height))0x140199e60)(this, width, height);
		}

		FontInfo(uint32_t id)
		{
			((FontInfo*(*)(FontInfo* fi, uint32_t id))0x140196510)(this, id);
		}
	};

	FontInfo*(*getFont)(FontInfo* fi, uint32_t id) = (FontInfo*(*)(FontInfo* fi, uint32_t id))0x140196510;
	void(*fontSize)(FontInfo* fi, float width, float height) = (void(*)(FontInfo* fi, float width, float height))0x140199e60;

	struct DrawParams
	{
		uint32_t colour; // RGBA byte array?, so set as 0xAABBGGRR
		uint32_t fillColour; // ?? RGBA byte array?, so set as 0xAABBGGRR
		uint8_t clip;
		uint8_t unk09[0x3];
		RectangleBounds clipRect;
		uint32_t layer; // 8 seems similar to default but higher
						// 0x18 is below 0x19 but still seems to be above any game elements
		                // 0x19 is startup screen (below dwgui)
		                // I noticed some use different scaling
		uint32_t unk20;
		uint32_t unk24;
		uint32_t unk28;
		Point textCurrentLoc;
		Point lineOriginLoc; // must be set for newlines to work as expected
		uint8_t padding3c[0x4];
		uint64_t lineLength; // in characters
		FontInfo* font;
		uint16_t unk50;

		DrawParams(FontInfo* fi)
		{
			colour = 0xffffffff;
			fillColour = 0xff808080; // except it's not?
			clip = 0;
			clipRect = { 0, 0, 0, 0 };
			layer = 0x7;
			unk20 = 0x0;
			unk24 = 0xd;
			unk28 = 0;
			textCurrentLoc = { 0, 0 };
			lineOriginLoc = { 0, 0 };
			lineLength = 0;
			font = fi;
			unk50 = 0x25a1;
		}

		DrawParams()
		{
			DrawParams((FontInfo*)0x140eda860);
		}
	};

	enum drawTextFlags : uint32_t
	{
		DRAWTEXT_ENABLE_XADVANCE = 1,
		DRAWTEXT_ALIGN_RIGHT = 2,
		DRAWTEXT_ALIGN_SCREEN_CENTRE = 4,
		DRAWTEXT_ALIGN_CENTRE = 8,
		DRAWTEXT_ENABLE_CLIP = 0x200,
		DRAWTEXT_SCALING_OPTIMISED = 0x400, // ? -- seems to be set if the requested font size doesn't match the original font, and the internal width/height 1 and 2 match
											// maybe it's just required for any scaling though
		DRAWTEXT_STROKE = 0x10000,
	};

	void drawText(DrawParams* dtParam, drawTextFlags flags, std::string str)
	{
		((void(*)(DrawParams*, uint32_t, const char*, int64_t))0x140198500)(dtParam, flags, str.c_str(), str.length());
	}
	void drawTextW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(DrawParams*, uint32_t, const wchar_t**))0x140198380)(dtParam, flags, &ptrs[0]);
	}

	void drawTextFormattedW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(uint32_t, const wchar_t**, DrawParams*))0x1404c2aa0)(flags, &ptrs[0], dtParam);
	}

	void drawTextWithSpritesW(DrawParams* dtParam, drawTextFlags flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(uint32_t, const wchar_t**, DrawParams*))0x1404c2cf0)(flags, &ptrs[0], dtParam);
	}


	void(*fillRectangle)(DrawParams* dtParam, const RectangleBounds &rect) = (void(*)(DrawParams* dtParam, const RectangleBounds &rect))0x140198d80;

	// draws only a border -- use fillRectangle to fill contained pixels
	void drawRectangle(DrawParams* dtParam, const RectangleBounds &rect)
	{
		((void(*)(DrawParams*, const RectangleBounds&))0x140198320)(dtParam, rect);
	}

	// draws only a border -- use fillRectangle to fill contained pixels
	void drawRectangle(DrawParams* dtParam, const RectangleBounds &rect, float thickness)
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

	
	void drawLine(DrawParams* dtParam, const Point &p1, const Point &p2)
	{
		((void(*)(DrawParams*, const RectangleBounds&))0x140198080)(dtParam, { p1.x, p1.y, p2.x, p2.y });
	}
	// draw from the top left corner of rect to the bottom left
	void drawLine(DrawParams* dtParam, const RectangleBounds &rect)
	{
		drawLine(dtParam, { rect.x, rect.y }, { rect.x + rect.width, rect.y + rect.height });
	}

	void drawPolyline(DrawParams* dtParam, const std::vector<Point> points)
	{
		((void(*)(DrawParams*, const Point*, uint64_t))0x1401980e0)(dtParam, points.data(), points.size());
	}


	struct MsString {
		union {
			char* string_ptr;
			char string_buf[16];
		};
		uint64_t len;
		uint64_t bufsize;

		char* GetCharBuf()
		{
			if (bufsize > 0xf && string_ptr != nullptr)
				return string_ptr;
			else
				return string_buf;
		};

		void SetCharBuf(char* newcontent)
		{
			len = strlen(newcontent);
			bufsize = len;
			if (len > 0xf)
			{
				string_ptr = _strdup(newcontent);
			}
			else
			{
				strcpy_s(string_buf, newcontent);
			}
		}
	};

	/* struct MsStringW {
	union {
	wchar_t* string_ptr;
	wchar_t string_buf[8];
	};
	uint64_t len;
	uint64_t bufsize;

	wchar_t* GetCharBuf()
	{
	if (bufsize > 0x7 && string_ptr != nullptr)
	return string_ptr;
	else
	return string_buf;
	};

	void SetCharBuf(wchar_t* newcontent)
	{
	len = wcslen(newcontent);
	bufsize = len;
	if (len > 0xf)
	{
	string_ptr = wcsdup(newcontent);
	}
	else
	{
	wcscpy_s(string_buf, newcontent);
	}
	}
	}; */

	struct AetDebugFileInfo
	{
		MsString name1;
		int32_t gameId;
		char unk[4]; // seems to be some kind of mode prefix (eg. "GAM_")
		MsString name2; // same as name1???
		MsString filename;
		int32_t dbId1; // not sure what the db ids are...  this one might be a position
		int32_t dbId12;
	};

	struct AetFileInfo
	{
		int32_t id1;
		char unk1[4]; // seems to be some kind of mode prefix (eg. "GAM_")
		int32_t id2; // same as id1???
		int32_t unk2;
		MsString name;
		int32_t unk3;
		int32_t unk4; // might be related to sprites?
	};

	/*
	int findAetDebugFileId(std::string name)
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

	int findAetFileId(std::string name)
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

	enum createAetFlags : uint32_t
	{
		CREATEAET_20000 = 0x20000,
	};

	// draw an aet layer (with all settings)
	// aetSpeedCallback is actually a pointer to a class or struct with the callback address at offset +0x8
	int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, int32_t unk2, const char* animation, const char* animation2, float animationInTime, float animationOutTime, const Point &scale, const void* aetSpeedCallback)
	{
		return ((int(*)(int32_t, uint32_t, createAetFlags, const char*, const Point&, int32_t, const char*, const char*, float, float, const Point&, const void*))0x14013be60)(fileId, drawLayer, flags, name, loc, unk2, animation, animation2, animationInTime, animationOutTime, scale, aetSpeedCallback);
	}
	// draw an aet layer (with animation timing override)
	int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, float animationInTime, float animationOutTime)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, animationInTime, animationOutTime, *(Point*)0, 0);
	}
	// draw an aet layer (with scale)
	int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, const Point &scale)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, -1, -1, scale, 0);
	}
	// draw an aet layer
	int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc)
	{
		return createAetLayer(fileId, drawLayer, flags, name, loc, 0, 0, 0, -1, -1, *(Point*)0, 0);
	}

	void destroyAetLayer(int &layer)
	{
		if (layer != 0)
		{
			((void(*)(int layer))0x14019d570)(layer);
			layer = 0;
		}
	}
#pragma pack(pop)
}