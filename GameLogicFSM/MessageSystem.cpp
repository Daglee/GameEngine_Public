#include "MessageSystem.h"

MessageSystem* MessageSystem::instance = NULL;

float* MessageSystem::RetrieveMessage(float msgTitle)
{
	/*
	  LOCK! Nothing in the system is allowed
	  to change until the check is done.
	*/
	lock_guard<mutex> lock(transmit_mutex);

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
	/*
	  LOCK! Nothing in the system is allowed
	  to change until the check is done.
	*/
	lock_guard<mutex> lock(transmit_mutex);

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
	/*
	  LOCK! Nothing in the system is allowed
	  to change until transmission stopped.
	*/
	lock_guard<mutex> lock(transmit_mutex);

	for (vector<float>::iterator i = messages.begin();
		i != messages.end();) {
		if (*i == msgTitle) i = messages.erase(i);
		else ++i;
	}
}