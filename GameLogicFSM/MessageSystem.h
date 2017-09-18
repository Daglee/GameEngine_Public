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

	inline void TransmitMessage(float messaage)
	{
		/*
		Is the update busy? If not, transmit. If it is,
		then just wait.
		*/
		unique_lock<mutex> lock(transmit_mutex);

		messages.push_back(messaage);
	}

	inline void BeginEvent(float messaage)
	{
		/*
		Is the update busy? If not, transmit. If it is,
		then just wait.
		*/
		unique_lock<mutex> lock(transmit_mutex);

		events.push_back(messaage);
	}

	void StopEvent(float msgTitle);

	//Is a message currently being transmitted?
	bool MessageTransmitting(float msgTitle);

	void ClearAllMessages()
	{
		messages.clear();
	}

	void ClearEverything()
	{
		messages.clear();
		events.clear();
	}

private:
	MessageSystem() {}

	virtual ~MessageSystem()
	{
		Destroy();
	}

	static void Destroy()
	{
		delete instance;
	}

	bool MessageExists(float messageToFind);
	bool EventExists(float eventToFind);

	static MessageSystem* instance;

	vector<float> messages;
	vector<float> events;
};

