#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>

class ABC : public useful_di::DataInterface<int, std::string>
{
    public:
    ABC(int a)
    {
        this->___set_type();
        this->msg = a;
        this->data = std::to_string(a);
    }

    void ___set_type() override
    {
        this->___type = utils::cppext::get_type<ABC>();
    }
};


class BCA : public useful_di::DataInterface<float, std::string>
{
    public:
    BCA(float a)
    {
        this->___set_type();
        this->msg = a;
        this->data = std::to_string(a);
    }

    void ___set_type() override
    {
        this->___type = utils::cppext::get_type<BCA>();
    }
};


enum TypeId
{
    _ABC = 0,
    _BCA = 1
};


class IdFactory : public useful_di::IdFactoryInterface<TypeId>
{
    public:
    TypeId get_id(std::shared_ptr<useful_di::TypeInterface> data) override
    {
        if (data->___get_type() == utils::cppext::get_type<ABC>())
        {
            return TypeId::_ABC;
        }
        else if (data->___get_type() == utils::cppext::get_type<BCA>())
        {
            return TypeId::_BCA;
        }
        else
        {
            throw std::runtime_error("Unknown type");
        }
    }
};


class DataStorageExample : public useful_di::RegistryDataStorage<TypeId, IdFactory>
{

};


class DataCompositeExample : public useful_di::DataCompositeInterface<nlohmann::json, TypeId, IdFactory, DataStorageExample>
{
    void ___set_type() override
    {
        this->___type = utils::cppext::get_type<DataCompositeExample>();
    }


    public: 
    void add_data(std::shared_ptr<TypeInterface> data) override
    {
        this->msg->add_data(data);
    }


    protected:
    nlohmann::json _get_data() override
    {
        nlohmann::json new_data;
        for (std::map<TypeId, std::shared_ptr<useful_di::TypeInterface>>::iterator iter = std::dynamic_pointer_cast<DataStorageExample>(this->msg)->begin(); iter != std::dynamic_pointer_cast<DataStorageExample>(this->msg)->end(); iter++)
        {
            auto type = iter->second->___get_type();
            if (type == utils::cppext::get_type<ABC>())
            {
                auto data = std::dynamic_pointer_cast<ABC>(iter->second);
                new_data["ABC"] = data->get_data();
            }
            else if (type == utils::cppext::get_type<BCA>())
            {
                auto data = std::dynamic_pointer_cast<BCA>(iter->second);
                new_data["BCA"] = data->get_data();
            }
            else 
            {
                throw std::runtime_error("Unknown type");
            }
        }

        this->data = new_data;
        return this->data;
    }
};


int main()
{

    DataCompositeExample composite;
    auto abc = std::make_shared<ABC>(1);
    auto bca = std::make_shared<BCA>(2.5);
    auto cab = std::make_shared<BCA>(3.5);

    // storage.add_data(abc);
    // storage.add_data(bca);

    composite.add_data(abc);
    composite.add_data(bca);
    composite.add_data(cab);

    // std::shared_ptr<DataStorageExample> storage = std::dynamic_pointer_cast<DataStorageExample>(composite.get_msg());

    nlohmann::json data = composite.get_data();

    std::cout << data << std::endl;

    // TypeId id;
    

    // int i = 0;
    // for (std::map<TypeId, std::shared_ptr<useful_di::TypeInterface>>::iterator iter = storage->begin(); iter != storage->end(); iter++)
    // {
    //     std::cout << ++i << "'st  iteration.\n";
    //     auto type = iter->second->___get_type();
    //     if (type == utils::cppext::get_type<ABC>())
    //     {
    //         auto data = std::dynamic_pointer_cast<ABC>(iter->second);
    //         std::cout << data->get_data() << std::endl;
    //     }
    //     else if (type == utils::cppext::get_type<BCA>())
    //     {
    //         auto data = std::dynamic_pointer_cast<BCA>(iter->second);
    //         std::cout << data->get_data() << std::endl;
    //     }
    //     else 
    //     {
    //         throw std::runtime_error("Unknown type");
    //     }
    //     std::cout << "\n";
    // }
    
    // id = TypeId::_ABC;

    // std::cout << std::dynamic_pointer_cast<ABC>(storage->at(id))->get_msg() << std::endl;
    // id = TypeId::_BCA;

    // std::cout << std::dynamic_pointer_cast<BCA>(storage->at(id))->get_msg() << std::endl;

    return 0;
}