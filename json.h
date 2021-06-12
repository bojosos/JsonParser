#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace json
{

  enum class NodeType
  {
    None,
    Object,
    Array,
    Integer,
    Double,
    Null,
    Boolean,
    String
  };

  class Node;

  struct JsonMember
  {
    Node* nameNode;
    Node* node;
  };

  class Node
  {
  public:
    Node();

    operator bool() const;
    operator int64_t() const;
    operator const char*() const;
    operator double() const;
    const Node& operator[](std::size_t idx) const;
    const Node& operator[](const char* key) const;
    uint64_t getSize() const;
    NodeType getType() const
    {
      return m_Type;
    }

  private:
    friend class JsonParser;
    friend class Parser;
    NodeType m_Type;
    union {
      bool boolean;
      std::int64_t integer;
      double dbl;

      struct
      {
        uint64_t length;
        const char* ptr;
      } string;

      struct
      {
        std::size_t length;
        JsonMember** values;
      } object;

      struct
      {
        std::size_t length;
        Node** values;
      } array;
    } m_Data;
  };

  using Json = Node*;

  class JsonParser
  {
  public:
    /**
     * @brief Parses a json file. Throws an exception if an the format is
     * incorrect.
     *
     * @param text The text version of the json.
     * @return Json
     */
    static Json Parse(const std::string& text);

    /**
     * @brief Parses a json file. If an error occurs tries to parse as much as it
     * can.
     *
     * @param text The text version of the json.
     * @return Json
     */
    static Json ParsePartially(const std::string& text);

    /**
     * @brief Outputs the formatted json to the stream.
     *
     * @param outout Output stream.
     * @param json Json to print.
     * @param indent Left indentation.
     */
    static std::ostream& PrettyPrint(std::ostream& outout, Json json, uint32_t indent = 2);

    /**
     * @brief Outputs the json in a as compact way as possible.
     *
     * @param output Output stream.
     * @param json Json to print
     */
    static std::ostream& CompactPrint(std::ostream& output, Json json);

    static void JsonFree(Json json);
  };

} // namespace json
