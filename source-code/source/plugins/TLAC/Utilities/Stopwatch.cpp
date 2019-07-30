#include "Stopwatch.h"

namespace TLAC::Utilities
{
	Stopwatch::Stopwatch()
	{
	}

	Stopwatch::~Stopwatch()
	{
	}

	void Stopwatch::Start() 
	{
		start = high_resolution_clock::now();
	}

	float Stopwatch::Stop()
	{
		end = high_resolution_clock::now();
		return GetElapsed();
	}

	float Stopwatch::Restart()
	{
		float elapsed = Stop();
		Start();

		return elapsed;
	}

	float Stopwatch::GetElapsed()
	{
		return (float)(chrono::duration_cast<std::chrono::microseconds>(end - start).count() / TIME_FACTOR);
	}
}