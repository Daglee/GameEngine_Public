#pragma once

#include <string>
#include <vector>
#include <algorithm>

class Renderer;

struct Entry
{
	std::string name;
	int score;

	Entry(std::string n, int s) : name(n), score(s) {}

	bool operator < (const Entry& rhs) const
	{
		return (score < rhs.score);
	}
};

//For std::sort.
struct Comparison
{
	bool const operator()(Entry* lhs, Entry* rhs) const
	{
		return lhs->score > rhs->score;
	}
};

/*
  Singleton that manages points for an entity.
  Adds text to top right of screen.
   - Sorts entries into descending order.
   - Can add points to entries.
*/
class ScoreBoard
{
public:
	static void Initialise(Renderer* renderer)
	{
		instance = new ScoreBoard(renderer);
	}

	static ScoreBoard* GetInstance()
	{
		return instance;
	}

	//Render it.
	void DisplayScores();

	//Duplicates will be allowed.
	void AddEntry(Entry* entry)
	{
		entries.push_back(entry);
	}

	//Adds the parameter score to the current score.
	void UpdateEntryScore(std::string name, int score) const;

private:
	explicit ScoreBoard(Renderer* renderer);
	~ScoreBoard()
	{
		Destroy();
	}

	static void Destroy()
	{
		delete instance;
	}

	//Using std::sort.
	void OrderAscending();
	void DisplayText(const std::string& entry, const float& yPosition) const;

	std::vector<Entry*> entries;
	Renderer* renderer;

	static ScoreBoard* instance;
};

