#include "Resource.h"
#include "../Utilities/ErrorLog.h"

Resource::Resource(std::string resourcename, size_t resourcesize)
{
	if (resourcename.empty())
	{
		ErrorLog::Error("Empty filename not allowed.");
	}

	if (resourcesize == 0)
	{
		ErrorLog::Error("Size 0 not allowed.");
	}

	name = resourcename;
	size = resourcesize;

	references = 0;
}

void Resource::SetName(std::string name)
{
	if (name.empty())
	{
		ErrorLog::Error("Empty filename not allowed.");
	}

	this->name = name;
}

void Resource::SetSizeInBytes(size_t size)
{
	if (size == 0)
	{
		ErrorLog::Error("Size 0 not allowed.");
	}

	this->size = size;
}

std::string Resource::GetName()
{
	return name;
}

int Resource::GetReferencesCount()
{
	return references;
}

size_t Resource::GetSizeInBytes()
{
	return size;
}

void Resource::IncReferences()
{
	references++;
}

void Resource::DecReferences()
{
	references--;
}