#pragma once

#include "json.h"

#include <string>
#include <vector>

class Interpreter
{
public:
  Interpreter() = default;

  /**
   * @brief Processes a single command.
   *
   * @param command
   */
  void Process(const std::string& command);

  /**
   * @brief Prints basic help information.
   *
   */
  static void ShowHelp();

private:
  /**
   * @brief Processes a "print" command and prints a json if one is loaded. Throws if an error occurs.
   */
  void ProcessPrint(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a 'search" command and prints the keys that have been found. Throws if an error occurs.
   */
  void ProcessSearch(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes an "edit" command and edits the current json. Assumes everything after the second arguement is
   * json. Throws if an error occurs.
   */
  void ProcessEdit(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "create" command and creates the enty in the json. Assumes everything after the second argument is
   * json. Throws if an error occurs.
   */
  void ProcessCreate(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "remove" command and creates the entry in the json. Throws if an error occurs.
   */
  void ProcessRemove(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "move" command and moves everything from the first key to the second. Throws if an error occurs.]
   */
  void ProcessMove(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "new" command and creates an empty json. Throws if an error occurs.
   */
  void ProcessNew(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "open" command and reads a json from disk. Throws if an error occurs.
   */
  void ProcessOpen(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "close" and closes the currently open json. Throws if an error occurs.
   */
  void ProcessClose(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "save" command and saves the currently open json. Throws if an error occurs.
   */
  void ProcessSave(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "saveas" command and saves the file to the specified path. Throws if an error occurs.
   */
  void ProcessSaveAs(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process an "exit" command. If a unsaved file is open asks the user if the file should be saved. Throws if an
   * error occurs.
   */
  void ProcessExit();

private:
  bool m_Saved = false;
  std::string m_Filepath;
  json::Json m_Json;
};