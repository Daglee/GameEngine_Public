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

	percent = ((float)currentUsage / (float)maxUsage) * 100;
	bytesleft = (float)maxUsage - (float)currentUsage;

	/*
	  Log::Error is not needed here to check if database has
	  exceeded its bounds. A more specific error
	  message will be sent in the ResourceManager that
	  has exceeded it's limits.
	*/
}

void MemoryWatcher::DisplayPercent()
{
	float percent = ((float)currentUsage / (float)maxUsage) * 100;
	float bytesleft = (float)maxUsage - (float)currentUsage;

	cout << "Memory: " << percent << "%";
}

