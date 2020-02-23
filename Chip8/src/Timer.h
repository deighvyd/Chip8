#pragma once

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Pause();
	void Unpause();

	unsigned int GetTicks();

	bool IsStarted();
	bool IsPaused();

private:
	unsigned int _startTicks;
	unsigned int _pausedTicks;
	bool _paused;
	bool _started;

};