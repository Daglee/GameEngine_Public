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
		std::string line = entry->name + " : " + std::to_string(entry->score);
		DisplayText(line, yOffset);

		yOffset += TEXT_SIZE;
	}
}

void ScoreBoard::DisplayText(const std::string& entry, const float& yPosition)
{
	Vector3 linePosition(renderer->GetWidth() - XAXIS_OFFSET, yPosition, 0.0f);

	Text entryText(entry, linePosition, TEXT_SIZE);

	renderer->textbuffer.push_back(entryText);
}

void ScoreBoard::OrderAscending()
{
	std::sort(entries.begin(), entries.end(), Comparison());
}

void ScoreBoard::UpdateEntryScore(std::string name, int s)
{
	for each (Entry* entry in entries)
	{
		if (entry->name == name)
		{
			entry->score = entry->score + s;

			break;
		}
	}
}
