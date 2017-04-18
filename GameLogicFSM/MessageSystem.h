#pragma once

#include "../ResourceManagment/Log.h"
#include <vector>


class MessageSystem
{
public:
	static void Initialise()
	{
		instance = new MessageSystem();
	}

	static MessageSystem* GetInstance()
	{
		return instance;
	}

	inline void Transmit(float msg)
	{
		messages.push_back(msg);
	}

	float* RetrieveMessage(float msgTitle);
	bool MessageTransmitting(float msgTitle);
	void StopTransmit(float msgTitle);

	void ClearAllMessages()
	{
		messages.clear();
	}

protected:
	MessageSystem() {}

	virtual ~MessageSystem() 
	{
		Destroy();
	}

	static void Destroy()
	{
		delete instance;
	}

	static MessageSystem* instance;

	vector<float> messages;
};

