#pragma once

#include "../nclgl/Vector3.h"

#include <string>

class Renderer;
class TextParagraph;

class PerspectiveHeadsUpDisplay
{
public:
	PerspectiveHeadsUpDisplay(Renderer* renderer, TextParagraph* linesOfText);
	explicit PerspectiveHeadsUpDisplay(Renderer* renderer = nullptr);
	~PerspectiveHeadsUpDisplay() {}

	void SetRenderer(Renderer* newRenderer);
	void SetText(TextParagraph* newText);
	void Set3DPosition(const Vector3& currentPosition);

	void DisplayAllText() const;

private:
	void DisplayParagraph() const;
	void DisplayAdaptiveText() const;

	void DisplayLine(const std::string& line, const Vector3& linePosition) const;
	Vector3 PositionAfterOffset(const Vector3& basePosition) const;

	Renderer* renderer;
	TextParagraph* linesOfText;
	Vector3 startingTextPosition;
};

