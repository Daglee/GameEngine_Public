#pragma once

#include "TextParagraph.h"
#include "../nclgl/Vector3.h"

#include <string>

class Renderer;

class PerspectiveHeadsUpDisplay
{
public:
	PerspectiveHeadsUpDisplay(Renderer* renderer, TextParagraph* linesOfText);
	PerspectiveHeadsUpDisplay(Renderer* renderer = nullptr);
	~PerspectiveHeadsUpDisplay() {}

	void SetRenderer(Renderer* newRenderer);
	void SetText(TextParagraph* newText);
	void Set3DPosition(const Vector3& currentPosition);

	void DisplayAllText();

private:
	void DisplayParagraph();
	void DisplayAdaptiveText();

	void DisplayLine(const std::string& line, const Vector3& linePosition);
	Vector3 PositionAfterOffset(const Vector3& basePosition);

	Renderer* renderer;
	TextParagraph* linesOfText;
	Vector3 startingTextPosition;
};

