#pragma once

#include <memory>
#include <string>

class ParadoxNode;

namespace ParadoxNodeParser {

std::shared_ptr<ParadoxNode> Parse(std::string content, const std::string& sourceName);

} // namespace ParadoxNodeParser