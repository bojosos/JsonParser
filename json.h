#pragma once

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace json
{

  class Node;
  using Json = Node*;

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

  struct JsonMember;

  struct Node
  {
  public:
    Node();
    ~Node();
    Node(const Node& other);

    /**
     * @brief Tries to cast the node to a boolean. Throws if type is not Boolean.
     */
    operator bool() const;

    /**
     * @brief Tries to cast the node to an integer. Throws if type is not Number(Integer, Double).
     */
    operator int64_t() const;

    /**
     * @brief Tries to cast the node to a const char*. Throws if the type is not Stirng.
     */
    operator const char*() const;

    /**
     * @brief Tries to cast the node to a double. Throws if the type is not a Number(Integer, Double).
     */
    operator double() const;

    /**
     * @brief Returns the element at the specified index inside the array. Throws if index is invalid.
     *
     * @param idx Desired index.
     * @return const Node&
     */
    const Node& operator[](std::size_t idx) const;

    /**
     * @brief Returns the element at the specified index inside the array. Throws if index is invalid.
     *
     * @param idx Desired index.
     * @return const Node&
     */
    Node& operator[](const std::string& index);

    /**
     * @brief Returns the value inside the object at the specified key. Throws if index is invalid.
     *
     * @param key Desired key.
     * @return const Node&
     */
    const Node& operator[](const std::string& key) const;

    /**
     * @brief Returns the size of array, object or string. If type is not an array, object or string throws.
     *
     * @return std::size_t
     */
    std::size_t getSize() const;

  private:
    /**
     * @brief Helper for searching the json.
     *
     * @param key Search key.
     * @param node Current node.
     * @param output Results.
     */
    static void searchUtil(const char* key, const Node& node, std::vector<Node*>& output);

  public:
    /**
     * @brief Searches for a key.
     *
     * @param key Key to look for.
     * @return Returns an array with all members with that key as objects. Returns an empty array if it fails.
     */
    Json search(const std::string& key) const;

    /**
     * @brief Edits a key.
     *
     * @param path The path of the key.
     * @param json The json to replace the current value with.
     * @param fullParse Set to false if you want the parser to try and fix the json if it cannot be normally parsed.
     */
    void edit(const std::string& path, const std::string& json, bool fullParse = true);

    /**
     * @brief Creates a new member with a key and a json. The path will be created recursively if it does not exist.
     *
     * @param path The path to the new key.
     * @param key The key of the new member.
     * @param json The json value of the new member.
     * @param fullParse Set to false if you want the parser to try and fix the json if it cannot be normally parsed.
     */
    void create(const std::string& path, const std::string& key, const std::string& json, bool fullParse = true);

    /**
     * @brief Removes a key from the json. Throws if the path is incorrect.
     *
     * @param paths A forward slash separated path.
     */
    void remove(const std::string& paths);

    /**
     * @brief Moves a all elements of a key to another. Throws if the path is incorrect.
     *
     * @param from A forward slash separated path.
     * @param to A forward slash separated path.
     */
    void move(const std::string& from, const std::string& to);

    NodeType type;
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
    } data;
  };

  struct JsonMember
  {
    JsonMember() = default;
    JsonMember(Node* name, Node* n) : node(n), nameNode(name)
    {
    }

    JsonMember(const JsonMember& other) = delete;
    JsonMember& operator=(const JsonMember& other) = delete;

    ~JsonMember()
    {
      delete nameNode;
      delete node;
      node = nullptr;
      nameNode = nullptr;
    }

    Node* nameNode = nullptr;
    Node* node = nullptr;
  };

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
     * @brief Outputs the formatted json to std::cout.
     *
     * @param json Json to print.
     */
    static void PrettyPrint(Json json);

    /**
     * @brief Outputs the formatted json to the stream.
     *
     * @param outout Output stream.
     * @param json Json to print.
     */
    static std::ostream& PrettyPrint(Json json, std::ostream& outout);

  private:
    /**
     * @brief Helper function for print a formatted json.
     *
     * @param outout Output stream.
     * @param json Json to print.
     * @param indent Left indentation.
     */
    static std::ostream& PrettyPrintUtil(std::ostream& outout, Json json, uint32_t indent = 2);

  public:
    /**
     * @brief Outputs the json in a as compact way as possible to std::cout.
     *
     * @param json Json to print.
     */
    static void CompactPrint(Json json);

    /**
     * @brief Outputs the json to the stream in a as compact way as possible.
     *
     * @param output Output stream.
     * @param json Json to print
     */
    static std::ostream& CompactPrint(Json json, std::ostream& outout);

    /**
     * @brief Destroys a json object.
     *
     * @param json
     */
    static void JsonFree(Json json);
  };

} // namespace json
