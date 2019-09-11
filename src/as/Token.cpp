#include <algorithm>
#include <as/Token.h>

Token::Token() /* I feel alright mama I'm not jokin' */
    : str("")
    , value(0)
    , line(0)
    , pos(0)
{ }

std::string Token::normalized() const
{
    std::string norm(str);
    std::transform(norm.begin(), norm.end(), norm.begin(), ::toupper);

    return norm;
}
