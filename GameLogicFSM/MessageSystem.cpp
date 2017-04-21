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
	lock_guard<mutex> lock(transmit_mutex);

	for (vector<float>::iterator i = events.begin();
		i != messages.end();) {
		if (*i == msgTitle) i = events.erase(i);
		else ++i;
	}
}