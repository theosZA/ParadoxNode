#include "ParadoxNodeParser.h"

#include <stack>
#include <stdexcept>

#include <pegtl.hh>

#include "ParadoxNode.h"

namespace ParadoxNodeParser {

// Grammar element rules

struct WhitespaceCharacter
  : pegtl::ascii::one<' ', '\t', '\n'> {};

struct Whitespace
  : pegtl::plus<WhitespaceCharacter> {};

struct Text
  : pegtl::plus<pegtl::ascii::not_one<'}', '\n'>> {};

struct Integer
  : pegtl::plus<pegtl::ascii::digit> {};

struct IntegerSequence
  : pegtl::list<Integer, Whitespace> {};

struct Key
  : pegtl::ascii::identifier {};

struct TextValue
  : Text {};

struct CompositeValue;

struct Composite
  : pegtl::seq<pegtl::ascii::one<'{'>, pegtl::pad<CompositeValue, WhitespaceCharacter>, pegtl::ascii::one<'}'>> {};

struct Value
  : pegtl::sor<Composite, TextValue> {};

struct Node
  : pegtl::seq<pegtl::pad<Key, WhitespaceCharacter>, pegtl::ascii::one<'='>, pegtl::pad<Value, WhitespaceCharacter>> {};

struct NodeSet
  : pegtl::star<Node> {};

struct CompositeValue
  : pegtl::sor<IntegerSequence, NodeSet> {};

// Actions

typedef std::vector<std::shared_ptr<ParadoxNode>> ActiveNodeSet;

template<typename Rule>
struct Action
  : pegtl::nothing<Rule> {};

template<> struct Action<Integer>
{
  static void apply(const pegtl::input& in, std::stack<ActiveNodeSet>& nodes)
  {
    // We need to move back down the stack in case we're still assuming child
    // nodes instead of the integer sequence we actually have.
    if (nodes.top().empty())
    {
      nodes.pop();
    }

    nodes.top().back()->AddIntegerValue(std::stoi(in.string()));
  }
};

template<> struct Action<Key>
{
  static void apply(const pegtl::input& in, std::stack<ActiveNodeSet>& nodes)
  {
    nodes.top().push_back(ParadoxNode::Create(in.string()));
  }
};

template<> struct Action<TextValue>
{
  static void apply(const pegtl::input& in, std::stack<ActiveNodeSet>& nodes)
  {
    std::string textValue = in.string();
    auto finalCharPos = textValue.find_last_not_of(" \t\n");
    if (finalCharPos != std::string::npos)
      nodes.top().back()->SetValue(textValue.substr(0, finalCharPos + 1));
  }
};

template<> struct Action<pegtl::ascii::one<'{'>>
{
  static void apply(const pegtl::input& in, std::stack<ActiveNodeSet>& nodes)
  {
    // Move up the stack assuming we're going to get child nodes.
    nodes.emplace();
  }
};

template<> struct Action<NodeSet>
{
  static void apply(const pegtl::input& in, std::stack<ActiveNodeSet>& nodes)
  {
    auto children = nodes.top();
    nodes.pop();
    nodes.top().back()->SetChildren(children);
  }
};



std::shared_ptr<ParadoxNode> Parse(std::string content, const std::string & sourceName)
{
  // Remove all comments.
  auto commentBegin = content.find('#');
  while (commentBegin != std::string::npos)
  {
    auto commentEnd = content.find('\n', commentBegin);
    if (commentEnd == std::string::npos)
    {
      content.erase(commentBegin);
      commentBegin = std::string::npos;
    }
    else
    {
      content.erase(commentBegin, commentEnd - commentBegin);
      commentBegin = content.find('#', commentBegin);
    }
  }

  // Parse.
  std::stack<ActiveNodeSet> nodes;
  nodes.emplace();
  nodes.top().push_back(ParadoxNode::CreateRoot());
  nodes.emplace();
  auto success = pegtl::parse<NodeSet, Action>(content, sourceName, nodes);
  if (!success)
  {
    throw std::runtime_error("Error parsing " + sourceName);
  }
  if (nodes.empty())
  {
    throw std::runtime_error("No node set created when parsing " + sourceName);
  }
  return nodes.top().front();
}

} // namespace ParadoxNodeParser