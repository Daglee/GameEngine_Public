#include "SubsystemTimer.h"

const int TWO_DECIMAL_PLACES = 2;

SubsystemTimer::SubsystemTimer()
{
	start = timer.GetMS();
	finish = timer.GetMS();
}

void SubsystemTimer::DisplayTimer() const
{
	cout << fixed << setprecision(TWO_DECIMAL_PLACES) << "Time: " << timePassed;
}

void SubsystemTimer::DisplayTimer(const string timername) const
{
	cout << timername + ": " << setprecision(TWO_DECIMAL_PLACES) << fixed << timePassed;
}