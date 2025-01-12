#pragma once

namespace utils
{
    namespace cppext
    {
        #include <string>
        template <typename T>
        std::string get_type()
        {
            return typeid(T).name();
        }
    };
};