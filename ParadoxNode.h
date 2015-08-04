#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// A ParadoxNode is a key-value pair where the value is one of:
// 1. A straightforward value (as a string) from "key = value"
// 2. A sequence of values (as ints) from "key = { value1, value2, ... }"
// 3. A collection of ParadoxNode children from "key = { ParadoxNode ParadoxNode ... }"
// A ParadoxNode may not contain a key in the case of the root node.
class ParadoxNode
{
public:
  static std::shared_ptr<ParadoxNode> CreateRoot();
  static std::shared_ptr<ParadoxNode> Create(std::string key);
  static std::shared_ptr<ParadoxNode> Create(std::string key, std::string textValue);
  static std::shared_ptr<ParadoxNode> Create(std::string key, std::vector<int> intValues);
  static std::shared_ptr<ParadoxNode> Create(std::string key, std::vector<std::shared_ptr<ParadoxNode>> children);

  // Returns the root node of a Paradox Script Syntax file. Note that the root node will have no key.
  static std::shared_ptr<ParadoxNode> Parse(std::string content, const std::string& sourceName);
  static std::shared_ptr<ParadoxNode> ParseFromFile(const std::string& fullPath);

  friend std::ostream& operator<<(std::ostream&, const std::shared_ptr<ParadoxNode>&);

  void SetKey(std::string key);
  void SetValue(std::string textValue);
  void SetValues(std::vector<int> intValues);
  void AddIntegerValue(int intValue);
  void SetChildren(std::vector<std::shared_ptr<ParadoxNode>>);
  void AddChild(std::shared_ptr<ParadoxNode>);

  const std::string& GetKey() const { return key; }
  const std::string& GetValue() const { return textValue; }
  const std::vector<int>& GetValues() const { return intValues; }
  const std::vector<std::shared_ptr<ParadoxNode>>& GetChildren() const { return children; }

  // Returns the first child with the given key. If there is no such child then nullptr is returned instead.
  std::shared_ptr<ParadoxNode> GetChild(const std::string& childKey) const;

  // Returns the value, but if the value has quotes (' or ") around it, then those quotes are stripped.
  std::string GetValueUnquoted() const;

private:
  ParadoxNode() {}

  void ClearValue();

  void Output(std::ostream&, int depth) const;

  std::string key;
  std::string textValue;
  std::vector<int> intValues;
  std::vector<std::shared_ptr<ParadoxNode>> children;
};