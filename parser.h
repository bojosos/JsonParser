#pragma once 

#include "lexer.h"

#include <string>

namespace json
{

    class Json;
    class Node;
    class JsonMember;
    
    class Parser
    {
    public:
        Parser(const std::string& text);
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
        Lexer m_Lexer;
    };

}