#include <algorithm>
#include <as/Token.h>

Token::Token() /* I feel alright mama I'm not jokin' */
    : value(0)
    , str_value("")
    , row(0)
    , col(0)
{ }

std::string Token::norm_str() const
{
    std::string norm(str_value);
    std::transform(norm.begin(), norm.end(), norm.begin(), ::toupper);

    return norm;
}
