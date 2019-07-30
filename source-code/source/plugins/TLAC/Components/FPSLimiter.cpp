#include "FPSLimiter.h"
#include <Windows.h>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdio>
#include <tchar.h>

using dsec = std::chrono::duration<double>;
using seconds = std::chrono::seconds;

// this is pretty much just lybxlpsv's limiter, but removed from GLComponent

namespace TLAC::Components
{
	using namespace std::chrono;

	static std::chrono::time_point mBeginFrame = system_clock::now();
	static std::chrono::time_point prevTimeInSeconds = time_point_cast<seconds>(mBeginFrame);

	const LPCTSTR config_file_name = _T(".\\config.ini");
	int fpsLimit = GetPrivateProfileIntW(L"graphics", L"fps.limit", 0, config_file_name);

	FPSLimiter::FPSLimiter()
	{
	}

	FPSLimiter::~FPSLimiter()
	{
	}

	const char* FPSLimiter::GetDisplayName()
	{
		return "fps_limiter";
	}

	void FPSLimiter::Initialize(ComponentsManager* manager)
	{
		mBeginFrame = std::chrono::system_clock::now();
		prevTimeInSeconds = std::chrono::time_point_cast<seconds>(mBeginFrame);
		frameCountPerSecond = 0;
		if(fpsLimit != 0)
			printf("FPSLimiter fpsLimit: %d\n", fpsLimit);
	}
	
	// I assume this gets called once per frame... if not this won't work
	void FPSLimiter::Update()
	{
		auto invFpsLimit = round<std::chrono::system_clock::duration>(dsec{ 1. / fpsLimit });
		auto mEndFrame = mBeginFrame + invFpsLimit;    
		auto timeInSeconds = std::chrono::time_point_cast<seconds>(std::chrono::system_clock::now());

		++frameCountPerSecond;
		if (timeInSeconds > prevTimeInSeconds)
		{
			//printf("FPSLimiter::Update(): FPS: %d\n", frameCountPerSecond);
			frameCountPerSecond = 0;
			prevTimeInSeconds = timeInSeconds;
		}

		// This part keeps the frame rate.
		if (fpsLimit > 19) // not sure why lyb used 19 here
			std::this_thread::sleep_until(mEndFrame);
		mBeginFrame = mEndFrame;
		mEndFrame = mBeginFrame + invFpsLimit;
	}
}