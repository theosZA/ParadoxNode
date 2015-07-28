#include "StringUtilities.h"

#include <locale>

std::string TrimWhitespace(const std::string& text)
{
  auto newBegin = text.find_first_not_of(" \t\n");
  if (newBegin == std::string::npos)
    return "";
  auto newEnd = text.find_last_not_of(" \t\n");
  return text.substr(newBegin, newEnd - newBegin + 1);
}

std::vector<std::string> SplitString(const std::string& text, char delimiter)
{
  std::vector<std::string> result;
  size_t beginPos = 0;
  size_t delimPos = 0;
  while (delimPos != std::string::npos)
  {
    delimPos = text.find(delimiter, delimPos + 1);
    if (delimPos == std::string::npos)
      result.emplace_back(text.substr(beginPos));
    else
      result.emplace_back(text.substr(beginPos, delimPos - beginPos));
    beginPos = delimPos + 1;
  }
  return result;
}

std::string StripQuotes(const std::string& text)
{
  if (text.size() >= 2 && text.front() == '"' && text.back() == '"')
    return text.substr(1, text.size() - 2);
  else
    return text;
}
