#include "json.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
  std::ifstream stream;
  if (argc > 1)
    stream = std::ifstream(argv[1]);
  else
    stream = std::ifstream("test.json");

  std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

  json::Node* result;
  try
  {
    result = json::JsonParser::Parse(str);
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return 0;
  }
  if (argc > 2 && std::strcmp(argv[2], "--output"))
    json::JsonParser::PrettyPrint(std::cout, result);
  return 0;
}