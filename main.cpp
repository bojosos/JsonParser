#include "interpreter.h"

#include <iostream>

int main(int argc, char** argv)
{

  std::string line;
  Interpreter interpreter;

  Interpreter::ShowHelp();
  while (std::getline(std::cin, line))
  {
    if (line.empty())
      continue;
    try
    {
      interpreter.process(line);
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: " << ex.what() << std::endl;
    }
  }

  return 0;
}