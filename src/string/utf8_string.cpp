#include <mzx/string/utf8_string.h>

namespace mzx
{

Utf8String::Utf8String()
{
}

Utf8String::Utf8String(const std::string &str)
    : str_(str)
{
    UpdateOffset();
}

Utf8String::Utf8String(const char *str)
    : str_(str)
{
    UpdateOffset();
}

Utf8String::Utf8String(const char *str, std::size_t len)
    : str_(str, len)
{
    UpdateOffset();
}

void Utf8String::UpdateOffset()
{
    if (str_.empty())
    {
        return;
    }
    offset_list_.clear();
    for (std::size_t i = 0; i < str_.size(); ++i)
    {
        auto c = static_cast<unsigned char>(str_[i]);
        if (c <= 0x7f || c >= 0xc0)
        {
            offset_list_.emplace_back(i);
        }
    }
}

const char *Utf8String::CharAt(std::size_t index, std::size_t *len) const
{
    if (index >= offset_list_.size())
    {
        return nullptr;
    }
    if (len)
    {
        if (index + 1 >= offset_list_.size())
        {
            *len = str_.size() - offset_list_[index];
        }
        else
        {
            *len = offset_list_[index + 1] - offset_list_[index];
        }
    }
    return str_.c_str() + offset_list_[index];
}

std::string Utf8String::Substring(std::size_t index, std::size_t len)
{
    if (len == 0 || index >= offset_list_.size())
    {
        return std::string();
    }
    if (offset_list_.size() - index <= len)
    {
        return std::string(str_.c_str() + offset_list_[index]);
    }
    return std::string(str_.c_str() + offset_list_[index], offset_list_[index + len] - offset_list_[index]);
}

Utf8String Utf8String::Utf8Substring(std::size_t index, std::size_t len)
{
    if (len == 0 || index >= offset_list_.size())
    {
        return Utf8String();
    }
    if (offset_list_.size() - index <= len)
    {
        return Utf8String(str_.c_str() + offset_list_[index]);
    }
    return Utf8String(str_.c_str() + offset_list_[index], offset_list_[index + len] - offset_list_[index]);
}

std::size_t Utf8String::Length() const
{
    return offset_list_.size();
}

const std::string &Utf8String::Data() const
{
    return str_;
}

std::string Utf8String::operator[](std::size_t index) const
{
    if (index >= offset_list_.size())
    {
        return std::string();
    }
    if (index + 1 >= offset_list_.size())
    {
        return std::string(str_.c_str() + offset_list_[index], str_.size() - offset_list_[index]);
    }
    else
    {
        return std::string(str_.c_str() + offset_list_[index], offset_list_[index + 1] - offset_list_[index]);
    }
}

Utf8String Utf8String::operator+(const std::string &str) const
{
    return Utf8String(str_ + str);
}

Utf8String Utf8String::operator+(const Utf8String &str) const
{
    return Utf8String(str_ + str.Data());
}

std::size_t Utf8String::Length(const std::string &str)
{
    std::size_t count = 0;
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        auto c = static_cast<unsigned char>(str[i]);
        if (c <= 0x7f || c >= 0xc0)
        {
            ++count;
        }
    }
    return count;
}

} // namespace mzx
