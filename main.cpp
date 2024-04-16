#include <variant>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <regex> // 包含regex  cmatch
#include <charconv>
#include "print.h"

struct JSONobject;
using JSONList = std::vector<JSONobject>;
using JSONDICT = std::unordered_map<std::string, JSONobject>;
struct JSONobject

{
    std::variant<
        std::nullopt_t // null
        ,
        bool, int, double, std::string, JSONList, JSONDICT>
        inner;

    void do_print() const
    {
        print(inner);
    }
};

JSONobject parse(std::string_view json);
int main()
{
    std::string_view str3 = "-12e3";
    print(parse(str3));

    return 0;
}

JSONobject parse(std::string_view json)
{
    if (json.empty())
    {
        return JSONobject{std::nullptr_t{}};
    }
    if (0 <= json[0] && json[0] <= 9 || json[0] == '-' || json[0] == '+')
    {
        // 使用正则表达式来解析数字
        return JSONobject{int{json[0] - '0'}};
    }
    return JSONobject{std::nullptr_t{}};
}

/*
可以通过这个函数来初步判断它是什么类型的 用json[0]来判断
比如说字典{"id", 123}; 字典的开头必然是一个花括号 "{"
列表[1,2,3,4,5,10] 开头必为"[""
数字类型开头可以用from_chars来判断
string类型开头为" 或者 '
*/
template <class T> // 可以通过这个函数来初步判断它是什么类型的
std::optional<T> try_parse_num(std::string_view str)
{            // 将一个 std::string_view 类型的字符串 str 转换为指定类型 T 的数值.这个函数对于解析 JSON 字符串中的数字非常有帮助，因为它可以处理整数和浮点数。
    T value; // 这个变量用来存储转换后的数值。

    /*
    from_chars()函数 来自c17 <charconv>头文件
    提供了字符串和数值之间的转换
    std::from_chars(str.data(),  str.data() + str.size(),   value); // 字符串起始地址，字符串结束地址， 一个引用 用于存放解析后的数值
    如何检查std::from_chars函数的返回值res：

    如果转换成功：
    res.ec是一个错误码 表示转换过程中是否发生错误。std::errc()是一个无错误的错误发，res.ptr表示转换后字符串的结束位置的指针
    if res.ec == std::errc() 而且 res.ptr == str.data() + str.size() 说明字符串呗成功转换为树脂 没有剩余字符 即转换成功
    这种情况下 函数返回Value 它被包装在std::optional<T>里

    如果转换失败 // 字符串中包含无法被转换的字符：
    返回错误码 并且res.ptr将指向第一个无法转换的字符 在这种情况下返回std::nullopt 表示没有值被解析
    */

    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if (res.ec == std::errc() && res.ptr == str.data() + str.size())
    {
        return value;
    }
    return std::nullopt; // 转换失败时 不包含任何值 即返回std::nullopt
}