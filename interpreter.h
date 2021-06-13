#pragma once

#include "json.h"

#include <string>
#include <vector>

class Interpreter
{
public:
  Interpreter() = default;
  ~Interpreter();
  /**
   * @brief Processes a single command.
   *
   * @param command
   */
  void process(const std::string& command);

  /**
   * @brief Prints basic help information.
   *
   */
  static void ShowHelp();

private:
  /**
   * @brief Processes a "print" command and prints a json if one is loaded. Throws if an error occurs.
   */
  void processPrint(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a 'search" command and prints the keys that have been found. Throws if an error occurs.
   */
  void processSearch(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes an "edit" command and edits the current json. Assumes everything after the second arguement is
   * json. Throws if an error occurs.
   */
  void processEdit(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "create" command and creates the enty in the json. Assumes everything after the second argument is
   * json. Throws if an error occurs.
   */
  void processCreate(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "remove" command and creates the entry in the json. Throws if an error occurs.
   */
  void processRemove(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "move" command and moves everything from the first key to the second. Throws if an error occurs.]
   */
  void processMove(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "new" command and creates an empty json. Throws if an error occurs.
   */
  void processNew(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "open" command and reads a json from disk. Throws if an error occurs.
   */
  void processOpen(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "close" and closes the currently open json. Throws if an error occurs.
   */
  void processClose(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "save" command and saves the currently open json. Throws if an error occurs.
   */
  void processSave(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Processes a "saveas" command and saves the file to the specified path. Throws if an error occurs.
   */
  void processSaveAs(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "mode" command and changes the parsing mode.
   */
  void processSetMode(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "savesearch" command and save the file to the specified path. Throws if an error occurs.
   */
  void processSaveSearch(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "savecompact" command and save the file to the specified path in a as compact way as possible.
   */
  void processSaveCompact(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process a "savesearchcomapct" command and save the search to the specified path in a as compact way as
   * possible.
   */
  void processSaveSearchCompact(const std::string& line, const std::vector<std::string>& args);

  /**
   * @brief Process an "exit" command. If a unsaved file is open asks the user if the file should be saved. Throws if an
   * error occurs.
   */
  void processExit();

private:
  bool m_FullParse = true;
  bool m_Saved = false;
  json::Json m_Json = nullptr;
  std::string m_Filepath;
};