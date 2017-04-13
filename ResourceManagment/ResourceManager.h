#pragma once
#include <unordered_map>
#include <vector>
#include "ResourceMap.h"
#include <mutex>

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
		this->verbose			= verbose;
		this->allowDuplicates	= allowDuplicates;
	}

	~ResourceManager()
	{
		ReleaseAll();
	}

	void Initialise(const std::string &name, size_t maxNewBinSize, int maxNumberBins)	
	{
		if (name.empty()) Log::Error("Empty Resource Manager name not allowed");

		Name			= Log::TrimAndLower(name);
		newBinSize		= maxNewBinSize;
		maxNumBins		= maxNumberBins;
		maxManagerSize	= (maxNumBins * newBinSize) + sizeof(*this);
		currentSize		= sizeof(*this);
	}

	//Must be updated based on first object being added
	void Initialise(const std::string &name, int maxNumItemsPerBin, int maxNumberBins)
	{
		if (name.empty()) Log::Error("Empty Resource Manager name not allowed");

		Name			= Log::TrimAndLower(name);
		newBinSize		= 0;
		maxNumBins		= maxNumberBins;
		maxManagerSize	= 0;
		currentSize		= 0;
		maxItemsPerBin	= maxNumItemsPerBin;
	}

	//Add an item to the database
	T *Load(const std::string &resourcename, T* resourceObj)
	{
		if (resourcename.empty()) Log::Error("Empty filename not allowed");

		std::string ResourceName = Log::TrimAndLower(resourcename);
		resourceObj->SetResourceName(ResourceName);

		//Check if this is first object to be added. 
		//If so, update sizes according to first object being added
		if (newBinSize == 0)
		{
			//newBinSize = resourceObj->GetResourceSize() * maxItemsPerBin;
			//maxManagerSize = maxNumBins * newBinSize;
		}

		//Check for duplicate
		if (!allowDuplicates)
		{
			std::vector<ResourceMap<T>*>::iterator mapit;
			for (mapit = resourceMaps.begin();
				mapit != resourceMaps.end(); mapit++) {

				T* res = (*mapit)->Get(resourcename);
				if (res != nullptr) 
				{
					//Already loaded
					if (verbose) cout << "Resource already exists" << endl;
					return res;
				}

			}
		}

		//Check if adding the new item would lead to exceeding the managers size limit
		if (currentSize + resourceObj->GetResourceSize() <= maxManagerSize)
		{
			//Insert into the map
			//Insert into first bin that has enough space
			for (std::vector<ResourceMap<T>*>::iterator mapit = resourceMaps.begin();
				mapit != resourceMaps.end(); mapit++) 
			{
				if ((*mapit)->FreeSpace() >= resourceObj->GetResourceSize())
				{
					//There is enough space. Insert.
					(*mapit)->Add(resourcename, resourceObj);
					currentSize = currentSize + resourceObj->GetResourceSize();
					return resourceObj;
				}
			}

			//No bins had enough space or no bins present. Create new bin
			ResourceMap<T>* newMap = new ResourceMap<T>;
			newMap->Initialise("Map" + std::to_string(binNumber), verbose, allowDuplicates, newBinSize);
			AddResourceMap(newMap);
			newMap->Add(resourcename, resourceObj);
			currentSize = currentSize + resourceObj->GetResourceSize();
		}
		else Log::Error("Cannot add item - would exceed limit");

		return resourceObj;
	}

	//Add an item but the resource name has already been defined
	T* Load(T* resourceObj)
	{
		const std::string name = resourceObj->GetResourceName();

		Load(name, resourceObj);

		return resourceObj;
	}

	//Delete an item
	bool Unload(const std::string &resourcename)
	{
		if (resourcename.empty()) Log::Error("Empty resource name not allowed");

		std::string name = Log::TrimAndLower(resourcename);

		//Find the item to delete
		for (std::vector<ResourceMap<T>*>::iterator mapit = resourceMaps.begin();
			mapit != resourceMaps.end(); mapit++)
		{
			if ((*mapit)->Get(name) != nullptr)
			{
				//Found
				currentSize = currentSize - (*mapit)->Get(name)->GetResourceSize();
				(*mapit)->Remove(name);

				return true;
			}

		}

		//If this reached, it could not be found...
		if (verbose) std::cout << ("cannot find " + name) << std::endl;
		return false;
	}

	//Retrieve an item
	T* Find(const std::string &resourcename) 
	{
		if (resourcename.empty()) Log::Error("Empty filename not allowed");

		std::string ResourceName = Log::TrimAndLower(resourcename);

		/*
		  Iterates though each map to check for an item.
		  Worst case: O(N) -- albeit, likely a very short one.
		  Best case:  O(1) -- First item!
		*/
		for (std::vector<ResourceMap<T>*>::iterator mapit = resourceMaps.begin();
			mapit != resourceMaps.end(); mapit++) 
		{
			T* res = (*mapit)->Get(resourcename);
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
		if (verbose) std::cout << Name + " added new Resource Map" << std::endl;

		resourceMaps.push_back(newMap);
		binNumber++;
	}

	//Return string containing info on items inside a bin(s)
	std::string Dump(int mapIndex = -1) 
	{
		if (mapIndex == -1) //Print all
		{ 
			std::string results;

			for (vector<ResourceMap<T>*>::iterator i = resourceMaps.begin(); i != resourceMaps.end(); i++)
			{
				results = results + (*i)->Dump();
			}

			results = results + ("\nSpace used : " + std::to_string(currentSize));
			results = results + ("\nFree space : " + std::to_string(maxManagerSize - currentSize));
			results = results + "\n";

			return results;
		}
		else return resourceMaps.at(mapIndex)->Dump();
	}

	//Return string containing statistics about the resourcemanager
	std::string DumpStatistics() 
	{
		string results = "\n-----------------" + Name + "-----------------";

		results = results + ("\nNumber of bins in use : " + std::to_string(resourceMaps.size()));
		results = results + ("\nMaximum number of bins allowed : " + std::to_string(maxNumBins));
		results = results + ("\nSpace used : " + std::to_string(currentSize));
		results = results + ("\nFree space : " + std::to_string(maxManagerSize - currentSize));
		results = results + "\n";

		return results;
	}

	const std::string &GetName() const { return Name; }
	const int Size() const { return Map.size(); }

	size_t GetMaxSize() const
	{
		return maxManagerSize;
	}

	size_t GetCurrentSize() const 
	{
		size_t size = 0;
		std::vector<ResourceMap<T>*>::const_iterator it = resourceMaps.begin();

		while (it != resourceMaps.end()){
			size += (*it)->GetCurrentSize();
			it++;
		}

		return size;
	}

private:
	ResourceManager(const ResourceManager&) { };
	ResourceManager &operator = (const ResourceManager&) { return *this; }

	//Force removal for each map
	void ReleaseAll()
	{
		std::vector<ResourceMap<T>*>::iterator it = resourceMaps.begin();

		while (it != resourceMaps.end())
		{
			it = resourceMaps.erase(it);
		}
	}

	std::unordered_map<std::string, T*> Map;
	std::string	Name;
	std::vector<ResourceMap<T>*> resourceMaps;

	size_t newBinSize;
	size_t maxManagerSize;
	size_t currentSize;

	int maxNumBins = 0;
	int binNumber = 0;
	int maxItemsPerBin;

	bool verbose;
	bool allowDuplicates;
};

