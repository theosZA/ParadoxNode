#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "..\ParadoxNode.h"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " input_file\n";
    exit(1);
  }

  std::string inputFileName(argv[1]);
  // Read the whole input into a string first so file access is not included in timings.
  std::ifstream inputFile(inputFileName);
  inputFile >> std::noskipws;
  std::string inputText(std::istream_iterator<char>(inputFile), (std::istream_iterator<char>()));
  std::istringstream inputTextStream(inputText);

  auto rootNode = ParadoxNode::Parse(inputTextStream);
  std::cout << rootNode;
}