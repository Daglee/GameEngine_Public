#include "MessageSystem.h"

MessageSystem* MessageSystem::instance = NULL;

float* MessageSystem::RetrieveMessage(float msgTitle)
{
	for each(float msg in messages)
	{
		if (msg == msgTitle) {
			return &msg;
		}
	}

	return nullptr;
}

bool MessageSystem::MessageTransmitting(float msgTitle)
{
	for each(float msg in messages)
	{
		if (msg == msgTitle) {
			return true;
		}
	}

	return false;
}

void MessageSystem::StopTransmit(float msgTitle)
{
	for (vector<float>::iterator i = messages.begin();
		i != messages.end();) {
		if (*i == msgTitle) i = messages.erase(i);
		else ++i;
	}
}