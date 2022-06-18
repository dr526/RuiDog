#ifndef BASE64_H
#define BASE64_H

#include <iostream>
#include <string>
using namespace std;

class base64{
public:
    string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
    bool is_base64(const char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }
    string encode(const char * bytes_to_encode, unsigned int in_len);
    string decode(string const & encoded_string);
    static base64* instance();//获取单例对象的接口函数
};
#endif // BASE64_H
