#include <functional>
#include <iostream>
#include <memory>

class Abc
{
public:
    Abc(std::function<int(int const&, int const&)> f) 
    {
        two_num_func = f;
    }
    
    int call()
    {
        return two_num_func(num1, num2);
    }


    void set_f(std::function<int(int const&, int const&)> f)
    {
        two_num_func = f;
    }

private:
    int num1 = 3;
    int num2 = 2;
    std::function<int(int const&, int const&)> two_num_func;
};


class WhateverCaller
{
public:
    WhateverCaller(std::function<int()> f)
    {
        whatever = f;
    }

    std::function<int()> whatever;
};


int main()
{
    int multiplexor = 3;
    std::function<int(int, int)> sum_plus_multiplex = 
    [multiplexor](int a, int b) 
    {
        return multiplexor * (a + b);
    };

    std::function<int(int, int)> sum = 
    [](int a, int b) 
    {
        return a + b;
    };

    std::function<int(int, int)> difference = 
    [](int a, int b) 
    {
        return a - b;
    };

    Abc obj(sum_plus_multiplex);
    std::cout << obj.call() << "\n";

    obj.set_f(sum);
    std::cout << obj.call() << "\n";

    obj.set_f(difference);
    std::cout << obj.call() << "\n";

    WhateverCaller caller(std::bind(&Abc::call, &obj));

    std::cout << caller.whatever() << "\n";

    return 0;
}