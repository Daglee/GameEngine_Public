#pragma once
#include <string.h>
#include <iostream>
#include "../ResourceManagment/Log.h"

class DataBase;

//Restricted to database
class MemoryWatcher
{
public:
	MemoryWatcher(size_t max = 0, DataBase* db = nullptr);

	virtual ~MemoryWatcher() {}

	void Update();
	void DisplayPercent() const;

	void SetWatch(DataBase* db)
	{
		database = db;
	}

	float percent;
	float bytesleft;
protected:
	DataBase* database;
	size_t maxUsage;
	size_t currentUsage;
};

