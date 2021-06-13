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
      error("EOF", m_Lexer.peekStr(1));
    m_AllocatedCharArrays.clear();
    m_AllocatedMemberArrays.clear();
    m_AllocatedMembers.clear();
    m_AllocatedNodeArrays.clear();
    m_AllocatedNodes.clear();
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
      m_AllocatedNodes.push_back(boolean);
      boolean->type = NodeType::Boolean;
      boolean->data.boolean = false;
      m_Lexer.skipChars(5);
      return boolean;
    }

    if (peek.rfind("true", 0) == 0)
    {
      Node* boolean = new Node();
      m_AllocatedNodes.push_back(boolean);
      boolean->type = NodeType::Boolean;
      boolean->data.boolean = true;
      m_Lexer.skipChars(4);
      return boolean;
    }

    if (peek.rfind("null", 0) == 0)
    {
      Node* null = new Node();
      m_AllocatedNodes.push_back(null);
      null->type = NodeType::Null;
      m_Lexer.skipChars(4);
      return null;
    }

    error("object/array/string/true/false/number/null", m_Lexer.peekStr(5));
    Node* empty = new Node();
    empty->type = NodeType::Object;
    empty->data.object.length = 0;
    empty->data.object.values = nullptr;
    return empty;
  }

  Node* Parser::parseObject()
  {
    m_Lexer.skipChar(); // {
    m_Lexer.skipWhitespace();
    Node* node = parseMembers();
    if (m_Lexer.peek() != '}')
    {
      error("}", m_Lexer.peekStr(1));
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
    m_AllocatedNodes.push_back(result);
    result->type = NodeType::Object;
    result->data.object.length = members.size();
    result->data.object.values = new JsonMember*[members.size()];
    m_AllocatedMemberArrays.push_back(result->data.object.values);
    std::memcpy(result->data.object.values, members.data(), members.size() * sizeof(JsonMember*));
    return result;
  }

  JsonMember* Parser::parseMember()
  {
    m_Lexer.skipWhitespace();
    Node* name = parseString();
    if (m_Lexer.peek() != ':')
    {
      error(":", m_Lexer.peekStr(1));
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != ':')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    Node* element = parseElement();
    JsonMember* result = new JsonMember();
    m_AllocatedMembers.push_back(result);
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
    m_AllocatedNodes.push_back(array);
    array->type = NodeType::Array;
    array->data.array.length = elements.size();
    array->data.array.values = new Node*[elements.size()];
    m_AllocatedNodeArrays.push_back(array->data.array.values);
    std::memcpy(array->data.array.values, elements.data(), elements.size() * sizeof(Node*));
    if (m_Lexer.peek() != ']')
    {
      error("]", m_Lexer.peekStr(1));
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
      error("\"", m_Lexer.peekStr(1));
    expectedClose = m_Lexer.peek(); // ' or "
    m_Lexer.skipChar();
    uint64_t length = 0;
    while (m_Lexer.peek(length) != -1 && (m_Lexer.peek(length) != expectedClose ||
                                          (m_Lexer.peek(length) == expectedClose && m_Lexer.peek(length - 1) == '\\')))
      length++;

    char* result = new char[length + 1];
    m_AllocatedCharArrays.push_back(result);
    std::strncpy(result, m_Lexer.c_str(), length);
    result[length] = '\0';
    m_Lexer.skipChars(length);
    if (m_Lexer.peek() != expectedClose)
    {
      error(std::to_string(expectedClose), m_Lexer.peekStr(1));
      while (m_Lexer.peek() != -1 && m_Lexer.peek() != '}')
        m_Lexer.skipChar();
    }
    m_Lexer.skipChar();
    Node* node = new Node();
    m_AllocatedNodes.push_back(node);
    node->type = NodeType::String;
    node->data.string.length = length;
    node->data.string.ptr = result;
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
      m_AllocatedNodes.push_back(node);
      node->type = NodeType::Integer;
      node->data.integer = res;
      return node;
    }
    double res = std::stold(integerPart + decimalPart + exponent);
    Node* result = new Node();
    m_AllocatedNodes.push_back(result);
    result->type = NodeType::Double;
    result->data.dbl = res;
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
        error("e/E", m_Lexer.peekStr(1));
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
      error("0-9", m_Lexer.peekStr(1));
      return std::string();
    }
    while (m_Lexer.peek() != -1 && Lexer::IsNumber(m_Lexer.peek()))
    {
      result += m_Lexer.peek();
      m_Lexer.skipChar();
    }
    return result;
  }

  void Parser::error(const std::string& expected, const std::string& got)
  {
    std::string res = "Unexpected character at " + std::to_string(m_Lexer.getLine()) + ":" +
                      std::to_string(m_Lexer.getColumn()) + ". Expected \"" + expected + "\" got " +
                      (got.empty() ? "blank" : got) + ".\n";
    if (m_ShouldThrow)
    {
      for (auto* node : m_AllocatedNodes)
        operator delete(node);
      for (auto* member : m_AllocatedMembers)
        operator delete(member);
      for (auto** array : m_AllocatedMemberArrays)
        delete[] array;
      for (auto** array : m_AllocatedNodeArrays)
        delete[] array;
      for (auto* array : m_AllocatedCharArrays)
        delete[] array;

      m_AllocatedCharArrays.clear();
      m_AllocatedMemberArrays.clear();
      m_AllocatedMembers.clear();
      m_AllocatedNodeArrays.clear();
      m_AllocatedNodes.clear();
      throw std::runtime_error(res);
    }
    std::cout << res;
  }

} // namespace json