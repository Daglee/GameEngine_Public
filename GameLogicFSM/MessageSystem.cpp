#include "MessageSystem.h"

MessageSystem* MessageSystem::instance = NULL;

bool MessageSystem::MessageTransmitting(float msgTitle)
{
	/*
	  LOCK! Nothing in the system is allowed
	  to change until the check is done.
	*/
	lock_guard<mutex> lock(transmit_mutex);

	return MessageExists(msgTitle) || EventExists(msgTitle);
}

void MessageSystem::StopEvent(float msgTitle)
{
	if (EventExists(msgTitle))
	{
		lock_guard<mutex> lock(transmit_mutex);

		for (vector<float>::iterator i = events.begin();
			i != events.end();)
		{
			if (*i == msgTitle)
			{
				i = events.erase(i);

				break;
			}
			else ++i;
		}
	}
}

bool MessageSystem::MessageExists(float messageToFind)
{
	for each(float message in messages)
	{
		if (message == messageToFind)
		{
			return true;
		}
	}

	return false;
}

bool MessageSystem::EventExists(float eventToFind)
{
	for each(float event in events)
	{
		if (event == eventToFind)
		{
			return true;
		}
	}

	return false;
}
