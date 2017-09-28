#pragma once

#include "../Resources/ResourceManager.h"

#include <functional>
#include <string>

struct TableConfiguration
{
	string name;
	size_t maxNewBinSize;
	int maxNumberBins;

	TableConfiguration(string name, size_t maxNewBinSize, int maxNumberBins)
	{
		this->name = name;
		this->maxNewBinSize = maxNewBinSize;
		this->maxNumberBins = maxNumberBins;
	}

	TableConfiguration()
	{
		this->name = "";
		this->maxNewBinSize = 0;
		this->maxNumberBins = 0;
	}
};

template <class ResourceType>
class Table
{
public:

	Table()
	{}

	Table(bool verbose, bool allowDuplicates, function<ResourceType*(string)> ReadFunction,
		TableConfiguration initialisation)
	{
		manager = new ResourceManager<ResourceType>(verbose, allowDuplicates);

		ReadFromFile = ReadFunction;
		tableConfig = initialisation;
	}

	virtual ~Table() 
	{
		delete manager;
	}

	void AddNewEntryFromFile(std::string line)
	{
		manager->Load(ReadFromFile(line));
	}

	ResourceManager<ResourceType>* GetResources()
	{
		return manager;
	}

	void ReserveInitialMemory()
	{
		manager->Initialise(tableConfig.name, tableConfig.maxNewBinSize, tableConfig.maxNumberBins);
	}

private:
	function<ResourceType*(std::string)> ReadFromFile;
	ResourceManager<ResourceType>* manager;
	TableConfiguration tableConfig;
};

