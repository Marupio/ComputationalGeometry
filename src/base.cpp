#include <gaden/base.hpp>
#include <gaden/Logger.hpp>
#include <iostream>

gaden::base::base():
    m_baseInt(0),
    m_baseStr("default"),
    m_vectorInt()
{
    std::cout << "Null constructor!" << std::endl;
    report();
}

gaden::base::base(int baseInt, std::string baseStr, std::initializer_list<int> vectorInt):
    m_baseInt(baseInt),
    m_baseStr(baseStr),
    m_vectorInt(vectorInt)
{
    std::cout << "Components constructor!" << std::endl;
    report();
}


void gaden::base::report() const {
    std::cout << "m_baseInt=[" << m_baseInt << "]" << std::endl;
    std::cout << "m_baseStr=[" << m_baseStr << "]" << std::endl;
    std::cout << "m_vectorInt.size=[" << m_vectorInt.size() << "]" << std::endl;
    std::cout << "elements=" << std::endl;
    for (int i : m_vectorInt) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    Log_Debug4("Log_Debug4 class Level");
    Log_Debug3("Log_Debug3 class Level");
    Log_Debug2("Log_Debug2 class Level");
    Log_Debug("Log_Debug class Level");
    Log_Info("Log_Info class Level");
    Log_Warn("Log_Warn class Level");
    Log_Error("Log_Error class Level");
}


int gaden::base::doSomething() {
    std::cout << "Did something" << std::endl;
    return 0;
}
