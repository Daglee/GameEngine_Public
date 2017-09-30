#pragma once

#include "../ResourceManagement/Utilities/ErrorLog.h"

#include <vector>
#include <mutex>

/*
  A system for subsystems to transmit messages, read them,
  and trigger events.
*/
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

	inline void TransmitMessage(const float messaage)
	{
		unique_lock<mutex> lock(transmitMutex);

		messages.push_back(messaage);
	}

	inline void BeginEvent(const float messaage)
	{
		unique_lock<mutex> lock(transmitMutex);

		events.push_back(messaage);
	}

	void StopEvent(const float msgTitle);

	bool MessageTransmitting(const float msgTitle);

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

	bool MessageExists(float messageToFind) const;
	bool EventExists(float eventToFind) const;

	static MessageSystem* instance;

	vector<float> messages;
	vector<float> events;

	mutex transmitMutex;
};

