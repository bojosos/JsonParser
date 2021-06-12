#pragma once

#include "lexer.h"

#include <string>

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
    Node* parseElement();
    Node* parseValue();
    Node* parseObject();
    Node* parseString();
    Node* parseNumber();
    Node* parseArray();
    Node* parseBoolean();
    Node* parseMembers();
    JsonMember* parseMember();
    std::string parseInteger();
    std::string parseDecimal();
    std::string parseExponent();

    void error();

  private:
    bool m_ShouldThrow;
    Lexer m_Lexer;
  };

} // namespace json