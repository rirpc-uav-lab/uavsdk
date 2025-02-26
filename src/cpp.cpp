#include <memory>
#include <iostream>
#include <vector>
#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>


class IAnimal : public useful_di::TypeInterface
{
    public:
    virtual void speak() = 0;
};


class Cat : public IAnimal
{
    public:
    Cat()
    {
        this->___set_type();
    }

    void speak()
    {
        std::cout << "Meow" << std::endl;
    }


    void eat()
    {
        std::cout << "Cat eats\n";
    }
};


class Dog : public IAnimal
{
    public:
    Dog()
    {
        this->___set_type();
    }
    void speak()
    {
        std::cout << "Woof" << std::endl;
    }


    void run()
    {
        std::cout << "Dog runs\n";
    }
};


class Bird  : public IAnimal
{
    public:
    Bird()
    {
        this->___set_type();
    }
    void speak()
    {
        std::cout << "Chirp" << std::endl;
    }


    void fly()
    {
        std::cout << "Bird flies\n";
    }
};




class Zoo 
{
    private:
    std::vector<std::shared_ptr<IAnimal>> animals;

    public:
    void addAnimal(std::shared_ptr<IAnimal> animal)
    {
        animals.push_back(animal);
    }

    void sounds()
    {
        for (const auto& animal : animals)
        {
            if (animal->___get_type() == utils::cppext::get_type<Cat>())
            {
                std::dynamic_pointer_cast<Cat>(animal)->eat();
            }
            if (animal->___get_type() == utils::cppext::get_type<Dog>())
            {
                std::dynamic_pointer_cast<Dog>(animal)->run();
            }
            if (animal->___get_type() == utils::cppext::get_type<Bird>())
            {
                std::dynamic_pointer_cast<Bird>(animal)->fly();
            }
            animal->speak();
        }
    }
};




int main()
{
    return 0;
}