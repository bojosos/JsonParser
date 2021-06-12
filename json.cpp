#include "json.h"
#include "parser.h"

#include <iomanip>
#include <iostream>

namespace json
{

  Json JsonParser::Parse(const std::string& text)
  {
    return Parser(text).parseJson();
  }

  Json JsonParser::ParsePartially(const std::string& text)
  {
    return Parser(text, false).parseJson();
  }

  std::ostream& JsonParser::PrettyPrint(std::ostream& output, Json json, uint32_t indent)
  {
    switch (json->m_Type)
    {
    case NodeType::Object:
      output << "{" << std::endl;
      for (uint32_t i = 0; i < json->m_Data.object.length; i++)
      {
        for (uint32_t i = 0; i < indent; i++)
          output << "  ";
        output << "\"" << json->m_Data.object.values[i]->nameNode->m_Data.string.ptr << "\": ";
        PrettyPrint(output, json->m_Data.object.values[i]->node, indent + 2);
        if (i != json->m_Data.array.length - 1)
          output << ",";
        output << std::endl;
      }
      output << "}" << std::endl;
      break;
    case NodeType::Array:
      output << "[ ";
      for (uint32_t i = 0; i < json->m_Data.array.length; i++)
      {
        PrettyPrint(output, json->m_Data.array.values[i]);
        if (i != json->m_Data.array.length - 1)
          output << ", " << std::endl;
      }
      output << " ]" << std::endl;
      break;
    case NodeType::Integer:
      output << json->m_Data.integer;
      break;
    case NodeType::Double:
      output << json->m_Data.dbl;
      break;
    case NodeType::Null:
      break;
      output << "null";
    case NodeType::Boolean:
      output << (json->m_Data.boolean == true ? "true" : "false");
      break;
    case NodeType::String:
      output << "\"" << json->m_Data.string.ptr << "\"";
      break;
    case NodeType::None:
      break;
    }
    return output;
  }

  std::ostream& JsonParser::CompactPrint(std::ostream& output, Json json)
  {
    switch (json->m_Type)
    {
    case NodeType::Object:
      output << "{";
      for (uint32_t i = 0; i < json->m_Data.object.length; i++)
      {
        output << "\"" << json->m_Data.object.values[i]->nameNode->m_Data.string.ptr << "\":";
        CompactPrint(output, json->m_Data.object.values[i]->node);
        if (i != json->m_Data.array.length - 1)
          output << ",";
      }
      output << "}";
      break;
    case NodeType::Array:
      output << "[";
      for (uint32_t i = 0; i < json->m_Data.array.length; i++)
      {
        CompactPrint(output, json->m_Data.array.values[i]);
        if (i != json->m_Data.array.length - 1)
          output << ",";
      }
      output << "]";
      break;
    case NodeType::Integer:
      output << json->m_Data.integer;
      break;
    case NodeType::Double:
      output << json->m_Data.dbl;
      break;
    case NodeType::Null:
      break;
      output << "null";
    case NodeType::Boolean:
      output << (json->m_Data.boolean == true ? "true" : "false");
      break;
    case NodeType::String:
      output << "\"" << json->m_Data.string.ptr << "\"";
      break;
    case NodeType::None:
      break;
    }
    return output;
  }

  void JsonParser::JsonFree(Json json)
  {
    delete json;
  }

  Node::Node()
  {
    std::memset(this, 0, sizeof(Node));
  }

  const Node& Node::operator[](std::size_t index) const
  {
    if (m_Type != NodeType::Array || index < 0 || ((unsigned int)index) >= m_Data.array.length)
    {
      throw std::runtime_error("Invalid array index.");
    }

    return *m_Data.array.values[index];
  }

  const Node& Node::operator[](const char* index) const
  {
    if (m_Type != NodeType::Object)
      throw std::runtime_error("Node is not an object.");

    for (std::size_t i = 0; i < m_Data.object.length; ++i)
      if (!strcmp(m_Data.object.values[i]->nameNode->m_Data.string.ptr, index))
        return *m_Data.object.values[i]->node;

    throw std::runtime_error("Invalid index.");
  }

  inline Node::operator const char*() const
  {
    if (m_Type == NodeType::String)
      return m_Data.string.ptr;
    throw std::runtime_error("Node is not a string.");
  }

  inline Node::operator int64_t() const
  {
    switch (m_Type)
    {
    case NodeType::Integer:
      return m_Data.integer;

    case NodeType::Double:
      return (int64_t)m_Data.dbl;
    default:
      throw std::runtime_error("Node is not a number.");
    };
  }

  inline Node::operator bool() const
  {
    if (m_Type == NodeType::Boolean)
      return m_Data.boolean;
    throw std::runtime_error("Node is not a boolean.");
  }

  inline Node::operator double() const
  {
    switch (m_Type)
    {
    case NodeType::Integer:
      return (double)m_Data.integer;

    case NodeType::Double:
      return m_Data.dbl;
    default:
      throw std::runtime_error("Node is not a number.");
    };
  }

  uint64_t Node::getSize() const
  {
    switch (m_Type)
    {
    case (NodeType::Array):
      return m_Data.array.length;
    case (NodeType::Object):
      return m_Data.object.length;
    case (NodeType::String):
      return m_Data.string.length;
    default:
      throw std::runtime_error("Node does not have a size.");
    }
  }
} // namespace json