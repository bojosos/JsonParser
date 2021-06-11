#pragma once

#include <string>

namespace json
{

  enum class NodeType
  {
    Object,
    Array,
    Integer,
    Character,
    Double,
    Null,
    Boolean,
    String
  };
  
  struct Node;
  
  struct JsonMember
  {
    Node* nameNode;
    Node* node;
  };
  
  struct Node
  {
    Node* parent;
    
    NodeType type;
    union
    {
      bool boolean;
      int64_t integer;
      double dbl;
      
      struct
      {
        uint64_t length;
        const char* ptr;
      } string;
      
      struct
      {
        uint64_t length;
        JsonMember** values;
      } object;
      
      struct
      {
        uint64_t length;
        Node** values;
      } array;
    };
    
  };
  
  class Json
  {
  public:
    static Node* Parse(const std::string& file);
  
  };

}
