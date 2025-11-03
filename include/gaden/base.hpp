#pragma once
#include <string>
#include <vector>
#include <initializer_list>

namespace gaden
{

class base {
    int m_baseInt;
    std::string m_baseStr;
    std::vector<int> m_vectorInt;


public:
    base();
    base(int baseInt, std::string baseStr, std::initializer_list<int> vectorInt);

    int doSomething();

    void report() const;
};

} // namespace gaden
