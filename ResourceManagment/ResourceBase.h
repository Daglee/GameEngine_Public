#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include "Log.h"
#include <sstream>
#include <iterator>

/*
  Anything that will go in a resource manager
  must inherit from this.
  Implements functions required for the managers.
*/
class ResourceBase
{
public:

	template <class T> friend class  ResourceManager;

	ResourceBase(string resourcename, size_t resourcesize, void *args)
	{
		// exit with an error if filename is empty
		if (resourcename.empty()) {
			Log::Error("Empty filename not allowed.");
		}
		if (resourcesize == 0) {
			Log::Error("Size 0 not allowed.");
		}
		// init data members
		References = 0;
	}

	ResourceBase(){}

	virtual ~ResourceBase(){}

	virtual void SetResourceName(string name) {
		if (name.empty()) {
			Log::Error("Empty filename not allowed.");
		}

		ResourceName = name;
	}

	virtual void SetResourceSize(size_t size) {
		if (size == 0) {
			Log::Error("Size 0 not allowed.");
		}

		ResourceSize = size;
	}
	
	virtual std::string GetResourceName()
	{
		return ResourceName;
	}

	virtual int GetReferencesCount()
	{
		return References;
	}

	virtual std::size_t GetResourceSize()
	{
		return ResourceSize;
	}

	virtual void Read(string resourcename) = 0;

	virtual void ReadParams(string params) = 0;

protected:
	ResourceBase(const ResourceBase& object) { }
	ResourceBase& operator=(const ResourceBase& object) { return *this; }

private:
	int References;

	std::string ResourceName;
	std::size_t ResourceSize;

	void IncReferences() { References++; }
	void DecReferences() { References--; }

};

