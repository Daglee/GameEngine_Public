#pragma once

#include "../ResourceManagment/Log.h"

#include <vector>
#include <mutex>

class MessageSystem
{
public:
	/*
	  A mutex is needed in the update function to make sure no new 
	  messages are transmitted during any transmission checks.
	  Performance loss is negligible.
	*/
	mutex transmit_mutex;

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
		/*
		  Is the update busy? If not, transmit. If it is,
		  then just wait.
		*/
		unique_lock<mutex> lock(transmit_mutex);

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

