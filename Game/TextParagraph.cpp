#include "TextParagraph.h"

TextParagraph::TextParagraph(const int numberOfLines)
{
	this->numberOfLines = numberOfLines;
	paragraph = std::vector<std::string>(numberOfLines, "");
}

TextParagraph::TextParagraph()
{
	numberOfLines = 0;
}

void TextParagraph::SetNumberOfLinesInParagraph(const int& newNumberOfLines)
{
	numberOfLines = newNumberOfLines;
	paragraph = std::vector<std::string>(numberOfLines, "");
}

void TextParagraph::SetLineOfParagraph(const std::string& text, const int& lineNumber)
{
	paragraph[lineNumber] = text;
}

void TextParagraph::AppendLineToParagraph(const std::string & text)
{
	textAppendedToParagraph.push_back(text);
}

void TextParagraph::ClearAppendedText()
{
	textAppendedToParagraph.clear();
}

void TextParagraph::AddAdaptiveLine(const AdaptiveLine& text)
{
	adaptiveText.push_back(text);
}
