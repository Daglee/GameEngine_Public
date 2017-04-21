#pragma once

#include "../ResourceManagment/Log.h"

#include <vector>
#include <mutex>

/*
  A system for subsystems to transmit messages, read them, 
  and trigger events.
   - Messages last 1 frame.
   - Events stay until asked to to be removed.
  Strings must be hashed.
  Uses singleton.
*/
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

	//Single frame message
	inline void Transmit(float msg, bool isEvent)
	{	
		/*
		  Is the update busy? If not, transmit. If it is,
		  then just wait.
		*/
		unique_lock<mutex> lock(transmit_mutex);

		if (isEvent) events.push_back(msg);
		else messages.push_back(msg);
	}

	//Is a message currently being transmitted?
	bool MessageTransmitting(float msgTitle);

	//Stop an event
	void StopTransmitting(float msgTitle);

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
	vector<float> events;
};

