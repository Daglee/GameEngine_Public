#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>

using namespace std;

class StringUtility
{
public:

	static std::string TrimAndLower(const string s)
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

	static vector<string> Tokenise(const string line)
	{
		std::istringstream iss(line);

		vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };

		return tokens;
	}

	static float Hash(const string s)
	{
		return static_cast<float>(std::hash<std::string>{}(s));
	}

	static bool FileExists(const string filename)
	{
		std::ifstream infile(filename);

		return infile.good();
	}
};

