#include "MessageSystem.h"

MessageSystem* MessageSystem::instance = nullptr;

bool MessageSystem::MessageTransmitting(const float msgTitle)
{
	lock_guard<mutex> lock(transmitMutex);

	return MessageExists(msgTitle) || EventExists(msgTitle);
}

void MessageSystem::StopEvent(const float msgTitle)
{
	if (EventExists(msgTitle))
	{
		lock_guard<mutex> lock(transmitMutex);

		for (vector<float>::iterator message = events.begin();
			message != events.end();)
		{
			if (*message == msgTitle)
			{
				message = events.erase(message);

				break;
			}
			
			++message;
		}
	}
}

bool MessageSystem::MessageExists(float messageToFind) const
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

bool MessageSystem::EventExists(float eventToFind) const
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
