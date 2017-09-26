#pragma once
#include <string>

/*
  A bin that is created by the resource manager.
   -- Essentially a wrapper around a map.
  This actually contains the memory addresses of objects.
*/
template <class T>
class ResourceMap
{
public:
	ResourceMap()
	{
		verbose = -1;
		allowDuplicates = -1;
	}

	~ResourceMap()
	{
		if (verbose)
		{
			std::cout << mapName + (" : Releasing\n");
		}

		Clear();
	}

	void Initialise(const std::string &name, bool verbosity, bool duplicates, size_t maximumSize)
	{
		Log::ExitIfEmpty(name, mapName + " : Array name cannot be empty");

		mapName = Log::TrimAndLower(name);

		verbose = verbosity;
		allowDuplicates = duplicates;

		//Just debug info...
		if (verbose)
		{
			if (allowDuplicates)
			{
				std::cout << (mapName + " : Allows duplicates") << std::endl;
			}
			else
			{
				std::cout << (mapName + " : Disallows duplicates") << std::endl;
			}
		}

		maxSize = maximumSize;
		currentSize = 0;
	}

	//Add a new element!
	T *Add(const std::string &resourceName, T* resource)
	{
		Log::ExitIfEmpty(resourceName, mapName + " : resource name cannot be empty");

		const std::string formattedResourceName = Log::TrimAndLower(resourceName);

		if (verbose)
		{
			std::cout << ("Adding " + formattedResourceName) << std::endl;
		}

		size_t hash = std::hash <std::string>{}(formattedResourceName);

		resources.insert(std::pair<size_t, T*>(hash, resource));

		currentSize = currentSize + resource->GetSizeInBytes();

		return resource;
	}


	//Delete an element using resourcename
	void Remove(const std::string &resourceName)
	{
		Log::ExitIfEmpty(resourceName, mapName + " : resource name cannot be empty");

		const std::string formattedResourceName = Log::TrimAndLower(resourceName);

		if (verbose)
		{
			std::cout << ("Removing " + formattedResourceName) << std::endl;
		}

		size_t hash = std::hash <std::string>{}(formattedResourceName);

		currentSize = currentSize - resources.at(hash)->GetSizeInBytes();

		resources.erase(hash);
	}

	//Clear all elements from bin
	void Clear()
	{
		std::unordered_map<size_t, T* >::iterator it = resources.begin();

		while (it != resources.end())
		{
			const std::string filename = (*it).second->GetName();
			size_t hash = std::hash <std::string>{}(filename);

			delete(resources.find(hash)->second);
			it = resources.erase(hash);
		}

		currentSize = 0;
	}

	//Dumps bin content to a string
	std::string Dump()
	{
		std::string str = ("Dumping database - " + mapName + "\n");

		for (std::unordered_map<size_t, T* >::iterator it = resources.begin(); it != resources.end(); ++it)
		{
			const size_t size = (*it).second->GetSizeInBytes();
			str += ("Name : " + (*it).second->GetName() + " -- Size : ");
			str += std::to_string(size);
		}

		return str + "\n";

	}

	//Retrieves element using resource name -- Hashed
	T* Get(const std::string &resourceName)
	{
		Log::ExitIfEmpty(resourceName, mapName + " : resource name cannot be empty");

		const std::string formattedResourceName = Log::TrimAndLower(resourceName);
		size_t hash = std::hash <std::string>{}(formattedResourceName);

		if (verbose)
		{
			std::cout << (mapName + "Looking for " + formattedResourceName + ".");
		}

		std::unordered_map<size_t, T*>::iterator it = resources.find(hash);

		//Yes, return pointer to element
		if (it != resources.end())
		{
			if (verbose) std::cout << " -- Found --" << std::endl;
			return it->second;
		}

		//if we get here, element couldn't be found
		if (verbose) std::cout << " -- NOT Found --" << std::endl;
		return nullptr;
	}

	void	AllowDuplicates()
	{
		allowDuplicates = true;
	}

	void	DisallowDuplicates()
	{
		allowDuplicates = false;
	}

	void	SetVerbose()
	{
		verbose = true;
	}

	void	SetQuiet()
	{
		verbose = false;
	}

	size_t	GetMaxSize() const
	{
		return maxSize;
	}

	size_t	GetCurrentSize()
	{
		size_t size = 0;

		for (std::unordered_map<size_t, T* >::iterator it = resources.begin(); it != resources.end(); ++it)
		{
			size += (*it).second->GetSizeInBytes();
		}

		return size;
	}

	size_t  FreeSpace() const
	{
		const size_t free = maxSize - currentSize;
		return free;
	}

	const std::string &GetName() const
	{
		return mapName;
	}

	const int Size() const
	{
		return resources.size();
	}

private:

	bool IsValNonUnique(const std::string &filename)
	{
		if (allowDuplicates)
		{
			return true;
		}

		//Check if element (by value) is already present.
		//If it is found, then return true, else exit with false.
		std::unordered_map<size_t, T* > ::iterator it = Map.begin();

		while (it != resources.end())
		{
			if ((it->second->GetName() == filename))
			{
				return false;
			}

			++it;
		}

		return true;
	}

	std::string mapName;							//Name of this resource mapper
	int verbose;
	int allowDuplicates;								//Allow or disallow duplicate resources
	std::unordered_map<unsigned int, T* > resources;	//Resource map
	size_t maxSize;								//Max size that this map can contain
	size_t currentSize;							//How much space currently being used

	ResourceMap(const ResourceMap&) {}

	ResourceMap &operator = (const ResourceMap&)
	{
		return *this;
	}
};

