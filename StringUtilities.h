#pragma once

#include <string>
#include <vector>

// Removes spaces before and after the text.
std::string TrimWhitespace(const std::string& text);

// Splits a string into a sequence of sub-strings separated by the given delimiter character.
std::vector<std::string> SplitString(const std::string& text, char delimiter);

// Returns a string without any surrounding quotes (if there are any).
std::string StripQuotes(const std::string& text);
