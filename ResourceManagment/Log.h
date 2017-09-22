#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>

using namespace std;

//Utility functions
class Log
{
public:

	Log(){}

	~Log(){}

	static string TrimAndLower(string s)
	{
		string newString = s;

		//Remove spaces
		newString.erase(remove_if(newString.begin(), newString.end(), 
			isspace), newString.end());

		//Make lowercase
		transform(newString.begin(), newString.end(),
				  newString.begin(), ::tolower);

		return newString;
	}

	static void ExitIfEmpty(string stringToCheck, string errorMessage)
	{
		if (stringToCheck.empty())
		{
			Error(errorMessage);
		}
	}

	static void Error(string s) 
	{
		try 
		{
			throw runtime_error(s);
		}
		catch (const runtime_error& error) 
		{
			cout << error.what();
		}
	}

	static vector<string> tokenise(string line)
	{
		std::istringstream iss(line);

		vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };

		return tokens;
	}

	static void Warning(string s)
	{
		string warning =
			"////////////////////////////////////////////////////\n----------------------WARNING----------------------\n" +
			s +
			"\n////////////////////////////////////////////////////\n";
		cout << warning;
	}

	static float Hash(string s) 
	{
		return static_cast<float>(std::hash<std::string>{}(s));
	}

	static bool FileExists(string filename)
	{
		std::ifstream infile(filename);

		if (infile.good()) return true;
		else return false;
	}
};

