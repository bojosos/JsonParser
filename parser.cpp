#include "parser.h"
#include "lexer.h"
#include "json.h"

#include <iostream>
#include <vector>
#include <cstring>

namespace json
{
    
    Parser::Parser(const std::string& text)
    {
        m_Lexer = Lexer(text);
    }
    
    Node* Parser::parseJson()
    {
        Node* result = parseElement();
        if (m_Lexer.peek() != std::string::npos)
            error();
        
        return result;
    }
    
    Node* Parser::parseElement()
    {
        m_Lexer.skipWhitespace();
        Node* node =  parseValue();
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
        else if (next == '"')
        {
            std::string peek = m_Lexer.peekStr(5);
            if (peek.rfind("false", 0) == 0)
            {
                Node* boolean = new Node();
                boolean->type = NodeType::Boolean;
                boolean->boolean = false;
                m_Lexer.skipChars(5);
                return boolean;
            }
            
            if (peek.rfind("true", 0) == 0)
            {
                Node* boolean = new Node();
                boolean->type = NodeType::Boolean;
                boolean->boolean = true;
                m_Lexer.skipChars(4);
                return boolean;
            }
            
            if (peek.rfind("null", 0) == 0)
            {
                Node* null = new Node();
                null->type = NodeType::Null;
                m_Lexer.skipChars(3);
                return null;
            }
            
            return parseString();
        } if (Lexer::IsNumber(next))
        {
            return parseNumber();
        }
        
        return nullptr;
    }
    
    Node* Parser::parseObject()
    {
        m_Lexer.skipChar(); // {
        m_Lexer.skipWhitespace();
        Node* node = parseMembers();
        if (m_Lexer.peek() != '}')
            error();
        m_Lexer.skipChar();
        return node;
    }
    
    Node* Parser::parseMembers()
    {
        std::vector<JsonMember*> members;
        while (m_Lexer.peek() != std::string::npos && m_Lexer.peek() != '}')
        {
            m_Lexer.skipWhitespace();
            members.push_back(parseMember());
            m_Lexer.skipWhitespace();
            if (m_Lexer.peek() == ',')
                m_Lexer.skipChar();
        }
        Node* result = new Node();
        result->type = NodeType::Object;
        result->object.length = members.size();
        result->object.values = new JsonMember*[members.size()];
        std::memcpy(result->object.values, members.data(), members.size() * sizeof(Node*));
        return result;
    }
    
    JsonMember* Parser::parseMember()
    {
        m_Lexer.skipWhitespace();
        Node* name = parseString();
        if (m_Lexer.peek() != ':')
            error();
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
        while (m_Lexer.peek() != std::string::npos && m_Lexer.peek() != ']')
        {
            m_Lexer.skipWhitespace();
            elements.push_back(parseElement());
            m_Lexer.skipWhitespace();
            if (m_Lexer.peek() == ',')
                m_Lexer.skipChar();
        }
        
        Node* array = new Node();
        array->type = NodeType::Array;
        array->array.length = elements.size();
        array->array.values = new Node*[elements.size()];
        std::memcpy(array->array.values, elements.data(), elements.size() * sizeof(Node*));
        if (m_Lexer.peek() != ']')
            error();
        m_Lexer.skipChar();
        return array;
    }
    
    Node* Parser::parseString()
    {
        if (m_Lexer.peek() != '"')
            error();
        m_Lexer.skipChar();
        uint64_t length = 0;
        while (m_Lexer.peek(length) != std::string::npos && m_Lexer.peek(length) != '"')
            length++;
        
        char* result = new char[length + 1];
        std::strncpy(result, m_Lexer.c_str(), length);
        result[length] = '\0';
        m_Lexer.skipChars(length);
        if (m_Lexer.peek() != '"')
            error();
        m_Lexer.skipChar();
        Node* node = new Node();
        node->type = NodeType::String;
        node->string.length = length;
        node->string.ptr = result;
        std::cout << node->string.ptr << std::endl;
        return node;
    }
    
    Node* Parser::parseNumber()
    {
        std::string integerPart = parseInteger();
        std::string decimalPart = parseDecimal();
        std::string exponent = parseExponent();
        double res = std::stold(integerPart + decimalPart + exponent);
        Node* result = new Node();
        result->type = NodeType::Double;
        result->dbl = res;
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
        
        while (m_Lexer.peek() != std::string::npos && Lexer::IsNumber(m_Lexer.peek()))
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
        std::string result;
        while (m_Lexer.peek() != std::string::npos && Lexer::IsNumber(m_Lexer.peek()))
        {
            result += m_Lexer.peek();
            m_Lexer.skipChar();
        }
        
        return result;
    }
    
    std::string Parser::parseExponent()
    {
        if (m_Lexer.peek() != 'e' && m_Lexer.peek() != 'E')
            return std::string();
        std::string result;
        result += m_Lexer.peek(); // add e
        result += m_Lexer.peek(); // add sign
        while (m_Lexer.peek() != std::string::npos && Lexer::IsNumber(m_Lexer.peek()))
        {
            result += m_Lexer.peek();
            m_Lexer.skipChar();
        }
        return result;
    }
    
    void Parser::error()
    {
        std::cout << "Unexpected character at " << m_Lexer.getLine() << ":" << m_Lexer.getColumn() << std::endl;
    }
    
}