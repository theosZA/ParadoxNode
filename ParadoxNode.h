#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// A ParadoxNode is a key-value pair where the value is either
// a straightforward value or a collection of more ParadoxNode children.
class ParadoxNode
{
public:
  static std::shared_ptr<ParadoxNode> CreateRoot();
  static std::shared_ptr<ParadoxNode> Create(std::string key);
  static std::shared_ptr<ParadoxNode> Create(std::string key, std::string value);
  static std::shared_ptr<ParadoxNode> Create(std::string key, std::vector<std::shared_ptr<ParadoxNode>> children);

  // Returns the root node of a Paradox Script Syntax file. Note that the root node will have no key.
  static std::shared_ptr<ParadoxNode> Parse(std::istream&);

  friend std::ostream& operator<<(std::ostream&, const std::shared_ptr<ParadoxNode>&);

  void SetKey(std::string key);
  void SetValue(std::string value);
  void AddChild(std::shared_ptr<ParadoxNode>);

  const std::string& GetKey() const { return key; }
  const std::string& GetValue() const { return value; }
  const std::vector<std::shared_ptr<ParadoxNode>>& GetChildren() const { return children; }

  // Returns the first child with the given key. If there is no such child then nullptr is returned instead.
  std::shared_ptr<ParadoxNode> GetChild(const std::string& childKey);

  // Returns the value, but if the value has quotes (' or ") around it, then those quotes are stripped.
  std::string GetValueUnquoted() const;

private:
  ParadoxNode() {}

  void ParseNode(const std::string& nodeText);
  void Output(std::ostream&, int depth) const;

  std::string key;
  std::string value;
  std::vector<std::shared_ptr<ParadoxNode>> children;
};