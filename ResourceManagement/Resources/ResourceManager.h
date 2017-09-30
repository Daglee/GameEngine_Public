#pragma once
#include <unordered_map>
#include <vector>
#include "ResourceMap.h"

#include "../Utilities/ErrorLog.h"
#include "../Utilities/StringUtility.h"

/*
  Manages a series of bins for one type of object.
  Main use is to manage memory.
   -- How much space left? Can It add the item?
*/
template <class T>
class ResourceManager
{
public:
	ResourceManager(bool verbose = false, bool allowDuplicates = false)
	{
		this->verbose = verbose;
		this->allowDuplicates = allowDuplicates;
	}

	~ResourceManager() {}

	void Initialise(const std::string &name, size_t maxNewBinSize, int maxNumberBins)
	{
		ErrorLog::LogErrorAndThrowExceptionIfStringEmpty(name, "Empty Resource Manager name not allowed");

		Name = StringUtility::TrimAndLower(name);
		newBinSize = maxNewBinSize;
		maxNumBins = maxNumberBins;

		maxManagerSize = (maxNumBins * newBinSize) + sizeof(*this);
		currentSize = sizeof(*this);
	}

	//Must be updated based on first object being added
	void Initialise(const std::string &name, int maxNumItemsPerBin, int maxNumberBins)
	{
		ErrorLog::LogErrorAndThrowExceptionIfStringEmpty(name, "Empty Resource Manager name not allowed");

		Name = StringUtility::TrimAndLower(name);
		maxNumBins = maxNumberBins;
		maxItemsPerBin = maxNumItemsPerBin;

		newBinSize = 0;
		maxManagerSize = 0;
		currentSize = 0;
	}

	//Add an item to the database
	T *Load(const std::string &resourceName, T* resource)
	{
		ErrorLog::LogErrorAndThrowExceptionIfStringEmpty(resourceName, "Empty filename not allowed");

		string formattedResourceName = StringUtility::TrimAndLower(resourceName);
		resource->SetName(formattedResourceName);

		if (!allowDuplicates)
		{
			vector<unique_ptr<ResourceMap<T>*>>::iterator mapIterator;
			for (mapIterator = resourceMaps.begin(); mapIterator != resourceMaps.end(); ++mapIterator)
			{
				T* duplicate = (*(*mapIterator))->Get(resourceName);

				if (duplicate != nullptr)
				{
					//Already loaded
					if (verbose)
					{
						cout << "Resource already exists" << endl;
					}

					return duplicate;
				}

			}
		}

		//Check if adding the new item would lead to exceeding the managers size limit
		if (currentSize + resource->GetSizeInBytes() <= maxManagerSize)
		{
			//Insert into the map
			//Insert into first bin that has enough space
			std::vector<unique_ptr<ResourceMap<T>*>>::iterator iterator;
			for (iterator = resourceMaps.begin(); iterator != resourceMaps.end(); ++iterator)
			{
				if ((*(*iterator))->FreeSpace() >= resource->GetSizeInBytes())
				{
					//There is enough space. Insert.
					(*(*iterator))->Add(resourceName, resource);
					currentSize = currentSize + resource->GetSizeInBytes();

					return resource;
				}
			}

			//No bins had enough space or no bins present. Create new bin
			ResourceMap<T>* newMap = new ResourceMap<T>;
			newMap->Initialise("Map" + std::to_string(binNumber), verbose, allowDuplicates, newBinSize);
			newMap->Add(resourceName, resource);
			AddResourceMap(newMap);
			
			currentSize = currentSize + resource->GetSizeInBytes();
		}
		else
		{
			ErrorLog::Error(Name + " Cannot add item - would exceed limit");
		}

		return resource;
	}

	//Add an item but the resource name has already been defined
	T* Load(T* resource)
	{
		const std::string name = resource->GetName();

		Load(name, resource);

		return resource;
	}

	//Delete an item
	bool Unload(const std::string &resourcename)
	{
		ErrorLog::LogErrorAndThrowExceptionIfStringEmpty(resourcename, "Empty resource name not allowed");

		const string name = StringUtility::TrimAndLower(resourcename);

		//Find the item to delete
		for (std::vector<unique_ptr<ResourceMap<T>*>>::iterator mapit = resourceMaps.begin();
			mapit != resourceMaps.end(); ++mapit)
		{
			if ((*(*mapit))->Get(name) != nullptr)
			{
				//Found
				currentSize = currentSize - (*(*mapit))->Get(name)->GetSizeInBytes();
				(*(*mapit))->Remove(name);

				return true;
			}

		}

		//If this reached, it could not be found...
		if (verbose) cout << ("cannot find " + name) << endl;
		return false;
	}

	//Retrieve an item
	T* Find(const string &resourcename)
	{
		ErrorLog::LogErrorAndThrowExceptionIfStringEmpty(resourcename, "Empty resource name not allowed");

		const string formattedResourceName = StringUtility::TrimAndLower(resourcename);

		/*
		  Iterates though each map to check for an item.
		  Worst case: O(N) -- albeit, likely a very short one.
		  Best case:  O(1) -- First item!
		*/
		for (vector<unique_ptr<ResourceMap<T>*>>::iterator mapit = resourceMaps.begin();
			mapit != resourceMaps.end(); ++mapit)
		{
			T* res = (*(*mapit))->Get(formattedResourceName);
			if (res != nullptr)
			{
				//Found
				return res;
			}

		}

		return nullptr;
	}

	//Add a new bin
	void AddResourceMap(ResourceMap<T>* newMap)
	{
		if (verbose) cout << Name + " added new Resource Map" << endl;

		resourceMaps.push_back(make_unique<ResourceMap<T>*>(newMap));
		binNumber++;
	}

	//Return string containing info on items inside a bin(s)
	string Dump(int mapIndex = -1)
	{
		if (mapIndex == -1) //Print all
		{
			string results;

			for (vector<unique_ptr<ResourceMap<T>*>>::iterator i = resourceMaps.begin(); i != resourceMaps.end(); ++i)
			{
				results = results + (*i)->Dump();
			}

			results = results + ("\nSpace used : " + to_string(currentSize));
			results = results + ("\nFree space : " + to_string(maxManagerSize - currentSize));
			results = results + "\n";

			return results;
		}
		else return resourceMaps.at(mapIndex)->Dump();
	}

	//Return string containing statistics about the resourcemanager
	string DumpStatistics()
	{
		string results = "\n-----------------" + Name + "-----------------";

		results = results + ("\nNumber of bins in use : " + std::to_string(resourceMaps.size()));
		results = results + ("\nMaximum number of bins allowed : " + std::to_string(maxNumBins));
		results = results + ("\nSpace used : " + std::to_string(currentSize));
		results = results + ("\nFree space : " + std::to_string(maxManagerSize - currentSize));
		results = results + "\n";

		return results;
	}

	const string &GetName() const
	{
		return Name;
	}
	const int Size() const
	{
		return Map.size();
	}

	size_t GetMaxSize() const
	{
		return maxManagerSize;
	}

	size_t GetCurrentSize() const
	{
		size_t size = 0;
		vector<unique_ptr<ResourceMap<T>*>>::const_iterator it = resourceMaps.begin();

		while (it != resourceMaps.end())
		{
			size += (*(*it))->GetCurrentSize();
			++it;
		}

		return size;
	}

private:
	ResourceManager(const ResourceManager&) {}
	ResourceManager &operator = (const ResourceManager&)
	{
		return *this;
	}

	unordered_map<string, T*> Map;
	string	Name;
	vector<unique_ptr<ResourceMap<T>*>> resourceMaps;

	size_t newBinSize;
	size_t maxManagerSize;
	size_t currentSize;

	int maxNumBins = 0;
	int binNumber = 0;
	int maxItemsPerBin;

	bool verbose;
	bool allowDuplicates;
};

