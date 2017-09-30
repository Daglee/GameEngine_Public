#pragma once
#include <string>
#include <iostream>

using namespace std;

class ErrorLog
{
public:

	static void Error(const string s) 
	{
		const string error =
			"////////////////////////////////////////////////////\n----------------------ERROR----------------------\n" +
			s +
			"\n////////////////////////////////////////////////////\n";
		cout << error;
	}

	static void Warning(const string s)
	{
		const string warning =
			"\n----------------------WARNING----------------------\n" +
			s;
		cout << warning;
	}

	static void LogErrorAndThrowExceptionIfStringEmpty(const string stringToCheck, const string errorMessage)
	{
		if (stringToCheck.empty())
		{
			Error(errorMessage);
			throw new runtime_error(errorMessage);
		}
	}
};

