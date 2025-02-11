#pragma once
#include <any>
#include <map>
#include <string>
#include <iostream>
#include <memory>


using namespace std;

template <typename T>
std::string get_type()
{
    return typeid(T).name();
}

class Element
{
    public:
    void doSomething()
    {
        std::cout << "default action"<< std::endl;
    }
};

class Robot:Element
{

};

class BlackBoard
{
private:
    map<string, std::any> bb_;
public:

    template<typename Elem>
    void addElement()
    {
        string name = get_type<Elem>();
        auto it = bb_.find(name);
        if (it == bb_.end())
        bb_[name] = std::shared_ptr<Elem>();
    }

    BlackBoard(
        
        /* args */);
    ~BlackBoard();
};