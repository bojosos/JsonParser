#include "json.h"
#include "parser.h"
#include "utils.h"

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

  void JsonParser::PrettyPrint(Json json)
  {
    PrettyPrint(json, std::cout);
  }

  std::ostream& JsonParser::PrettyPrint(Json json, std::ostream& output)
  {
    PrettyPrintUtil(output, json);
    output << std::endl;
    return output;
  }

  std::ostream& JsonParser::PrettyPrintUtil(std::ostream& output, Json json, uint32_t indent)
  {
    switch (json->type)
    {
    case NodeType::Object:
      if (json->data.object.length == 0)
      {
        output << "{ }";
        return output;
      }
      output << "{";
      output << std::endl;
      for (uint32_t i = 0; i < json->data.object.length; i++)
      {
        for (uint32_t i = 0; i < indent; i++)
          output << " ";

        output << "\"" << json->data.object.values[i]->nameNode->data.string.ptr << "\": ";
        PrettyPrintUtil(output, json->data.object.values[i]->node, indent + 2);
        if (i != json->data.array.length - 1)
          output << ",";
        output << std::endl;
      }
      for (uint32_t i = 0; i < indent - 2; i++)
        output << " ";
      output << "}";
      break;
    case NodeType::Array:
      output << "[ ";
      for (uint32_t i = 0; i < json->data.array.length; i++)
      {
        PrettyPrintUtil(output, json->data.array.values[i], indent);
        if (i != json->data.array.length - 1)
          output << ", ";
      }
      output << " ]";
      break;
    case NodeType::Integer:
      output << json->data.integer;
      break;
    case NodeType::Double:
      output << json->data.dbl;
      break;
    case NodeType::Null:
      break;
      output << "null";
    case NodeType::Boolean:
      output << (json->data.boolean == true ? "true" : "false");
      break;
    case NodeType::String:
      output << "\"" << json->data.string.ptr << "\"";
      break;
    case NodeType::None:
      break;
    }
    return output;
  }

  void JsonParser::CompactPrint(Json json)
  {
    CompactPrint(json, std::cout);
  }

  std::ostream& JsonParser::CompactPrint(Json json, std::ostream& output)
  {
    switch (json->type)
    {
    case NodeType::Object:
      output << "{";
      for (uint32_t i = 0; i < json->data.object.length; i++)
      {
        output << "\"" << json->data.object.values[i]->nameNode->data.string.ptr << "\":";
        CompactPrint(json->data.object.values[i]->node, output);
        if (i != json->data.array.length - 1)
          output << ",";
      }
      output << "}";
      break;
    case NodeType::Array:
      output << "[";
      for (uint32_t i = 0; i < json->data.array.length; i++)
      {
        CompactPrint(json->data.array.values[i], output);
        if (i != json->data.array.length - 1)
          output << ",";
      }
      output << "]";
      break;
    case NodeType::Integer:
      output << json->data.integer;
      break;
    case NodeType::Double:
      output << json->data.dbl;
      break;
    case NodeType::Null:
      break;
      output << "null";
    case NodeType::Boolean:
      output << (json->data.boolean == true ? "true" : "false");
      break;
    case NodeType::String:
      output << "\"" << json->data.string.ptr << "\"";
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

  void Node::searchUtil(const char* key, const Node& node, std::vector<Node*>& output)
  {
    if (node.type == NodeType::Array)
      for (std::size_t i = 0; i < node.getSize(); i++)
        searchUtil(key, node[i], output);
    if (node.type == NodeType::Object)
      for (std::size_t i = 0; i < node.getSize(); i++)
      {
        if (!std::strcmp(node.data.object.values[i]->nameNode->data.string.ptr, key))
        {
          Node* obj = new Node();
          obj->type = NodeType::Object;
          obj->data.object.length = 1;
          obj->data.object.values = new JsonMember*[1];
          obj->data.object.values[0] = new JsonMember(new Node(*node.data.object.values[i]->nameNode),
                                                      new Node(*node.data.object.values[i]->node));
          output.push_back(obj);
        }
        searchUtil(key, *node.data.object.values[i]->node, output);
      }
  }

  Node::Node()
  {
    std::memset(this, 0, sizeof(Node));
  }

  Node::~Node()
  {
    switch (type)
    {
    case (NodeType::Array):
      for (uint32_t i = 0; i < data.array.length; i++)
        delete data.array.values[i];
      delete[] data.array.values;
      data.array.values = nullptr;
      break;
    case (NodeType::Object):
      for (uint32_t i = 0; i < data.object.length; i++)
        delete data.object.values[i];
      delete[] data.object.values;
      data.object.values = nullptr;
      break;
    case (NodeType::String):
      delete[] data.string.ptr;
      data.string.ptr = nullptr;
      break;
    }
    type = NodeType::None;
  }

  Node::Node(const Node& other)
  {
    type = other.type;
    switch (other.type)
    {
    case (NodeType::Array):
      data.array.length = other.data.array.length;
      data.array.values = new Node*[data.array.length];
      std::memcpy(data.array.values, other.data.array.values, data.array.length * sizeof(Node*));
      break;
    case (NodeType::Object):
      data.object.length = other.data.object.length;
      data.object.values = new JsonMember*[data.object.length];
      for (uint32_t i = 0; i < data.object.length; i++)
        data.object.values[i] = new JsonMember(new Node(*other.data.object.values[i]->nameNode),
                                               new Node(*other.data.object.values[i]->node));

      break;
    case (NodeType::Double):
      data.dbl = other.data.dbl;
      break;
    case (NodeType::Integer):
      data.integer = other.data.integer;
      break;
    case (NodeType::Boolean):
      data.boolean = other.data.boolean;
      break;
    case (NodeType::String):
      data.string.length = other.data.string.length;
      char* copy = new char[data.string.length + 1];
      std::memcpy(copy, other.data.string.ptr, data.string.length + 1); // +1 for \0
      data.string.ptr = copy;
      break;
    }
  }

  Json Node::search(const std::string& key) const
  {
    Node* array = new Node();
    array->type = NodeType::Array;
    std::vector<Node*> output;
    searchUtil(key.c_str(), *this, output);

    array->data.array.length = output.size();
    array->data.array.values = new Node*[output.size()];

    std::memcpy(array->data.array.values, output.data(), output.size() * sizeof(Node*));
    return array;
  }

  void Node::remove(const std::string& path)
  {
    auto paths = Utils::SplitString(path, "/");
    if (paths.size() == 0)
      throw std::runtime_error("Invalid args.");
    Node* c = this;
    Node* p = this;
    for (auto& path : paths)
    {
      p = c;
      c = &(*c)[path];
    }

    Node& prev = *p;
    JsonMember** copy = new JsonMember*[prev.data.object.length - 1];
    uint32_t copyIdx = 0;
    for (std::size_t i = 0; i < prev.data.object.length; i++)
    {
      if (std::strcmp(prev.data.object.values[i]->nameNode->data.string.ptr,
                      paths[paths.size() - 1].c_str())) // do not match
        copy[copyIdx++] = prev.data.object.values[i];
      else
        delete prev.data.object.values[i];
    }
    delete[] prev.data.object.values;
    prev.data.object.length = copyIdx;
    prev.data.object.values = copy;
  }

  void Node::move(const std::string& from, const std::string& to)
  {
    auto fromPaths = Utils::SplitString(from, "/");
    auto toPaths = Utils::SplitString(to, "/");

    if (to.rfind(from, 0) == 0) // if toPath starts with fromPath it will break the json
      throw std::runtime_error("This action will break the json structure.");

    if (fromPaths.size() == 0 || toPaths.size() == 0)
      throw std::runtime_error("Invalid args.");
    Node* c = this;
    Node* pFrom = this;
    Node* pTo = this;
    try
    {
      for (auto& path : fromPaths)
      {
        c = &(*c)[path];
        pFrom = c;
      }
      c = this;
      for (auto& path : toPaths)
      {
        pTo = c;
        c = &(*c)[path];
      }
      Node& prev = *c;
      if (pFrom->type != NodeType::Object || c->type != NodeType::Object)
        throw std::runtime_error("Can only move from object to object.");

      JsonMember** copy = new JsonMember*[c->data.object.length + pFrom->data.object.length];
      std::memcpy(copy, c->data.object.values, c->data.object.length * sizeof(JsonMember*));
      std::memcpy(copy + c->data.object.length, pFrom->data.object.values,
                  pFrom->data.object.length * sizeof(JsonMember*));

      delete[] c->data.object.values;
      c->data.object.values = copy;
      c->data.object.length = c->data.object.length + pFrom->data.object.length;
      delete[] pFrom->data.object.values;
      pFrom->data.object.values = nullptr;
      pFrom->data.object.length = 0;
    }
    catch (const std::exception& ex)
    {
      throw;
    }
  }

  void Node::edit(const std::string& path, const std::string& text, bool fullParse)
  {
    Node* parsedJson = nullptr;
    parsedJson = fullParse ? JsonParser::Parse(text) : JsonParser::ParsePartially(text);
    try
    {
      auto paths = Utils::SplitString(path, "/");
      if (paths.size() == 0)
        throw std::runtime_error("Invalid args.");
      Node* current = this;
      Node* prev = this;
      for (auto& path : paths)
      {
        prev = current;
        current = &(*current)[path];
      }
      for (std::size_t i = 0; i < prev->data.object.length; i++)
      {
        if (!std::strcmp(prev->data.object.values[i]->nameNode->data.string.ptr, paths[paths.size() - 1].c_str()))
        {
          delete prev->data.object.values[i]->node;
          prev->data.object.values[i]->node = parsedJson;
        }
      }
    }
    catch (const std::exception& ex)
    {
      // JsonParser::JsonFree(parsedJson);
      throw;
    }
  }

  void Node::create(const std::string& path, const std::string& key, const std::string& text, bool fullParse)
  {
    Node* parsedJson = nullptr;
    parsedJson = fullParse ? JsonParser::Parse(text) : JsonParser::ParsePartially(text);
    try
    {
      auto paths = Utils::SplitString(path, "/");
      if (paths.size() == 0)
        throw std::runtime_error("Invalid args.");
      Node* current = this;
      for (auto& path : paths)
      {
        try
        {
          current = &(*current)[path];
        }
        catch (const std::exception& ex)
        {
          JsonMember** members = new JsonMember*[current->data.object.length + 1];
          std::memcpy(members, current->data.object.values, current->data.object.length * sizeof(JsonMember*));
          Node* node = new Node();
          node->type = NodeType::Object;

          node->data.object.values = nullptr;
          node->data.object.length = 0;

          Node* pathNode = new Node();
          pathNode->type = NodeType::String;
          pathNode->data.string.length = path.size();
          char* copy = new char[path.size() + 1];
          std::memcpy(copy, path.c_str(), path.size() + 1);
          pathNode->data.string.ptr = copy;
          members[current->data.object.length] = new JsonMember();
          members[current->data.object.length]->nameNode = pathNode;
          members[current->data.object.length]->node = node;
          current->data.object.length += 1;
          delete[] current->data.object.values;
          current->data.object.values = members;
          current = &(*current)[path];
        }
      }

      JsonMember** copy = new JsonMember*[current->data.object.length + 1];
      std::memcpy(copy, current->data.object.values, current->data.object.length * sizeof(JsonMember*));
      delete[] current->data.object.values;
      current->data.object.values = copy;

      Node* nameNode = new Node();
      nameNode->type = NodeType::String;
      nameNode->data.string.length = key.size();
      char* nameCopy = new char[key.size() + 1];
      std::memcpy(nameCopy, key.c_str(), key.size() + 1);
      nameNode->data.string.ptr = nameCopy;

      current->data.object.values[current->data.object.length] = new JsonMember();
      current->data.object.values[current->data.object.length]->nameNode = nameNode;
      current->data.object.values[current->data.object.length]->node = parsedJson;
      current->data.object.length++;
    }
    catch (const std::exception& ex)
    {
      // JsonParser::JsonFree(parsedJson);
      throw;
    }
  }

  const Node& Node::operator[](std::size_t index) const
  {
    if (type != NodeType::Array || index < 0 || ((unsigned int)index) >= data.array.length)
    {
      throw std::runtime_error("Invalid element index.");
    }

    return *data.array.values[index];
  }

  Node& Node::operator[](const std::string& index)
  {
    if (type != NodeType::Object)
      throw std::runtime_error("Node is not an object.");

    for (std::size_t i = 0; i < data.object.length; ++i)
      if (!strcmp(data.object.values[i]->nameNode->data.string.ptr, index.c_str()))
        return *data.object.values[i]->node;

    throw std::runtime_error(std::string("Invalid member index (") + index + ").");
  }

  const Node& Node::operator[](const std::string& index) const
  {
    if (type != NodeType::Object)
      throw std::runtime_error("Node is not an object.");

    for (std::size_t i = 0; i < data.object.length; ++i)
      if (!strcmp(data.object.values[i]->nameNode->data.string.ptr, index.c_str()))
        return *data.object.values[i]->node;

    throw std::runtime_error("Invalid index.");
  }

  inline Node::operator const char*() const
  {
    if (type == NodeType::String)
      return data.string.ptr;
    throw std::runtime_error("Node is not a string.");
  }

  inline Node::operator int64_t() const
  {
    switch (type)
    {
    case NodeType::Integer:
      return data.integer;

    case NodeType::Double:
      return (int64_t)data.dbl;
    default:
      throw std::runtime_error("Node is not a number.");
    };
  }

  inline Node::operator bool() const
  {
    if (type == NodeType::Boolean)
      return data.boolean;
    throw std::runtime_error("Node is not a boolean.");
  }

  inline Node::operator double() const
  {
    switch (type)
    {
    case NodeType::Integer:
      return (double)data.integer;

    case NodeType::Double:
      return data.dbl;
    default:
      throw std::runtime_error("Node is not a number.");
    };
  }

  uint64_t Node::getSize() const
  {
    switch (type)
    {
    case (NodeType::Array):
      return data.array.length;
    case (NodeType::Object):
      return data.object.length;
    case (NodeType::String):
      return data.string.length;
    default:
      throw std::runtime_error("Node does not have a size.");
    }
  }
} // namespace json