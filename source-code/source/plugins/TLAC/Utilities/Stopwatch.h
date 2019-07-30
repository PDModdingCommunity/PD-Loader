#pragma once
#include <chrono>

namespace chrono = std::chrono;

typedef chrono::time_point<chrono::steady_clock> steady_clock;
typedef chrono::high_resolution_clock high_resolution_clock;

namespace TLAC::Utilities
{
	class Stopwatch
	{
		const float TIME_FACTOR = 1000.0f;

	public:
		Stopwatch();
		~Stopwatch();

		void Start();
		float Stop();
		float Restart();

		float GetElapsed();

	private:
		steady_clock start;
		steady_clock end;
	};
}

