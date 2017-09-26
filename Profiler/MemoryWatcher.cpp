#include "MemoryWatcher.h"
#include "../ResourceManagment/DataBase.h"

MemoryWatcher::MemoryWatcher(size_t max, DataBase* db)
{
	maxUsage = max;
	database = db;
}

void MemoryWatcher::Update()
{
	currentUsage = database->CurrentSize();

	percent = (static_cast<float>(currentUsage) / static_cast<float>(maxUsage)) * 100;
	bytesleft = static_cast<float>(maxUsage) - static_cast<float>(currentUsage);

	/*
	  Log::Error is not needed here to check if database has
	  exceeded its bounds. A more specific error
	  message will be sent in the ResourceManager that
	  has exceeded it's limits.
	*/
}

void MemoryWatcher::DisplayPercent() const
{
	const float percent = (static_cast<float>(currentUsage) /
		static_cast<float>(maxUsage)) * 100;

	cout << "Memory: " << percent << "%";
}

