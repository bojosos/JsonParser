#include "interpreter.h"
#include "json.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{

  std::string line;
  Interpreter interpreter;

  while (std::getline(std::cin, line))
  {
    if (line.empty())
      continue;
    try
    {
      interpreter.Process(line);
    }
    catch (const std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
  }

  return 0;
}