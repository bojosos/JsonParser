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
    /**
     * @brief Construct a new Parser object
     * 
     * @param text The json text.
     * @param shouldThrow Set to false if you want to parse the json partially and want the parser to try and fix unparsable json-s.
     */
    Parser(const std::string& text, bool shouldThrow = true);

    /**
     * @brief Parses the current json.
     * 
     * @return A Json. 
     */
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

    /**
     * @brief Called whenever an error during parsing occurs. Throws an exeption if m_ShouldThrow is set and clears the
     * allocated memory.
     *
     * @param expected What the parser actually exepcted.
     * @param got What the parser received.
     */
    void error(const std::string& expected, const std::string& got);

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