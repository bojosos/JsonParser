#pragma once

#include "lexer.h"

#include <string>
#include <vector>

namespace json
{
  class Node;
  struct JsonMember;

  class Parser
  {
  public:
    Parser(const std::string& text, bool shouldThrow = true);
    Node* parseJson();

  private:
    /**
     * @brief Parses an element.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseElement();

    /**
     * @brief Parses a value.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseValue();

    /**
     * @brief Parses an object.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseObject();

    /**
     * @brief Parses a string.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseString();

    /**
     * @brief Parses a number. The number length is checked.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseNumber();

    /**
     * @brief Parses an array.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseArray();

    /**
     * @brief Parses the members of an object.
     * Refer to https://www.json.org/json-en.html
     *
     * @return Node*
     */
    Node* parseMembers();

    /**
     * @brief Parses a single member.
     * Refer to https://www.json.org/json-en.html
     *
     * @return JsonMember*
     */
    JsonMember* parseMember();

    /**
     * @brief Parses an integer.
     * Refer to https://www.json.org/json-en.html
     *
     * @return std::string
     */
    std::string parseInteger();

    /**
     * @brief Parses the decimal part after the . of a float. Returns an empty string if there is not one.
     * Refer to https://www.json.org/json-en.html
     *
     * @return std::string
     */
    std::string parseDecimal();

    /**
     * @brief Parses the exomponent of a float. Returns an empty string if there is not one.
     * Refer to https://www.json.org/json-en.html
     *
     * @return std::string
     */
    std::string parseExponent();

    void error();

  private:
    std::vector<Node*> m_AllocatedNodes;
    std::vector<JsonMember*> m_AllocatedMembers;
    std::vector<JsonMember**> m_AllocatedMemberArrays;
    std::vector<Node**> m_AllocatedNodeArrays;
    std::vector<char*> m_AllocatedCharArrays;
    bool m_ShouldThrow;
    Lexer m_Lexer;
  };

} // namespace json