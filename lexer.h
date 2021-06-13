#pragma once

#include <cctype>
#include <string>

namespace json
{
  class Lexer
  {
  public:
    Lexer() = default;
    Lexer(const std::string& text) : m_Text(text)
    {
    }

    char peek(uint64_t forward)
    {
      if (m_Idx + forward > m_Text.size())
        return (char)-1;
      return m_Text[m_Idx + forward];
    }
    char peek()
    {
      if (m_Idx + 1 > m_Text.size())
        return (char)-1;
      return m_Text[m_Idx];
    }
    std::string peekStr(uint64_t charCount)
    {
      if (m_Idx + 1 > m_Text.size())
        return "";
      if (std::min(charCount, m_Text.size() - m_Idx - 1) < 0)
        return "";
      return m_Text.substr(m_Idx, std::min(charCount, m_Text.size() - m_Idx));
    }
    void skipChar()
    {
      m_Idx++;
      m_Column += 1;
    }
    void skipChars(uint64_t count)
    {
      m_Idx += count;
      m_Column += count;
    }
    void skipWhitespace()
    {
      while (peek() == ' ' || peek() == '\r' || peek() == '\n' || peek() == '\t')
      {
        m_Column += 1;
        if (peek() == '\r' && peek(1) == '\n') // windows line endings KEKW
        {
          m_Column = 0;
          m_Line += 1;
          skipChars(2);
        }
        else if (peek() == '\r')
        {
          m_Column = 0;
          m_Line += 1;
        }
        else if (peek() == '\n')
        {
          m_Column = 0;
          m_Line += 1;
        }
        skipChar();
      }
    }

    uint32_t getLine()
    {
      return m_Line;
    }
    uint32_t getColumn()
    {
      return m_Column;
    }

    const char* c_str() const
    {
      return m_Text.data() + m_Idx;
    }

    static bool IsNumber(char c)
    {
      return c == '-' || std::isdigit(c);
    }
    static bool IsAlphaNum(char c)
    {
      return std::isalpha(c) || std::isdigit(c);
    }

  private:
    uint32_t m_Line = 1, m_Column = 0;
    uint64_t m_Idx = 0;
    std::string m_Text;
  };
} // namespace json