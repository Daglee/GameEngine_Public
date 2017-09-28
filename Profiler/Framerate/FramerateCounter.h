#pragma once

const int SECONDS = 1000;
const int MIN_FRAMES = 4;
const float MIN_TIME_PASSED = 0.25f;

class FramerateCounter
{
public:
	FramerateCounter(float starttime = 0);

	virtual ~FramerateCounter() {}

	inline void CalculateFPS(const float& time)
	{
		const float elapsedTime = time - lastTime;

		if (elapsedTime > MIN_TIME_PASSED && frames > MIN_FRAMES)
		{
			fps = (static_cast<float>(frames) / elapsedTime) * SECONDS;
			lastTime = time;
			frames = 0;
		}
	}

	void DisplayFPS() const;

	int		frames;
	float	lastTime;
	float	fps;
};

