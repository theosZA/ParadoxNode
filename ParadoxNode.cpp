#include "ParadoxNode.h"

#include <cstdio>
#include <fstream>
#include <stdexcept>

#include "ParadoxNodeParser.h"

std::string ReadFile(const char* fileName)
{
  FILE* file = fopen(fileName, "r");
  if (!file)
    throw std::runtime_error("Failed to open file " + std::string(fileName));

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  char* buffer = new char[size];
  rewind(file);

  fread(buffer, sizeof(char), size, file);
  std::string content(buffer, size);

  delete[] buffer;
  return content;
}

std::shared_ptr<ParadoxNode> ParadoxNode::CreateRoot()
{
  return std::shared_ptr<ParadoxNode>(new ParadoxNode);
}

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key, std::string textValue)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  node->textValue = std::move(textValue);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key, std::vector<int> intValues)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  node->intValues = std::move(intValues);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key, std::vector<std::shared_ptr<ParadoxNode>> children)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  node->children = std::move(children);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Parse(std::string content, const std::string& sourceName)
{
  return ParadoxNodeParser::Parse(content, sourceName);
}

std::shared_ptr<ParadoxNode> ParadoxNode::ParseFromFile(const std::string & fullPath)
{
  auto separator = fullPath.find_last_of('\\');
  auto sourceFileName = (separator == std::string::npos ? fullPath : fullPath.substr(separator + 1));

  return ParadoxNode::Parse(ReadFile(fullPath.c_str()), sourceFileName);
}

void ParadoxNode::WriteToFile(const std::string& fullPath) const
{
  std::ofstream out(fullPath);
  if (!out)
    throw std::runtime_error("Error writing file " + fullPath);
  Output(out, 0);
}

std::ostream& operator<<(std::ostream& out, const std::shared_ptr<ParadoxNode>& node)
{
  if (node)
    node->Output(out, 0);
  return out;
}

void ParadoxNode::SetKey(std::string key)
{
  this->key = std::move(key);
}

void ParadoxNode::SetValue(std::string textValue)
{
  ClearValue();
  this->textValue = std::move(textValue);
}

void ParadoxNode::SetValues(std::vector<int> intValues)
{
  ClearValue();
  this->intValues = std::move(intValues);
}

void ParadoxNode::AddIntegerValue(int intValue)
{
  textValue.clear();
  children.clear();
  intValues.push_back(intValue);
}

void ParadoxNode::SetChildren(std::vector<std::shared_ptr<ParadoxNode>> children)
{
  ClearValue();
  this->children = std::move(children);
}

void ParadoxNode::AddChild(std::shared_ptr<ParadoxNode> child)
{
  textValue.clear();
  intValues.clear();
  children.push_back(std::move(child));
}

std::shared_ptr<ParadoxNode> ParadoxNode::GetChild(const std::string& childKey) const
{
  for (const auto& child : children)
    if (child->key == childKey)
      return child;
  return nullptr;
}

std::string ParadoxNode::GetValueUnquoted() const
{
  if (textValue.size() > 2)
  {
    char start = textValue.front();
    if ((start == '\'' || start == '"') && start == textValue.back())
      return textValue.substr(1, textValue.size() - 2);
  }
  // No quotes - just return the unaltered value.
  return textValue;
}

void ParadoxNode::ClearValue()
{
  textValue.clear();
  intValues.clear();
  children.clear();
}

void ParadoxNode::Output(std::ostream& out, int depth) const
{
  if (key.empty())
  { // Only child nodes are allowed.
    for (const auto& child : children)
      child->Output(out, depth);
    return;
  }

  std::string indentation(2 * depth, ' ');

  out << indentation << key << " = ";

  if (!textValue.empty())
    out << textValue << '\n';
  else if (!intValues.empty())
  {
    out << '{';
    for (const auto& intValue : intValues)
      out << ' ' << intValue;
    out << " }\n";
  }
  else
  {
    out << "{\n";
    for (const auto& child : children)
      child->Output(out, depth + 1);
    out << indentation << "}\n";
  }
}
