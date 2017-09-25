#include "ScoreBoard.h"
#include "../nclgl/Renderer.h"

const float TEXT_SIZE = 15.0f;
const float XAXIS_OFFSET = 200.0f;

ScoreBoard* ScoreBoard::instance = nullptr;

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
		string line = entry->name + " : " + to_string(entry->score);
		DisplayText(line, yOffset);

		yOffset += TEXT_SIZE;
	}
}

void ScoreBoard::DisplayText(const string& entry, const float& yPosition) const
{
	const Vector3 linePosition(renderer->GetWidth() - XAXIS_OFFSET, yPosition, 0.0f);

	const Text entryText(entry, linePosition, TEXT_SIZE);

	renderer->AddText(entryText);
}

void ScoreBoard::OrderAscending()
{
	sort(entries.begin(), entries.end(), Comparison());
}

void ScoreBoard::UpdateEntryScore(string name, int s) const
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
