#pragma once

#include <string>
#include <vector>

class Utils
{
public:
  /**
   * @brief Splits a string.
   *
   * @param str String t split.
   * @param delim The separator to use when splitting the string. By default any whitespace is a separator.
   * @return A vector of the strings.
   */
  static std::vector<std::string> SplitString(const std::string& str, const std::string& delim = " ");
};