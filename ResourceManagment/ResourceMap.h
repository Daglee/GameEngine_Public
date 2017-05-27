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
		//Temporary values
		Verbose		= -1; 
		Duplicates	= -1;
	}

	~ResourceMap()
	{
		if (Verbose)  std::cout << Name + (" : Releasing\n");
		Clear();
	}

	void Initialise(const std::string &name, bool verbose, bool duplicates, size_t maximumSize)
	{
		if (name.empty()) Log::Error(Name + " : Array name cannot be empty");

		Name		= Log::TrimAndLower(name);
		Verbose		= verbose;
		Duplicates	= duplicates;

		//Just debug info...
		if (Verbose)
		{
			if (Duplicates)	std::cout << (Name + " : Allows duplicates") << std::endl;
			else			std::cout << (Name + " : Disallows duplicates") << std::endl;
		}

		maxSize = maximumSize;
		currentSize = 0;
	}

	//Add a new element!
	T *Add(const std::string &resourcename, T* resourceObj)
	{
		if (resourcename.empty()) Log::Error(Name + " : resource name cannot be empty");

		std::string ResourceName = Log::TrimAndLower(resourcename);
		
		if (Verbose) std::cout << ("Adding " + ResourceName) << std::endl;

		size_t hash = std::hash <std::string>{}(ResourceName);
		Map.insert(std::pair<size_t, T*>(hash, resourceObj));

		currentSize = currentSize + resourceObj->GetResourceSize();

		return resourceObj;
	}
	

	//Delete an element using resourcename
	void Remove(const std::string &resourcename)
	{
		if (resourcename.empty()) Log::Error(Name + " : resource name cannot be empty");

		std::string ResourceName = Log::TrimAndLower(resourcename);

		if (Verbose) std::cout << ("Removing " + ResourceName) << std::endl;

		size_t hash = std::hash <std::string>{}(ResourceName);
		currentSize = currentSize - Map.at(hash)->GetResourceSize();

		//delete Map.at(hash);
		Map.erase(hash);
	}

	//Clear all elements from bin
	void Clear()
	{
		std::unordered_map<size_t, T* >::iterator it = Map.begin();

		while (it != Map.end())
		{
			std::string filename = (*it).second->GetResourceName();
			size_t hash = std::hash <std::string>{}(filename);

			//Call destructor and remove from map
			delete(Map.find(hash)->second);
			it = Map.erase(hash);
		}

		currentSize = 0;
	}

	//Dumps bin content to a string
	std::string Dump()
	{
		std::string str = ("Dumping database - " +  Name + "\n");

		for (std::unordered_map<size_t, T* >::iterator it = Map.begin(); it != Map.end(); ++it) 
		{
			std::size_t size = (*it).second->GetResourceSize();
			str += ("Name : " + (*it).second->GetResourceName() + " -- Size : ");
			str += std::to_string(size);
		}

		return str + "\n";

	}

	//Retrieves element using resource name -- Hashed
	T* Get(const std::string &resourcename)
	{
		if (resourcename.empty()) Log::Error(Name + " : resource name cannot be empty");

		std::string ResourceName = Log::TrimAndLower(resourcename);
		size_t hash = std::hash <std::string>{}(ResourceName);

		std::unordered_map<size_t, T*>::iterator it;

		if (Verbose) std::cout << (Name + "Looking for " + ResourceName + ".");

		//Does this bin already contain this item?
		it = Map.find(hash);

		//Yes, return pointer to element
		if (it != Map.end())
		{
			if(Verbose) std::cout << " -- Found --" << std::endl;
			return it->second;
		}

		//if we get here, element couldn't be found
		if (Verbose) std::cout << " -- NOT Found --" << std::endl;
		return nullptr;
	}

	void	AllowDuplicates()		{ Duplicates = true; }
	void	DisallowDuplicates()	{ Duplicates = false; }
	void	SetVerbose()			{ Verbose = true; }
	void	SetQuiet()				{ Verbose = false; }

	size_t	GetMaxSize()			{ return maxSize; }

	size_t	GetCurrentSize() { 
		size_t size = 0;

		for (std::unordered_map<size_t, T* >::iterator it = Map.begin(); it != Map.end(); ++it) {
			size += (*it).second->GetResourceSize();
		}

		return size;
	}

	size_t  FreeSpace()
	{ 
		size_t free = maxSize - currentSize;
		return free;
	}

	const std::string &GetName() const { return Name; }
	const int Size() const { return Map.size(); }

private:

	bool IsValNonUnique(const std::string &filename)
	{
		if (Duplicates) return true;

		//Check if element (by value) is already present.
		//If it is found, then return true, else exit with false.
		std::unordered_mapsize_t, T*>::iterator it = Map.begin();

		while (it != Map.end())
		{
			if ((it->second->GetResourceName() == filename)) return false;
			++it;
		}

		return true;
	}

	std::string Name;							//Name of this resource mapper
	int Verbose;
	int Duplicates;								//Allow or disallow duplicate resources
	std::unordered_map<unsigned int, T* > Map;	//Resource map
	size_t maxSize;								//Max size that this map can contain
	size_t currentSize;							//How much space currently being used

	ResourceMap(const ResourceMap&) { };
	ResourceMap &operator = (const ResourceMap&) { return *this; }
};

