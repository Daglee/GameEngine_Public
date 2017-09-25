#pragma once

#include "../nclgl/Vector3.h"

#include <string>
#include <vector>

struct AdaptiveLine
{
	std::string* line;
	Vector3 position;

	AdaptiveLine(std::string* line, const Vector3 position)
	{
		this->line = line;
		this->position = position;
	}
};

class TextParagraph
{
public:
	explicit TextParagraph(const int numberOfLines);
	TextParagraph();
	~TextParagraph() {}

	void SetNumberOfLinesInParagraph(const int& newNumberOfLines);
	void SetLineOfParagraph(const std::string& text, const int& lineNumber);

	void AppendLineToParagraph(const std::string& text);
	void ClearAppendedText();

	void AddAdaptiveLine(const AdaptiveLine& text);

	inline const std::vector<std::string>* GetParagraph() const
	{
		return &paragraph;
	}

	inline const std::vector<std::string>* GetAppendedText() const
	{
		return &textAppendedToParagraph;
	}

	inline const std::vector<AdaptiveLine>* GetPositionedLines() const
	{
		return &adaptiveText;
	}

private:
	int numberOfLines;
	std::vector<std::string> paragraph;
	std::vector<std::string> textAppendedToParagraph; //Saves resizing
	std::vector<AdaptiveLine> adaptiveText;
};

