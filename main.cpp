#include "json.h"

#include <fstream>

int main ()
{
    std::ifstream stream("test.json");
    std::string str((std::istreambuf_iterator<char>(stream)),
                 std::istreambuf_iterator<char>());

    json::Node* result = json::Json::Parse(str);
}