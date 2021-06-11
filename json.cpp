#include "json.h"
#include "parser.h"

namespace json
{
    
    Node* Json::Parse(const std::string& text)
    {
        return Parser(text).parseJson();
    }
    
}