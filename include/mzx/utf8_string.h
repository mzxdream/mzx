#ifndef __MZX_UTF8_STRING_H__
#define __MZX_UTF8_STRING_H__

#include <string>
#include <vector>

namespace mzx
{

class Utf8String
{
public:
    Utf8String();
    Utf8String(const std::string &str);
    Utf8String(const char *str);
    Utf8String(const char *str, std::size_t len);

public:
    const char *CharAt(std::size_t index, std::size_t *len = nullptr) const;
    std::string Substring(std::size_t index, std::size_t len = static_cast<std::size_t>(-1));
    Utf8String Utf8Substring(std::size_t index, std::size_t len = static_cast<std::size_t>(-1));
    std::size_t Length() const;
    const std::string &Data() const;
    std::string operator[](std::size_t index) const;
    Utf8String operator+(const std::string &str) const;
    Utf8String operator+(const Utf8String &str) const;

    static std::size_t Length(const std::string &str);

private:
    void UpdateOffset();

private:
    std::string str_;
    std::vector<std::size_t> offset_list_;
};

} // namespace mzx

#endif
