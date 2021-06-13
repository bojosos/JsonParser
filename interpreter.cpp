#include "interpreter.h"

#include "utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

Interpreter::~Interpreter()
{
  json::JsonParser::JsonFree(m_Json);
}

void Interpreter::processPrint(const std::string& line, const std::vector<std::string>& args)
{
  if (m_Json == nullptr)
    throw std::runtime_error("No document open.");
  json::JsonParser::PrettyPrint(m_Json);
}

void Interpreter::processSave(const std::string& line, const std::vector<std::string>& args)
{
  if (m_Json == nullptr)
    throw std::runtime_error("No document open.");
  if (m_Filepath.empty())
  {
    std::cout << "Where should the file be saved?" << std::endl;
    std::getline(std::cin, m_Filepath);
  }
  if (m_Json != nullptr)
  {
    std::ofstream output(m_Filepath);
    if (output.is_open())
    {
      json::JsonParser::PrettyPrint(m_Json, output);
      output.close();
    }
  }
  m_Saved = true;
  std::cout << "File saved to " << m_Filepath << "." << std::endl;
}

void Interpreter::processSaveAs(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invalid args.");
  std::string path = line.substr(args[0].size() + 1, line.size() - args[0].size()); // cut out command + first space

  std::string resultArg;
  if (path[0] == '"') //
  {
    uint32_t idx = 0;
    while (path[idx + 1] != '"')
      resultArg += path[idx++];
  }
  else
    resultArg = args[1];
  if (m_Json != nullptr)
  {
    std::ofstream output(resultArg);
    if (output.is_open())
    {
      json::JsonParser::PrettyPrint(m_Json, output);
      output.close();
    }
  }
  else
    throw std::runtime_error("No document open.");
  m_Saved = true;
  m_Filepath = resultArg;
  throw std::runtime_error("Failed to save " + resultArg + ".");
}

void Interpreter::processClose(const std::string& line, const std::vector<std::string>& args)
{
  if (!m_Saved)
  {
    std::cout << "Do you want to save the changes you made to " << m_Filepath << "y/n" << std::endl;
    char c;
    std::cin >> c;
    if (c == 'y')
      processSave(line, args);
  }
  json::JsonParser::JsonFree(m_Json);
  m_Json = nullptr;
  m_Filepath.clear();
  m_Saved = false;
  std::cout << "File closed." << std::endl;
}

void Interpreter::processNew(const std::string& line, const std::vector<std::string>& args)
{
  m_Json = json::JsonParser::Parse("{}");
  m_Saved = false;
  m_Filepath = "";
  std::cout << "Empty document created." << std::endl;
}

void Interpreter::processOpen(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invalid args.");
  std::string path = line.substr(args[0].size() + 1, line.size() - args[0].size()); // cut out command + first space

  std::string resultArg;
  if (path[0] == '"') //
  {
    uint32_t idx = 0;
    while (path[idx + 1] != '"')
      resultArg += path[idx++];
  }
  else
    resultArg = args[1];
  std::ifstream input(resultArg);
  if (input.is_open())
  {
    std::string str((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    m_Json = m_FullParse ? json::JsonParser::Parse(str) : json::JsonParser::ParsePartially(str);
    m_Filepath = resultArg;
    json::JsonParser::PrettyPrint(m_Json);
  }
  else
    throw std::runtime_error("Document not found.");
  m_Saved = true;
}

void Interpreter::processSearch(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invalid args.");

  if (!m_Json)
    throw std::runtime_error("No document open.");

  json::Json array = m_Json->search(args[1]);
  json::JsonParser::PrettyPrint(array);
  json::JsonParser::JsonFree(array);
}

void Interpreter::processRemove(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");
  std::string path = args[1]; // cut out command + first space

  try
  {
    m_Json->remove(path);
    std::cout << "Element " << path << " removed." << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cout << "Element does not exist." << std::endl;
  }
  m_Saved = false;
}

void Interpreter::processMove(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args.");
  if (!m_Json)
    throw std::runtime_error("No document open.");

  m_Json->move(args[1], args[2]);
  std::cout << "Element " << args[1] << " moved." << std::endl;
  m_Saved = false;
}

void Interpreter::processEdit(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");
  std::string path = args[1]; // cut out command + first space
  std::string json =
    line.substr(args[0].size() + 1 + args[1].size() + 1, line.size() - args[0].size() - 1 - args[1].size() - 1);

  m_Json->edit(path, json, m_FullParse);
  std::cout << "Value editted." << std::endl;
  m_Saved = false;
}
void Interpreter::processCreate(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 4)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");
  std::string path = args[1]; // cut out command + first space
  std::string key = args[2];
  std::string json = line.substr(args[0].size() + 1 + args[1].size() + 1 + args[2].size() + 1,
                                 line.size() - args[0].size() - 1 - args[1].size() - 1 - args[2].size());

  m_Json->create(path, key, json, m_FullParse);
  std::cout << "Member created." << std::endl;
  m_Saved = false;
}

void Interpreter::processSetMode(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invaild args");
  if (args[1] == "partial")
  {
    std::cout << "Mode set to partial." << std::endl;
    m_FullParse = false;
  }
  else if (args[1] == "full")
  {
    std::cout << "Mode set to full." << std::endl;
    m_FullParse = true;
  }
  else
    throw std::runtime_error("Invalid mode.");
}

void Interpreter::processSaveSearch(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");

  std::ofstream output(args[2]);
  if (output.is_open())
  {
    json::Json array = m_Json->search(args[1]);
    if (array->getSize() == 0)
      throw std::runtime_error("Key not found");
    else if (array->getSize() == 1)
      json::JsonParser::PrettyPrint(array->data.array.values[0], output);
    else
      json::JsonParser::PrettyPrint(array, output);
    json::JsonParser::JsonFree(array);
    output.close();
    std::cout << "Search result saved to " << args[2] << "." << std::endl;
  }
  else
    throw std::runtime_error("Invalid path.");
}

void Interpreter::processSaveCompact(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invalid args.");
  std::string path = line.substr(args[0].size() + 1, line.size() - args[0].size()); // cut out command + first space

  std::string resultArg;
  if (path[0] == '"') //
  {
    uint32_t idx = 0;
    while (path[idx + 1] != '"')
      resultArg += path[idx++];
  }
  else
    resultArg = args[1];
  if (m_Json != nullptr)
  {
    std::ofstream output(resultArg);
    if (output.is_open())
    {
      json::JsonParser::CompactPrint(m_Json, output);
      output.close();
    }
  }
  else
    throw std::runtime_error("No document open.");
  m_Saved = true;
  std::cout << "Search result saved to " << args[1] << "." << std::endl;
}

void Interpreter::processSaveSearchCompact(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");

  std::ofstream output(args[2]);
  if (output.is_open())
  {
    json::Json array = m_Json->search(args[1]);
    if (array->getSize() == 0)
      throw std::runtime_error("Key not found");
    else if (array->getSize() == 1)
      json::JsonParser::CompactPrint(array->data.array.values[0], output);
    else
      json::JsonParser::CompactPrint(array, output);
    json::JsonParser::JsonFree(array);
    std::cout << "Search result saved to " << args[2] << "." << std::endl;
    output.close();
  }
  else
    throw std::runtime_error("Invalid path.");
}

void Interpreter::processExit()
{
  if (m_Json)
  {
    if (!m_Saved)
    {
      std::cout << "The open document has not been saved. Would you like to save it before exiting? y/n" << std::endl;
      char ans;
      std::cin >> ans;
      if (ans == 'y')
        processSave("", {});
      else
        json::JsonParser::JsonFree(m_Json);
    }
  }

  exit(0);
}

void Interpreter::ShowHelp()
{
  std::cout
    << "new                                 Create an empty document." << '\n'
    << "open <filepath>                     Open a document." << '\n'
    << "print                               Print the open document." << '\n'
    << "mode <mode>                         Sets the parsing mode of the program. Possible values are \"partial\" "
       "and \"full\""
    << '\n'
    << "                                    In partial mode the program will attempt to fix the json before "
       "executing a command."
    << '\n'
    << "                                    In full mode the programm will not execute the command if the json "
       "cannot be parsed."
    << '\n'
    << "save                                Save the open document." << '\n'
    << "saveas <filepath>                   Save the open document to another path." << '\n'
    << "savecompact <filepath>              Saves the document compactly to the filepath." << '\n'
    << "savesearchcompact <key> <filepath>  Saves the search compactly to the filepath." << '\n'
    << "savesearch <key> <filepath>         Saves the search result to the file." << '\n'
    << "close                               Close the open document." << '\n'
    << "move <path1> <path2>                Move the elements of path1 to path2." << '\n'
    << "remove <path>                       Remove the element at path." << '\n'
    << "edit <path> <json>                  Set the value of the element at path to the parsed json." << '\n'
    << "create <path> <key> <json>          Creates a new entry at path with the key and parsed json." << '\n'
    << "search <key>                        Searches for an element and prints the result as a json array." << '\n';
}

void Interpreter::process(const std::string& line)
{
  auto args = Utils::SplitString(line, " ");
  std::string& command = args[0];
  std::transform(command.begin(), command.end(), command.begin(), ::tolower);

  if (command == "print")
    processPrint(line, args);
  else if (command == "new")
    processNew(line, args);
  else if (command == "open")
    processOpen(line, args);
  else if (command == "close")
    processClose(line, args);
  else if (command == "save")
    processSave(line, args);
  else if (command == "saveas")
    processSaveAs(line, args);
  else if (command == "savesearch")
    processSaveSearch(line, args);
  else if (command == "savecompact")
    processSaveCompact(line, args);
  else if (command == "savesearchcompact")
    processSaveSearchCompact(line, args);
  else if (command == "mode")
    processSetMode(line, args);
  else if (command == "remove")
    processRemove(line, args);
  else if (command == "move")
    processMove(line, args);
  else if (command == "search")
    processSearch(line, args);
  else if (command == "edit")
    processEdit(line, args);
  else if (command == "create")
    processCreate(line, args);
  else if (command == "exit")
    processExit();
  else if (command == "help")
    ShowHelp();
  else
    throw std::runtime_error("Invalid command.");
}
