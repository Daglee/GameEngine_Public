#include "MessageSystem.h"

MessageSystem* MessageSystem::instance = NULL;

bool MessageSystem::MessageTransmitting(float msgTitle)
{
	/*
	  LOCK! Nothing in the system is allowed
	  to change until the check is done.
	*/
	lock_guard<mutex> lock(transmit_mutex);

	//Check messages
	for each(float msg in messages)
	{
		if (msg == msgTitle) {
			return true;
		}
	}

	//Check ongoing events
	for each(float evnt in events)
	{
		if (evnt == msgTitle) {
			return true;
		}
	}

	return false;
}

void MessageSystem::StopTransmitting(float msgTitle)
{
	/*
	  LOCK! Nothing in the system is allowed
	  to change until transmission stopped.
	*/

	bool msgExists = false;
	for each(float evnt in events)
	{
		if (evnt == msgTitle) {
			msgExists = true;
		}
	}

	if (msgExists) {
		lock_guard<mutex> lock(transmit_mutex);

		for (vector<float>::iterator i = events.begin();
			i != events.end();) {
			if (*i == msgTitle) {
				i = events.erase(i);

				break;
			}
			else ++i;
		}
	}
}