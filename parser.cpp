#include "parser.h"
#include "json.h"
#include "lexer.h"

#include <cstring>
#include <iostream>
#include <vector>

namespace json
{

  Parser::Parser(const std::string& text, bool shouldThrow) : m_ShouldThrow(shouldThrow)
  {
    m_Lexer = Lexer(text);
  }

  Node* Parser::parseJson()
  {
    Node* result = parseElement();
    if (m_Lexer.peek() != -1)
      error();

    return result;
  }

  Node* Parser::parseElement()
  {
    m_Lexer.skipWhitespace();
    Node* node = parseValue();
    m_Lexer.skipWhitespace();
    return node;
  }

  Node* Parser::parseValue()
  {
    char next = m_Lexer.peek();
    if (next == '{')
      return parseObject();
    else if (next == '[')
      return parseArray();
    else if (next == '"' || next == '\'')
      return parseString();
    if (Lexer::IsNumber(next))
    {
      return parseNumber();
    }

    std::string peek = m_Lexer.peekStr(5);
    if (peek.rfind("false", 0) == 0)
    {
      Node* boolean = new Node();
      boolean->m_Type = NodeType::Boolean;
      boolean->m_Data.boolean = false;
      m_Lexer.skipChars(5);
      return boolean;
    }

    if (peek.rfind("true", 0) == 0)
    {
      Node* boolean = new Node();
      boolean->m_Type = NodeType::Boolean;
      boolean->m_Data.boolean = true;
      m_Lexer.skipChars(4);
      return boolean;
    }

    if (peek.rfind("null", 0) == 0)
    {
      Node* null = new Node();
      null->m_Type = NodeType::Null;
      m_Lexer.skipChars(4);
      return null;
    }

    error();
    return new Node();
  }

  Node* Parser::parseObject()
  {
    m_Lexer.skipChar(); // {
    m_Lexer.skipWhitespace();
    Node* node = parseMembers();
    if (m_Lexer.peek() != '}')
    {
      error();
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != '}')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    return node;
  }

  Node* Parser::parseMembers()
  {
    std::vector<JsonMember*> members;
    while (m_Lexer.peek() != -1 && m_Lexer.peek() != '}')
    {
      m_Lexer.skipWhitespace();
      members.push_back(parseMember());
      m_Lexer.skipWhitespace();
      if (m_Lexer.peek() == ',')
        m_Lexer.skipChar();
    }
    Node* result = new Node();
    result->m_Type = NodeType::Object;
    result->m_Data.object.length = members.size();
    result->m_Data.object.values = new JsonMember*[members.size()];
    std::memcpy(result->m_Data.object.values, members.data(), members.size() * sizeof(Node*));
    return result;
  }

  JsonMember* Parser::parseMember()
  {
    m_Lexer.skipWhitespace();
    Node* name = parseString();
    if (m_Lexer.peek() != ':')
    {
      error();
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != ':')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    Node* element = parseElement();
    JsonMember* result = new JsonMember();
    result->nameNode = name;
    result->node = element;
    return result;
  }

  Node* Parser::parseArray()
  {
    m_Lexer.skipChar(); // [
    std::vector<Node*> elements;
    while (m_Lexer.peek() != -1 && m_Lexer.peek() != ']')
    {
      m_Lexer.skipWhitespace();
      elements.push_back(parseElement());
      m_Lexer.skipWhitespace();
      if (m_Lexer.peek() == ',')
        m_Lexer.skipChar();
    }

    Node* array = new Node();
    array->m_Type = NodeType::Array;
    array->m_Data.array.length = elements.size();
    array->m_Data.array.values = new Node*[elements.size()];
    std::memcpy(array->m_Data.array.values, elements.data(), elements.size() * sizeof(Node*));
    if (m_Lexer.peek() != ']')
    {
      error();
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != ']')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    return array;
  }

  Node* Parser::parseString()
  {
    char expectedClose;
    if (m_Lexer.peek() != '"' && m_Lexer.peek() != '\'')
      error();
    expectedClose = m_Lexer.peek(); // ' or "
    m_Lexer.skipChar();
    uint64_t length = 0;
    while (m_Lexer.peek(length) != -1 && (m_Lexer.peek(length) != expectedClose ||
                                          (m_Lexer.peek(length) == expectedClose && m_Lexer.peek(length - 1) == '\\')))
      length++;

    char* result = new char[length + 1];
    std::strncpy(result, m_Lexer.c_str(), length);
    result[length] = '\0';
    m_Lexer.skipChars(length);
    if (m_Lexer.peek() != expectedClose)
    {
      error();
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != '}')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    Node* node = new Node();
    node->m_Type = NodeType::String;
    node->m_Data.string.length = length;
    node->m_Data.string.ptr = result;
    return node;
  }

  Node* Parser::parseNumber()
  {
    std::string integerPart = parseInteger();
    std::string decimalPart = parseDecimal();
    std::string exponent = parseExponent();
    if (decimalPart.empty() && exponent.empty())
    {
      int res = std::stoll(integerPart);
      Node* node = new Node();
      node->m_Type = NodeType::Integer;
      node->m_Data.integer = res;
      return node;
    }
    double res = std::stold(integerPart + decimalPart + exponent);
    Node* result = new Node();
    result->m_Type = NodeType::Double;
    result->m_Data.dbl = res;
    return result;
  }

  std::string Parser::parseInteger()
  {
    std::string result;
    if (m_Lexer.peek() == '-')
    {
      result += '-';
      m_Lexer.skipChar();
    }

    while (m_Lexer.peek() != -1 && Lexer::IsNumber(m_Lexer.peek()))
    {
      result += m_Lexer.peek();
      m_Lexer.skipChar();
    }
    return result;
  }

  std::string Parser::parseDecimal()
  {
    if (m_Lexer.peek() != '.')
      return std::string();
    m_Lexer.skipChar();
    std::string result;
    while (m_Lexer.peek() != -1 && Lexer::IsNumber(m_Lexer.peek()))
    {
      result += m_Lexer.peek();
      m_Lexer.skipChar();
    }

    return result;
  }

  std::string Parser::parseExponent()
  {
    if (m_Lexer.peek() != 'e' && m_Lexer.peek() != 'E')
    {
      if (Lexer::IsAlphaNum(m_Lexer.peek()))
      {
        error();
        m_Lexer.skipChar();
      }
      else
        return std::string();
    }
    std::string result;
    result += m_Lexer.peek(); // add e
    m_Lexer.skipChar();
    if (m_Lexer.peek() == '-' || m_Lexer.peek() == '+')
    {
      result += m_Lexer.peek(); // add sign
      m_Lexer.skipChar();
    }
    if (m_Lexer.peek() != -1 && !Lexer::IsNumber(m_Lexer.peek()))
    {
      error();
      return std::string();
    }
    while (m_Lexer.peek() != -1 && Lexer::IsNumber(m_Lexer.peek()))
    {
      result += m_Lexer.peek();
      m_Lexer.skipChar();
    }
    return result;
  }

  void Parser::error()
  {
    std::string res =
      "Unexpected character at " + std::to_string(m_Lexer.getLine()) + ":" + std::to_string(m_Lexer.getColumn()) + "\n";
    if (m_ShouldThrow)
      throw std::runtime_error(res);
  }

} // namespace json