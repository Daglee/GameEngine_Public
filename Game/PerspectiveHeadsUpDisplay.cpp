#include "PerspectiveHeadsUpDisplay.h"

#include "../nclgl/Renderer.h"
#include "../nclgl/Text.h"

const bool PERSPECTIVE = true;
const float FONT_SIZE = 25.0f;

PerspectiveHeadsUpDisplay::PerspectiveHeadsUpDisplay(Renderer* renderer, TextParagraph* linesOfText)
{
	this->renderer = renderer;
	this->linesOfText = linesOfText;
}

PerspectiveHeadsUpDisplay::PerspectiveHeadsUpDisplay(Renderer* renderer)
{
	this->renderer = renderer;
}

void PerspectiveHeadsUpDisplay::SetRenderer(Renderer* newRenderer)
{
	renderer = newRenderer;
}

void PerspectiveHeadsUpDisplay::SetText(TextParagraph* newText)
{
	linesOfText = newText;
}

void PerspectiveHeadsUpDisplay::Set3DPosition(const Vector3& currentPosition)
{
	startingTextPosition = currentPosition;
}

void PerspectiveHeadsUpDisplay::DisplayAllText()
{
	DisplayParagraph();
	DisplayAdaptiveText();
}

void PerspectiveHeadsUpDisplay::DisplayParagraph()
{
	Vector3 initialPosition(0, 70, 0);
	Vector3 lineSpacing(0, 30, 0);

	Vector3 linePosition = initialPosition + lineSpacing;

	for each (std::string line in *linesOfText->GetParagraph())
	{
		DisplayLine(line, linePosition);
		linePosition = linePosition + lineSpacing;
	}

	for each (std::string line in *linesOfText->GetAppendedText())
	{
		DisplayLine(line, linePosition);
		linePosition = linePosition + lineSpacing;
	}
}

void PerspectiveHeadsUpDisplay::DisplayAdaptiveText()
{
	for each(AdaptiveLine adaptiveLine in *linesOfText->GetPositionedLines())
	{
		DisplayLine(*adaptiveLine.line, adaptiveLine.position);
	}
}

void PerspectiveHeadsUpDisplay::DisplayLine(const std::string& line, const Vector3& linePosition)
{
	Text text(line, PositionAfterOffset(linePosition), FONT_SIZE, PERSPECTIVE);
	renderer->textbuffer.push_back(text);
}

Vector3 PerspectiveHeadsUpDisplay::PositionAfterOffset(const Vector3& basePosition)
{
	return startingTextPosition + basePosition;
}
