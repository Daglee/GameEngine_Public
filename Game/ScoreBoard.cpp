#include "ScoreBoard.h"
#include "../nclgl/Renderer.h"

#define TEXT_SIZE 15.0f
#define XAXIS_OFFSET 200.0f

ScoreBoard* ScoreBoard::instance = NULL;

ScoreBoard::ScoreBoard(Renderer* renderer)
{
	this->renderer = renderer;
}

void ScoreBoard::DisplayScores()
{
	OrderAscending();

	float yOffset = 0.0f;

	for each (Entry* entry in entries)
	{
		std::string entryString = entry->name + " : " + std::to_string(entry->score);

		Text entryText(entryString, Vector3(renderer->GetWidth() - XAXIS_OFFSET, yOffset, 0.0f), TEXT_SIZE);
		yOffset += TEXT_SIZE;

		renderer->textbuffer.push_back(entryText);
	}
}

void ScoreBoard::OrderAscending()
{
	std::sort(entries.begin(), entries.end(), Comparison());
}

void ScoreBoard::UpdateEntryScore(std::string name, int s)
{
	for each (Entry* entry in entries)
	{
		if (entry->name == name) {
			entry->score = entry->score + s;

			break;
		}
	}
}
