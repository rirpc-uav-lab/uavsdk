#pragma once

#include "rclcpp/rclcpp.hpp"

#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/useful_data_lib/base_interfaces.hpp"
#include <iostream>
#include <mutex>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{
template <typename T>
class SimpleCollector : public virtual ICollector<T>
{
public:
    /**
     * @brief метод для изменения состояния субъекта
     */
    void update_data(std::shared_ptr<T> data) override
    {
        if (!data) throw std::runtime_error("SimpleCollector: tried to update data with an invalid pointer.");
        this->data = data;
        this->notify_observers();
    }

    /**
     * @brief метод, который добавляет наблюдателя к набору наблюдателей, которые хотят получать уведомления о изменении состояния субъекта
     */
    void attach_observer(std::shared_ptr<IObserver<T>> observer) override
    {
        if (!observer) throw std::runtime_error("SimpleCollector: tried to attach an observer wich is invalid.");
        this->observers.push_back(observer);
    }

protected:
    /**
     * @brief метод, который уведомляет всех наблюдателей о изменении состояния субъекта, и передает им текущее состояние
     */
    void notify_observers() override
    {
        for (const auto& observer : observers)
        {
            observer->be_notified(data);
        }
    }

private:
    std::shared_ptr<T> data;
    std::vector<std::shared_ptr<IObserver<T>>> observers;
};


// template <typename T>
// class SimpleObserverWithCallback : public IObserver<T>
// {
// public: 
//     SimpleObserverWithCallback(std::function<void(std::shared_ptr<T>)> callback)
//     {
//         this->set_callback(callback);
//     }


//     bool callback_ready()
//     {
//         return this->callback_set;
//     }


//     virtual void be_notified(std::shared_ptr<T> input_data) override
//     {
//         this->callback(input_data);
//     }

// protected:
//     std::function<void(std::shared_ptr<T>)> callback_obj;
//     bool callback_set{false};


//     void callback(std::shared_ptr<T> msg) 
//     {
//         if (!msg) 
//             throw std::runtime_error("SimpleObserverWithCallback: Invalid pointer passed to callback().");

//         if (callback_set)
//             this->callback_obj(msg);
//         else 
//             throw std::runtime_error("SimpleObserverWithCallback: Callback was not set but has already been called.");
//     }


//     void set_callback(std::function<void(std::shared_ptr<T>)> callback)
//     {
//         this->callback_obj = callback;
//         callback_set = true;
//     }
// };


// template <typename T>
// class SimpleBindableObserverWithCallback : public SimpleObserverWithCallback<T>, public virtual IBindable<std::shared_ptr<ICollector<T>>>, public virtual IBoundCheckAble
// {
// public: 
//     SimpleBindableObserverWithCallback(std::function<void(std::shared_ptr<T>)> callback) : SimpleObserverWithCallback<T>(callback)
//     { }


//     void bind(std::shared_ptr<ICollector<T>> collector) override 
//     {
//         std::shared_ptr<SimpleBindableObserverWithCallback<T>> this_p(this);
//         collector->attach_observer(this_p);
//         this_is_bound = true;
//     }


//     virtual void be_notified(std::shared_ptr<T> input_data) override
//     {
//         if (this->is_bound())
//             this->callback(input_data);
//         else
//             throw std::runtime_error("SimpleObserverWithCallback: Observer has not been bound.");
//     }


//     bool is_bound() override
//     {
//         // std::lock_guard<std::mutex> lock(data_mx);
//         return this->this_is_bound;
//     }

// protected:
//     bool this_is_bound{false};
// };



/**
 * @tparam SubjectType тип изначального объекта данных
 */
template <typename SubjectType>
class DataObjectJson : public useful_di::DataInterface<SubjectType, nlohmann::json>
{
    private:
    std::string name;

    public: 
    DataObjectJson(std::string name) { set_name(name); }
    
    void set_name(std::string name) { this->name = name; }
    
    std::string get_name() { return this->name; }
    
    virtual nlohmann::json get_data() override 
    { 
        this->data["name"] = this->name;
        return this->data; 
    }
};


/**
 * @tparam SubjectType тип изначального объекта данных
 */
template <typename SubjectType>
class TypedDataObject : public DataObjectJson<SubjectType>
{
private:
    std::string type;

public:
    TypedDataObject(std::string name) : DataObjectJson<SubjectType>(name)
    { 
        this->type = utils::cppext::get_type<SubjectType>(); // Так и должно быть! ___type нужен, чтобы можно было определять тип указателя, а type нужен, чтобы в json засунуть информацию об объекте-оригинале
    }

    // TypedDataObject& operator=(TypedDataObject other) 
    // {
    //     this->msg = other.msg;
    //     this->data = other.data;
    //     this->name = other.name; 
    //     this->type = other.type;
    //     return *this; 
    // }

    std::string get_type() { return this->type; }

    virtual nlohmann::json get_data() override 
    { 
        this->data["name"] = this->get_name();
        this->data["type"] = this->get_type();
        return this->data; 
    }
};


/**
 * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum.
 * @tparam ConcreteIdFactory реализация IdFactoryInterface
 * 
 * @brief Контейнер для заранее определенного набора данных. Может
 * хранить столько объектов, сколько определено ключей в Id. При повторении
 * ввода данных по уже существующему ключу, перезаписывает данные.
 */
template <typename Id, typename ConcreteIdFactory, typename UniversalDataFormat>
class RegistryDataStorage : public DataStorageInterface<Id>
{
public:
    RegistryDataStorage()
    {
        if (std::is_base_of<IdFactoryInterface<Id>, ConcreteIdFactory>::value == false)
        {
            throw std::runtime_error("Provided ConcreteIdFactory is not an implemenatation of IdFactoryInterface.");
        } // Not a SOLID violation because I make sure that ConcreteIdFactory is an implementation of IdFactoryInterface
        this->_id_factory = std::make_shared<ConcreteIdFactory>();
    }


    virtual Id add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& data) override 
    {
        Id id = this->_get_id_for_data(data);
        if (not this->data_storage.count(id))
        {
            this->data_storage.insert(std::pair<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>(id, data)); 
        }
        else 
        {
            this->data_storage.at(id) = data;
        }
        return id;
    }


    virtual void remove_data(const Id& data_identifier) override
    {
        this->data_storage.erase(data_identifier);
    }


    std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> at(const Id& data_identifier) override
    {
        return this->data_storage.at(data_identifier);
    }


    std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> at(const typename std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator iterator)
    {
        return this->data_storage.at(iterator);
    }


    void modify_data(const Id& data_identifier, const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& new_data) override
    {
        this->data_storage.at(data_identifier) = new_data;
    }


    virtual size_t size() override
    {
        return this->data_storage.size();
    }

    #warning Developer warning. Next two methods should be in the interface:
    typename std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator begin() { return this->data_storage.begin(); }
    typename std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator end() { return this->data_storage.end(); }

protected:
    std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>> data_storage;
    std::shared_ptr<IdFactoryInterface<Id>> _id_factory;

    Id _get_id_for_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& data)
    {
        return this->_id_factory->get_id(data);
    }
};


class UniMapStr : public useful_di::MapLikeDataStorageInterface<std::string>, public useful_di::IHasValueForKey<std::string>
{
public:
    UniMapStr()
    {
        this->___set_type();
    }

    virtual std::vector<std::string> get_present_keys() override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        return this->keys;
    }

    virtual bool has(const std::string& _key) override
    {
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "BEGIN FUNCTION HAS ========================== : " << _key);
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        for (const auto& key : this->keys)
        {
            // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "FUNCTION HAS = : " << key);
            if (key == _key) return true;
        }
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "FINISH FUNCTION HAS ========================== : ");
        return false;
    }

    virtual void add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data) override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);

        if (_key.empty()) {
            throw std::runtime_error("UniMapStr: empty key provided");
        }
        if (!_data) {
            throw std::runtime_error("UniMapStr: null data provided");
        }

        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "UNIMAP ADDDDDDDDDDDD DATAAAAAAAAAAA " << _key);
        if (!_data) 
            throw std::runtime_error("UniMapStr::add_data(): Invalid pointer _data passed to add_data()");

        if (not this->data_storage.count(_key))
        {
            std::string tmp_key = _key;
            
            this->keys.push_back(_key);
            this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(tmp_key), std::dynamic_pointer_cast<TypeInterface>(_data)));
            // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), " UNIMAP ADDDDDDDDDDDD IN data_storage " << _key);
        }
        else 
        {
            if (_data->___get_type() != this->data_storage.at(_key)->___get_type()) 
                throw std::runtime_error(std::string("UniMapStr::add_data(): Tried to provide a different type value for key: " + _key + ". Current object type is: " + this->data_storage.at("_key")->___get_type() + ". New type is: " + _data->___get_type()));
            // std::cout << "UniMapStr: Warning! Called _add_data(id, data), but this id is alredy in map. Calling modify_data(id, data). \n";
            
            // std::runtime_error("Called _add_data(id, data), but this id is alredy in map");
            // this->modify_data(_key, _data);
            if (this->data_storage.count(_key))
            {
                if (_data->___get_type() != this->data_storage.at(_key)->___get_type()) throw std::runtime_error("UniMapStr::modify_data(): Tried to change type of the object at key: " + _key + ". Current object type is " + this->data_storage.at(_key)->___get_type() + ". New object type is " + _data->___get_type());
                this->data_storage.at(_key) = _data;
            }
            else
            {
                throw std::runtime_error("UniMapStr::modify_data(): tried to modify data with key " + _key + " but no such key exists");
            }
        }
    }


    virtual void remove_data(const std::string& data_identifier) override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        if (this->data_storage.count(data_identifier)) 
        {
            this->data_storage.erase(data_identifier);
            auto it = std::find(keys.begin(), keys.end(), data_identifier);
            if (it != keys.end()) 
            {
                keys.erase(it);
            }
        }
        else
        {
            throw std::runtime_error("UniMapStr: tried to remove data with key " + data_identifier + " but no such key exists");
        }
    }


    #warning No interface for the following function?
    template <typename T>
    std::shared_ptr<T> at(const std::string& key)
    {
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "UniMapStr AT<>!!!!!! DATAAAAAAAAAAA " << key);
        std::shared_ptr<useful_di::TypeInterface> data = nullptr;
        try
        {
            data = this->at(key);
        }
        catch(const std::out_of_range e)
        {
            std::string msg(std::string(e.what()) + std::string("\n\tKey was ") + key + std::string("\n"));
            throw std::runtime_error(msg);
        }
        
        
        // std::cout << "bb at key = " << key << "\n";

        if (data->___get_type() == utils::cppext::get_type<T>())
        {
            return std::dynamic_pointer_cast<T>(data);
        }
        else
        {
            std::string msg = "CommandInterfaceWithBlackboard::at<T>(std::string key) error: tried conversion of data withe key " + key + " from type " + data->___get_type() + " to " + utils::cppext::get_type<T>() + "\n";
            throw std::runtime_error(msg);
        }
    }


    virtual std::shared_ptr<TypeInterface> at(const std::string& data_identifier) override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "UniMapStr AT!!!!!! DATAAAAAAAAAAA " << data_identifier);

        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), " 1111111111111111111111 data_identifier = " << data_identifier);
        if (data_storage.empty())
        {
            std::string msg = "data_storage is empty!!!! + \n";
            throw std::runtime_error(msg);
        }
        else
        {
            for (const auto& key : data_storage)
            {
                // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "key in data_storage: " << key.first);
            }
        }
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), " 22222222222222222222222222222222222");

        bool flag = false;
        for (const auto& key : this->keys)
        {
            // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "FUNCTION HAS = : " << key);
            if (key == data_identifier) flag = true;
        }

        if (!flag)
        {
            std::string msg = "UniMapStr: Error! No such key in blackboard. Key = " + data_identifier + ".\n";
            throw std::runtime_error(msg);
        }
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), " ЧТО-ТО ПОПЫТАЛСЯ ВЕРНУТЬ");
        auto type_interface = this->data_storage.at(data_identifier);
        // if (type_interface == nullptr)
        // {
        //     RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), " NUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUL");

        // }
        return type_interface;
    }


    virtual void modify_data(const std::string& data_identifier, const std::shared_ptr<TypeInterface>& new_data) override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        if (this->data_storage.count(data_identifier))
        {
            if (new_data->___get_type() != this->data_storage.at(data_identifier)->___get_type()) throw std::runtime_error("UniMapStr::modify_data(): Tried to change type of the object at key: " + data_identifier + ". Current object type is " + this->data_storage.at(data_identifier)->___get_type() + ". New object type is " + new_data->___get_type());
            this->data_storage.at(data_identifier) = new_data;
        }
        else
        {
            throw std::runtime_error("UniMapStr::modify_data(): tried to modify data with key " + data_identifier + " but no such key exists");
        }
    }


    virtual size_t size() override
    {
        std::lock_guard<std::mutex> llock(blackboard_mutex);
        return this->data_storage.size();
    }


    #warning Developer warning. Next two methods should be in the interface:
    std::map<std::string, std::shared_ptr<TypeInterface>>::iterator begin() { return this->data_storage.begin(); }
    std::map<std::string, std::shared_ptr<TypeInterface>>::iterator end() { return this->data_storage.end(); }


    private:
        int _get_key_id(std::string search_key)
        {
            // RCLCPP_INFO_STREAM(rclcpp::get_logger("uni_map_str"), "GET KEY GET KEY GET KEY GET KEY GEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEET");
            for (size_t i = 0; i < this->keys.size(); i++)
            {
                if (this->keys.at(i) == search_key) return i;
            }
            return -1;
        }
    
    protected:
        std::vector<std::string> keys;
        std::mutex blackboard_mutex;
        // Id _add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data)
};


/**
 * @tparam UniversalDataFormat универсальный тип данных 
 * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
 * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
 */
template <typename UniversalDataFormat, typename Id, typename ThisStorageType>
class ConvertableDataComposite : public DataInterface<std::shared_ptr<ThisStorageType>, UniversalDataFormat>, public IDataComposite<Id, ThisStorageType>
{
protected:
    // std::shared_ptr<DataStorageInterface<Msg, UniversalDataFormat, Id>> data_storage;

public:
    ConvertableDataComposite()
    {
        this->msg = std::make_shared<ThisStorageType>();
    }
    // virtual void add_data(std::shared_ptr<TypeInterface> data) = 0;
};



// template <typename UniversalDataFormat>
// class DescriptedUniMap : public DataStorageInterface<std::string, UniversalDataFormat>


class DataCompositeJson : public useful_di::ConvertableDataComposite<nlohmann::json, std::string, UniMapStr> //, useful_di::IAppendAbleContainer<std::string>
{
public: 

    void add_data(std::string key, std::shared_ptr<TypeInterface> data)
    {
        std::dynamic_pointer_cast<UniMapStr>(this->msg)->add_data(key, data);
    }


    nlohmann::json get_data() override
    {
        // int counter = 0;
        nlohmann::json new_data;

        for (typename std::map<std::string, std::shared_ptr<TypeInterface>>::iterator iter = std::dynamic_pointer_cast<UniMapStr>(this->msg)->begin(); iter != std::dynamic_pointer_cast<UniMapStr>(this->msg)->end(); iter++)
        {
            auto type = iter->second->___get_type();
            // if (type == utils::cppext::get_type<ABC>())
            // {
            auto data = std::dynamic_pointer_cast<useful_di::UniversalDataInterface<nlohmann::json>>(iter->second);
            // std::cout << "DataCompositeJson::_get_data(): " << data << "\n";
            if (data)
            {
                nlohmann::json json_data = data->get_data();

                // new_data[static_cast<int>(iter->first)] = data->get_data();
                // new_data[json_data["name"]] = json_data;
                new_data[json_data["name"].get<std::string>()] = json_data;
            }
            else
            {
                std::cerr << "Warning! Shared pointer is nullptr" << "\n";
            }
        }

        this->data = new_data;
        return this->data;
    }
};



class DataCompositeJsonMap : public useful_di::ConvertableDataComposite<nlohmann::json, std::string, UniMapStr>
{
    // void ___set_type() override
    // {
    //     this->___type = utils::cppext::get_type<DataCompositeJsonMap>();
    // }


    public: 
    // void add_data(std::shared_ptr<TypeInterface> data) override
    // {
    //     this->msg->add_data(data);
    // }


    void add_data(std::string key, std::shared_ptr<TypeInterface> data)
    {
        std::dynamic_pointer_cast<UniMapStr>(this->msg)->add_data(key, data);
    }


    std::vector<std::string> get_storage_keys()
    {
        return std::dynamic_pointer_cast<UniMapStr>(this->msg)->get_present_keys();
    }


    nlohmann::json get_data() override
    {
        // int counter = 0;
        nlohmann::json new_data;

        std::vector<std::string> keys = std::dynamic_pointer_cast<UniMapStr>(this->msg)->get_present_keys();

        for (const std::string& key : keys)
        {
            auto data = std::dynamic_pointer_cast<useful_di::UniversalDataInterface<nlohmann::json>>(this->msg->at(key));

            if (data)
            {
                nlohmann::json json_data = data->get_data();

                new_data[key] = json_data;
            }
            else
            {
                std::cerr << "Warning! Shared pointer is nullptr" << "\n";
            }
        }

        this->data = new_data;
        return this->data;
    }
};




template <typename SubjectType>
class SingleObserverDataCollector : public IDataCollector
{
public:
    static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "SingleObserverDataCollector: provided SubjectType is not derived from TypeInterface");
    void attach_observer(std::shared_ptr<useful_di::IObserver<useful_di::TypeInterface>> observer) override 
    {
        this->observer = observer;
    }

    void update_data(std::shared_ptr<TypeInterface> data) override 
    {
        // std::cout << "DataSubscriber::update_data: " << data->get_data() << std::endl;
        if (utils::cppext::get_type<SubjectType>() != data->___get_type()) throw std::runtime_error("SingleObserverDataCollector::update_data(): Provided input data type is not equal to SubjectType. SubjectType is " + utils::cppext::get_type<SubjectType>() + ", input type is " + data->___get_type() + ".\n");
        this->data = std::dynamic_pointer_cast<SubjectType>(data);
        this->notify_observers();
    }


    void notify_observers() override 
    {
        if (this->observer != nullptr)
        {
            this->observer->be_notified(this->data);
        }
    }

private:
    std::shared_ptr<SubjectType> data;
    std::shared_ptr<useful_di::IObserver<useful_di::TypeInterface>> observer;
};


// template <typename SubjectType>
// class MultiObserverDataCollector : public IDataCollector
// {
// public:
//     static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "MultiObserverDataCollector: provided SubjectType is not derived from TypeInterface");
//     size_t attach_observer(std::shared_ptr<IDataObserver> observer) override 
//     {
//         // this->observer = observer;
//         this->observers.push_back(observer);
//         auto id = this->observers.size() - 1;
//         return id;
//     }


//     void remove_observer(size_t id)
//     {
//         if (id < this->observers.size())
//         {
//             this->observers.erase(this->observers.begin() + id);
//         }
//         else 
//         {
//             throw std::runtime_error("MultiObserverDataCollector: Id " + std::to_string(id) + " is not present in MultiObserverDataCollector::observers.");
//         }
//         // std::vector<std::string> keys = this->observer_map.get_present_keys();

//         // bool id_present = false;

//         // for (const auto& key : keys)
//         // {
//         //     if (id == key)
//         //     {
//         //         id_present = true;
//         //         break;
//         //     }
//         // }

//         // if (id_present)
//         // {
//         //     this->observer_map.remove_data(id);
//         // }
//         // else
//         // {
//         //     throw std::runtime_error("MultiObserverDataCollector: Id " + id + " is not present in MultiObserverDataCollector::observers_map.");
//         // }
//     }


//     void update_data(std::shared_ptr<SubjectType> data) override 
//     {

//         // std::cout << "DataSubscriber::update_data: " << data->get_data() << std::endl;
//         this->data = data;
//         this->notify_observers();
//     }


//     void notify_observers() override 
//     {
//         for (const auto &observer : observers)
//         {
//             if (observer != nullptr)
//             {
//                 observer->be_notified(this->data);
//             }
//             else
//             {
//                 throw std::runtime_error("MultiObserverDataCollector::notify_observers(): observer != nullptr has failed.");
//             }
//         }
//     }

// private:
//     std::shared_ptr<SubjectType> data;
//     // std::vector<std::shared_ptr<IDataObserver<SubjectType>>> observer;
//     std::vector<std::shared_ptr<IDataObserver> observers;
// };


template <typename SubjectType>
class RetrievableDataObserver : public IDataObserver, public IRetrievableDataContainer<std::shared_ptr<SubjectType>>
{
    static_assert(std::is_base_of<TypeInterface, SubjectType>(), "RetrievableDataObserver: provided SubjectType is not derived from TypeInterface.");
};



template <typename SubjectType>
class DataSubscriber : public RetrievableDataObserver<SubjectType>
{
public:
    static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "DataSubscriber: provided SubjectType is not derived from TypeInterface");
    /**
     * @brief Конструктор объекта DataSubscriber.
     */
    DataSubscriber(std::shared_ptr<SubjectType> input_data) { this->data = input_data; }
    // DataSubscriber() { this->data = std::make_shared<DataInterface<SubjectType, UniversalDataFormat>>(); }

    

    /**
     * @brief Обновляет данные по указателю this->data.
     */
    void be_notified(std::shared_ptr<TypeInterface> input_data) override
    {
        // std::lock_guard<std::mutex> lock(data_mx);

        if (utils::cppext::get_type<SubjectType>() != input_data->___get_type()) throw std::runtime_error("DataSubscriber::be_notified(): Provided input data type is not equal to SubjectType. SubjectType is " + utils::cppext::get_type<SubjectType>() + ", input type is " + input_data->___get_type() + ".\n");
        // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
        this->data = std::dynamic_pointer_cast<SubjectType>(input_data);
        // std::cout << "After:  " << this->data->get_data() << std::endl;
    }


    std::shared_ptr<SubjectType> get_data() override 
    { 
        // std::lock_guard<std::mutex> lock(data_mx);
        return this->data; 
    }


    // std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> get_data() 
    // { 
    //     return this->data;
    //     // return std::make_shared<DataInterface<SubjectType, UniversalDataFormat>>();
    // }


protected:
    std::shared_ptr<SubjectType> data; ///< Указатель на объект, который будет обновляться при изменении состояния субъекта IDataCollector.
    std::mutex data_mx;
};


class ObserverCollectorExpectations
{
public:
    bool operator==(const ObserverCollectorExpectations& other) const
    {
        bool expectations_match = true;
        
        if (this->get_data_type() != other.get_data_type()) expectations_match = false;

        return expectations_match;
    }


    void set_data_type(std::string new_data_type)
    {
        this->data_type = new_data_type;
        this->data_type_set = true;
    }


    std::string get_data_type() const
    {
        if (!data_type_set) throw std::runtime_error("ObserverCollectorExpectations: data_type was not set.");
        return this->data_type;
    }


    std::string to_str() const
    {
        std::string out;
        out = "\ndata_type = " + data_type + "\n" +
        "data_type_set = " + std::to_string(data_type_set) + "\n";

        return out;
    }

private:
    std::string data_type;
    bool data_type_set{false};
};

};
