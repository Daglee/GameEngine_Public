#include "SubsystemTimer.h"

const int TWO_DECIMAL_PLACES = 2;

SubsystemTimer::SubsystemTimer()
{
	start = timer.GetMS();
	finish = timer.GetMS();
}

void SubsystemTimer::DisplayTimer()
{
	cout << fixed << setprecision(TWO_DECIMAL_PLACES) << "Time: " << timePassed;
}

void SubsystemTimer::DisplayTimer(string timername)
{
	cout << timername + ": " << setprecision(TWO_DECIMAL_PLACES) << fixed << timePassed;
}