#include "ParadoxNode.h"

#include <sstream>

#include "StringUtilities.h"

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

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key, std::string value)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  node->value = std::move(value);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Create(std::string key, std::vector<std::shared_ptr<ParadoxNode>> children)
{
  auto node = CreateRoot();
  node->key = std::move(key);
  node->children = std::move(children);
  return node;
}

std::shared_ptr<ParadoxNode> ParadoxNode::Parse(std::istream& in)
{
  auto root = ParadoxNode::CreateRoot();

  std::string currentNodeText;
  int openBraces = 0;
  while (!in.eof() && !in.fail())
  {
    std::string currentLine;
    std::getline(in, currentLine);

    auto commentPos = currentLine.find('#');
    if (commentPos != std::string::npos)
      currentLine.erase(commentPos); // remove any comment

    if (!currentNodeText.empty())
      currentNodeText.push_back('\n');  // preserve new-lines within an item
    currentNodeText += currentLine;
    openBraces += std::count(currentLine.begin(), currentLine.end(), '{');
    openBraces -= std::count(currentLine.begin(), currentLine.end(), '}');
    if (openBraces <= 0)
    {
      auto lastCharPos = currentNodeText.find_last_not_of(" \t");
      if (lastCharPos != std::string::npos && currentNodeText[lastCharPos] != '=')
      {
        auto newNode = CreateRoot();
        newNode->ParseNode(currentNodeText);
        if (!newNode->key.empty())
          root->children.push_back(newNode);
        currentNodeText.clear();
        openBraces = 0;
      }
    }
  }

  return root;
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

void ParadoxNode::SetValue(std::string value)
{
  this->value = std::move(value);
  children.clear();
}

void ParadoxNode::AddChild(std::shared_ptr<ParadoxNode> child)
{
  children.push_back(std::move(child));
  value.clear();
}

std::shared_ptr<ParadoxNode> ParadoxNode::GetChild(const std::string& childKey)
{
  for (const auto& child : children)
    if (child->key == childKey)
      return child;
  return nullptr;
}

std::string ParadoxNode::GetValueUnquoted() const
{
  if (value.size() > 2)
  {
    char start = value.front();
    if ((start == '\'' || start == '"') && start == value.back())
      return value.substr(1, value.size() - 2);
  }
  // No quotes - just return the unaltered value.
  return value;
}

void ParadoxNode::ParseNode(const std::string& nodeText)
{
  // Node is split as key = value.
  auto equalPos = nodeText.find('=');

  key = TrimWhitespace(nodeText.substr(0, equalPos));
  if (key.empty())
    return; // All items must have a key.

  value = TrimWhitespace(nodeText.substr(equalPos + 1));
  if (value.size() >= 2 && value.front() == '{' && value.back() == '}')
  { // This is a complex value that needs to be further passed.
    children = std::move(Parse(std::istringstream(value.substr(1, value.size() - 2)))->children);
    value.clear();
  }
}

void ParadoxNode::Output(std::ostream& out, int depth) const
{
  std::string indentation(2 * depth, ' ');

  out << indentation << key << " = ";

  if (!value.empty())
    out << value << '\n';
  else
  {
    out << "{\n";
    for (const auto& child : children)
      child->Output(out, depth + 1);
    out << indentation << "}\n";
  }
}
