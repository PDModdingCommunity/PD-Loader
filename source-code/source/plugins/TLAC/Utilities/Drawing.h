#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace TLAC::Utilities
{
	class Drawing
	{
	public:
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
			uint8_t width1; // advance
			uint8_t height1; // advance
			uint8_t width2; // tex glyph box
			uint8_t height2; // tex glyph box
			uint8_t layoutParam2Num; // layout param 2 is a fraction that seems to relate to margins
			uint8_t layoutParam2Div;
			uint8_t padding0a[0x02];
			int32_t fontmapId;
			float layoutParam2NumOverDiv;
			uint8_t padding14[0x04];
			uint64_t texWidthChars;
			int64_t dataBegin;
			int64_t dataEnd;
			int64_t dataCapacityEnd;
			uint8_t layoutParam1;
			uint8_t padding39[0x7];
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

		static FontInfo*(*getFont)(FontInfo* fi, uint32_t id);
		static void(*fontSize)(FontInfo* fi, float width, float height);

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

		static void drawText(DrawParams* dtParam, drawTextFlags flags, std::string str);
		static void drawTextW(DrawParams* dtParam, drawTextFlags flags, std::wstring str);
		static void drawTextFormattedW(DrawParams* dtParam, drawTextFlags flags, std::wstring str);
		static void drawTextWithSpritesW(DrawParams* dtParam, drawTextFlags flags, std::wstring str);

		static void(*fillRectangle)(DrawParams* dtParam, const RectangleBounds &rect);
		// draws only a border -- use fillRectangle to fill contained pixels
		static void drawRectangle(DrawParams* dtParam, const RectangleBounds &rect);
		// draws only a border -- use fillRectangle to fill contained pixels
		static void drawRectangle(DrawParams* dtParam, const RectangleBounds &rect, float thickness);

		static void drawLine(DrawParams* dtParam, const Point &p1, const Point &p2);
		// draw from the top left corner of rect to the bottom left
		static void drawLine(DrawParams* dtParam, const RectangleBounds &rect);

		static void drawPolyline(DrawParams* dtParam, const std::vector<Point> points);


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


		// int findAetDebugFileId(std::string name);

		// gets a file ID for use with createAetLayer
		// returns -1 if the file was not found
		// note: names are a little different to in 2dauth test -- it seems like they have "_MAIN" appended
		static int findAetFileId(std::string name);

		enum createAetFlags : uint32_t
		{
			CREATEAET_20000 = 0x20000,
		};

		// draw an aet layer (with all settings)
		// aetSpeedCallback is actually a pointer to a class or struct with the callback address at offset +0x8
		static int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, int32_t unk2, const char* animation, const char* animation2, float animationInTime, float animationOutTime, const Point &scale, const void* aetSpeedCallback);
		// draw an aet layer (with animation timing override)
		static int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, float animationInTime, float animationOutTime);
		// draw an aet layer (with scale)
		static int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc, const Point &scale);
		// draw an aet layer
		static int createAetLayer(int32_t fileId, uint32_t drawLayer, createAetFlags flags, const char* name, const Point &loc);

		static void destroyAetLayer(int &layer);
#pragma pack(pop)
	};
}