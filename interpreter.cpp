#include "interpreter.h"

#include "utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

void Interpreter::ProcessPrint(const std::string& line, const std::vector<std::string>& args)
{
  if (m_Json == nullptr)
    throw std::runtime_error("No document open.");
  json::JsonParser::PrettyPrint(m_Json);
}

void Interpreter::ProcessSave(const std::string& line, const std::vector<std::string>& args)
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
      json::JsonParser::PrettyPrint(m_Json, output);
  }
  m_Saved = true;
  std::cout << "File saved to " << m_Filepath << "." << std::endl;
}

void Interpreter::ProcessSaveAs(const std::string& line, const std::vector<std::string>& args)
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
      json::JsonParser::PrettyPrint(m_Json, output);
  }
  else
    throw std::runtime_error("No document open.");
  m_Saved = true;
  std::cout << "Filed saved to " << resultArg << "." << std::endl;
}

void Interpreter::ProcessClose(const std::string& line, const std::vector<std::string>& args)
{
  if (!m_Saved)
  {
    std::cout << "Do you want to save the changes you made to " << m_Filepath << "y/n" << std::endl;
    char c;
    std::cin >> c;
    if (c == 'y')
      ProcessSave(line, args);
  }
  json::JsonParser::JsonFree(m_Json);
  m_Json = nullptr;
  m_Filepath.clear();
  m_Saved = false;
  std::cout << "File closed." << std::endl;
}

void Interpreter::ProcessNew(const std::string& line, const std::vector<std::string>& args)
{
  m_Json = json::JsonParser::Parse("{}");
  m_Saved = false;
  m_Filepath = "";
  std::cout << "Empty document created." << std::endl;
}

void Interpreter::ProcessOpen(const std::string& line, const std::vector<std::string>& args)
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
    m_Json = json::JsonParser::Parse(str);
    m_Filepath = resultArg;
    json::JsonParser::PrettyPrint(m_Json);
  }
  else
    throw std::runtime_error("Document not found.");
  m_Saved = true;
}

void Interpreter::ProcessSearch(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 2)
    throw std::runtime_error("Invalid args.");

  if (!m_Json)
    throw std::runtime_error("No document open.");

  json::Json array = m_Json->search(args[1]);
  json::JsonParser::PrettyPrint(array);
}

void Interpreter::ProcessRemove(const std::string& line, const std::vector<std::string>& args)
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

void Interpreter::ProcessMove(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args.");
  if (!m_Json)
    throw std::runtime_error("No document open.");

  m_Json->move(args[1], args[2]);
  std::cout << "Element " << args[1] << " moved." << std::endl;
  m_Saved = false;
}

void Interpreter::ProcessEdit(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 3)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");
  std::string path = args[1]; // cut out command + first space
  std::string json =
    line.substr(args[0].size() + 1 + args[1].size() + 1, line.size() - args[0].size() - 1 - args[1].size() - 1);

  m_Json->edit(path, json);
  std::cout << "Value editted." << std::endl;
  m_Saved = false;
}
void Interpreter::ProcessCreate(const std::string& line, const std::vector<std::string>& args)
{
  if (args.size() < 4)
    throw std::runtime_error("Invaild args");
  if (!m_Json)
    throw std::runtime_error("No document open.");
  std::string path = args[1]; // cut out command + first space
  std::string key = args[2];
  std::string json = line.substr(args[0].size() + 1 + args[1].size() + 1 + args[2].size() + 1,
                                 line.size() - args[0].size() - 1 - args[1].size() - 1 - args[2].size());

  m_Json->create(path, key, json);
  std::cout << "Member created." << std::endl;
  m_Saved = false;
}

void Interpreter::ProcessExit()
{
  if (m_Json)
  {
    if (!m_Saved)
    {
      std::cout << "The open document has not been saved. Would you like to save it before exiting? y/n" << std::endl;
      char ans;
      std::cin >> ans;
      if (ans == 'y')
        ProcessSave("", {});
      else
        json::JsonParser::JsonFree(m_Json);
    }
  }

  exit(0);
}

void Interpreter::ShowHelp()
{
  std::cout << "VEHICLE <registration> <description>" << '\n'
            << "PERSON <name> <id>" << '\n'
            << "ACQUIRE <owner-id> <vehicle-id>" << '\n'
            << "RELEASE <owner-id> <vehicle-id>" << '\n'
            << "REMOVE <what>" << '\n'
            << "SAVE <path>" << '\n'
            << "SHOW [PEOPLE|VEHICLES|<id>" << std::endl;
}

void Interpreter::Process(const std::string& line)
{
  auto args = Utils::SplitString(line, " ");
  std::string& command = args[0];
  std::transform(command.begin(), command.end(), command.begin(), ::tolower);

  if (command == "print")
    ProcessPrint(line, args);
  else if (command == "new")
    ProcessNew(line, args);
  else if (command == "open")
    ProcessOpen(line, args);
  else if (command == "close")
    ProcessClose(line, args);
  else if (command == "save")
    ProcessSave(line, args);
  else if (command == "saveas")
    ProcessSaveAs(line, args);
  else if (command == "remove")
    ProcessRemove(line, args);
  else if (command == "move")
    ProcessMove(line, args);
  else if (command == "search")
    ProcessSearch(line, args);
  else if (command == "edit")
    ProcessEdit(line, args);
  else if (command == "create")
    ProcessCreate(line, args);
  else if (command == "exit")
    ProcessExit();
  else if (command == "help")
    ShowHelp();
  else
    throw std::runtime_error("Invalid command");
}
