#pragma once
#include <stdint.h>
#include <string>

namespace TLAC::Components
{
#pragma pack(push, 1)
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

	struct DrawTextParams
	{
		uint32_t textColour; // RGBA byte array?, so set as 0xAABBGGRR
		uint32_t strokeColour; // ?? RGBA byte array?, so set as 0xAABBGGRR
		uint8_t unk08;
		uint8_t padding09[0x3];
		uint32_t unk0c;
		uint32_t unk10;
		uint32_t unk14;
		uint32_t unk18;
		uint64_t unk1c;
		uint32_t unk24;
		uint32_t unk28;
		float xBegin; // ?
		float yBegin; // ?
		float xCurrent; // ?
		float yCurrent; // ?
		uint8_t padding3c[0x4];
		uint64_t unk40;
		FontInfo* font;
		uint16_t unk50;

		DrawTextParams(FontInfo* fi)
		{
			textColour = 0xffffffff;
			strokeColour = 0xff808080;
			unk08 = 0;
			unk0c = 0;
			unk10 = 0;
			unk14 = 0;
			unk18 = 0;
			unk1c = 0x7;
			unk24 = 0xd;
			unk28 = 0;
			xBegin = 0;
			yBegin = 0;
			xCurrent = 0;
			yCurrent = 0;
			unk40 = 0;
			font = fi;
			unk50 = 0x25a1;
		}

		DrawTextParams()
		{
			DrawTextParams((FontInfo*)0x140eda860);
		}
	};

	/* struct MsString {
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
				string_ptr = strdup(newcontent);
			}
			else
			{
				strcpy_s(string_buf, newcontent);
			}
		}
	}; */

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

	// flags aren't really known, but 1 == normal, 2 == right align?, 8 == centered
	void drawText(DrawTextParams* dtParam, uint32_t flags, std::string str)
	{
		((void(*)(DrawTextParams*, uint32_t, const char*, int64_t))0x140198500)(dtParam, flags, str.c_str(), str.length());
	}

	void drawTextW(DrawTextParams* dtParam, uint32_t flags, std::wstring str)
	{
		const wchar_t* ptrs[2];
		ptrs[0] = str.c_str();
		ptrs[1] = (wchar_t*)((uint64_t)ptrs[0] + str.length() * 2);

		((void(*)(DrawTextParams*, uint32_t, const wchar_t**))0x140198380)(dtParam, flags, &ptrs[0]);
	}
	//void(*drawTextW)(DrawTextParams* dtParam, uint32_t flags, MsStringW str) = (void(*)(DrawTextParams* dtParam, uint32_t flags, MsStringW str))0x140198380;
#pragma pack(pop)
}