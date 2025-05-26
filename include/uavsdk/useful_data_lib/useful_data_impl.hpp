#pragma once

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
/**
 * @brief Шаблонная реализация класса Collector из паттерна Observer
 * @tparam T тип передаваемой информации
 */
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
    std::shared_ptr<T> data; /// Последнее сохраненное состояние наблюдаемого объекта
    std::vector<std::shared_ptr<IObserver<T>>> observers; /// Хранилище наблюдателей
};


/**
 * @brief Шаблонная реализация класса Observer из паттерна Observer с коллбэк функцией, вызывающейся при обновлении данных
 * @tparam T тип передаваемых данных
 */
template <typename T>
class SimpleObserverWithCallback : public IObserver<T>
{
public: 
    /**
     * @brief конструктор, принимающий коллбэк функцию
     * @param callback функция колбэка
     */ 
    SimpleObserverWithCallback(std::function<void(std::shared_ptr<T>)> callback)
    {
        this->set_callback(callback);
    }


    /**
     * @brief Функция, которая проверяет, был ли добавлен колбэк
     */
    bool callback_ready()
    {
        return this->callback_set;
    }


    /**
     * @brief Функция, которая обновляет данные наблюдателя
     * @param input_data Новые данные
     */
    virtual void be_notified(std::shared_ptr<T> input_data) override
    {
        this->callback(input_data);
    }

protected:
    std::function<void(std::shared_ptr<T>)> callback_obj; /// Объект функции колбэка
    bool callback_set{false}; /// Переменная, отслеживающая, установлена ли функция колбэка


    /**
     * @brief Функция-обертка для пользовательского колбэка, выполняющая необходимые проверки безопасности
     */
    void callback(std::shared_ptr<T> msg) 
    {
        if (!msg) 
            throw std::runtime_error("SimpleObserverWithCallback: Invalid pointer passed to callback().");

        if (callback_set)
            this->callback_obj(msg);
        else 
            throw std::runtime_error("SimpleObserverWithCallback: Callback was not set but has already been called.");
    }


    /**
     * @brief Функция, которая устанавливает колбэк
     * @param callback объект функции колбэка
     */
    void set_callback(std::function<void(std::shared_ptr<T>)> callback)
    {
        this->callback_obj = callback;
        callback_set = true;
    }
};


/**
 * @brief Класс наблюдателя с колбэком и возможностью привязываться к Collector через функцию bind
 * @tparam T тип передаваемых данных
 */
template <typename T>
class SimpleBindableObserverWithCallback : public SimpleObserverWithCallback<T>, public virtual IBindable<std::shared_ptr<ICollector<T>>>, public virtual IBoundCheckAble
{
public: 
    /**
     * @brief конструктор, принимающий коллбэк функцию
     * @param callback функция колбэка
     */ 
    SimpleBindableObserverWithCallback(std::function<void(std::shared_ptr<T>)> callback) : SimpleObserverWithCallback<T>(callback)
    { }


    /**
     * @brief Функция, которая позволяет привязываться к Collector
     * @param collector объект Collector из паттерна Observer
     */
    void bind(std::shared_ptr<ICollector<T>> collector) override 
    {
        std::shared_ptr<SimpleBindableObserverWithCallback<T>> this_p(this);
        collector->attach_observer(this_p);
        this_is_bound = true;
    }


    /**
     * @brief Функция, которая обновляет данные наблюдателя
     * @param input_data Новые данные
     */
    virtual void be_notified(std::shared_ptr<T> input_data) override
    {
        if (this->is_bound())
            this->callback(input_data);
        else
            throw std::runtime_error("SimpleObserverWithCallback: Observer has not been bound.");
    }


    /**
     * @brief Функция, позволяющая проверить, привязан ли Collector
     * @returns bool true, усли привязан, иначе false
     */
    bool is_bound() override
    {
        // std::lock_guard<std::mutex> lock(data_mx);
        return this->this_is_bound;
    }

protected:
    bool this_is_bound{false}; /// Переменная, хранящая статус привязки Collector
};



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

    std::vector<std::string> get_present_keys()
    {
        return this->keys;
    }

    virtual bool has(const std::string& _key) override
    {
        for (const auto& key : keys)
        {
            if (key == _key) return true;
        }
        return false;
    }

    virtual void add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data) override
    {
        if (!_data) 
            throw std::runtime_error("UniMapStr::add_data(): Invalid pointer _data passed to add_data()");

        if (not this->data_storage.count(_key))
        {
            std::string tmp_key = _key;
            
            this->keys.push_back(_key);
            this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(tmp_key), std::dynamic_pointer_cast<TypeInterface>(_data)));
        }
        else 
        {
            if (_data->___get_type() != this->data_storage.at(_key)->___get_type()) 
                throw std::runtime_error(std::string("UniMapStr::add_data(): Tried to provide a different type value for key: " + _key + ". Current object type is: " + this->data_storage.at("_key")->___get_type() + ". New type is: " + _data->___get_type()));
            // std::cout << "UniMapStr: Warning! Called _add_data(id, data), but this id is alredy in map. Calling modify_data(id, data). \n";
            
            // std::runtime_error("Called _add_data(id, data), but this id is alredy in map");
            this->modify_data(_key, _data);
        }
    }


    virtual void remove_data(const std::string& data_identifier) override
    {
        if (this->data_storage.count(data_identifier)) 
        {
            this->data_storage.erase(data_identifier);
            this->keys.erase(this->keys.begin() + this->_get_key_id(data_identifier));
        }
        else
        {
            throw std::runtime_error("UniMapStr: tried to remove data with key " + data_identifier + " but no such key exists");
        }
    }


    std::shared_ptr<TypeInterface> at(const std::string& data_identifier) override
    {
        // fprintf(stdout, "at line: %i\n", __LINE__);
        if (not this->data_storage.count(data_identifier))
        {
            // fprintf(stdout, "at line: %i\n", __LINE__);
            std::string msg = "UniMapStr: Error! No such key in blackboard. Key = " + data_identifier + ".\n";
            throw std::runtime_error(msg);
        }
        // fprintf(stdout, "at line: %i\n", __LINE__);
        auto type_interface = this->data_storage.at(data_identifier);
        // fprintf(stdout, "at line: %i\n", __LINE__);
        return type_interface;
    }


    void modify_data(const std::string& data_identifier, const std::shared_ptr<TypeInterface>& new_data) override
    {
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
        return this->data_storage.size();
    }


    #warning Developer warning. Next two methods should be in the interface:
    std::map<std::string, std::shared_ptr<TypeInterface>>::iterator begin() { return this->data_storage.begin(); }
    std::map<std::string, std::shared_ptr<TypeInterface>>::iterator end() { return this->data_storage.end(); }


    private:
        int _get_key_id(std::string search_key)
        {
            for (size_t i = 0; i < this->keys.size(); i++)
            {
                if (this->keys.at(i) == search_key) return i;
            }
            return -1;
        }
    
    protected:
        std::vector<std::string> keys;
        // Id _add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data)
};


class Blackboard : public MapLikeDataStorageInterface<std::string>, public IHasValueForKey<std::string>
{
    public:
    Blackboard(std::shared_ptr<useful_di::UniMapStr> _blackboard)
    {
        this->___set_type();
        init_blackboard(_blackboard);
    }


    Blackboard()
    {
        this->___set_type();
        auto _blackboard = std::make_shared<useful_di::UniMapStr>();
        init_blackboard(_blackboard);
    }


    bool has(const std::string& key) override
    {
        for (const auto& _key : this->get_keys_from_blackboard())
        {
            if (key == _key) return true;
        }
        return false;
    }


    std::vector<std::string> get_keys_from_blackboard()
    {
        std::lock_guard<std::mutex> llock(bb_mutex);
        return blackboard->get_present_keys();
    }

    #warning No interface for the following function?
    template <typename T>
    std::shared_ptr<T> at(const std::string& key)
    {
        std::shared_ptr<useful_di::TypeInterface> data = nullptr;
        try
        {
            data = this->blackboard->at(key);
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


    // std::string add_data(const std::shared_ptr<TypeInterface>& _data) override
    // {
    //     throw std::runtime_error("Sorry, but calling useful_di::Blackboard::add_data(data) is not possible. We are working to remove this option completely so please do not use it. Instead you should explicitly specify the key.\n");
    //     return "error";
    // }


    void add_data(const std::string key, const std::shared_ptr<useful_di::TypeInterface>& data) override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);

        auto keys = this->blackboard->get_present_keys();
        
        bool key_not_in_bb = true;
        
        for (const auto& bb_key : keys)
        {
            if (bb_key == key)
            {
                key_not_in_bb = false;
                break;
            }
        }
        
        if (key_not_in_bb)
        {
            blackboard->add_data(key, data);
        }
        else
        {
            auto bb_type = blackboard->at(key)->___get_type();
            auto data_type = data->___get_type();
            if (bb_type != data_type) 
                throw std::runtime_error("Blackboard: OVERWITE ERROR! Called Blackboard::add_data(key, data) on key " + key + ", but this key is alredy in blackboard with a different type. bb_type = " + bb_type + " data_type = " + data_type);
            else
            {
                blackboard->add_data(key, data);
            }
        }
    }


    void remove_data(const std::string& data_identifier) override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);
        blackboard->remove_data(data_identifier);
    }


    void modify_data(const std::string& data_identifier, const std::shared_ptr<TypeInterface>& _data) override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);
        blackboard->modify_data(data_identifier, _data);
    }


    std::shared_ptr<TypeInterface> at(const std::string& data_identifier) override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);
        std::shared_ptr<useful_di::TypeInterface> data = nullptr;
        try
        {
            data = this->blackboard->at(data_identifier);
        }
        catch(const std::out_of_range e)
        {
            std::string msg(std::string(e.what()) + std::string("\n\tKey was ") + data_identifier + std::string("\n"));
            throw std::runtime_error(msg);
        }
        return data;
    }


    size_t size() override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);
        return blackboard->size();
    }


    protected:
    std::shared_ptr<useful_di::UniMapStr> blackboard; // shared_resource
    std::mutex bb_mutex; // blackboard mutex


    void init_blackboard(std::shared_ptr<useful_di::UniMapStr> init_bb)
    {
        this->blackboard = init_bb;
    }
};


// class MyClass {
// public:
//   MyClass();
//   MyClass(MyClass &&) = default;
//   MyClass(const MyClass &) = default;
//   MyClass &operator=(MyClass &&) = default;
//   MyClass &operator=(const MyClass &) = default;
//   ~MyClass();
//
// private:
//   
// };



template <typename T>
class BlackboardVariable
{
  public:
    explicit BlackboardVariable(std::shared_ptr<Blackboard> bb, const std::string& key) : key{key}
    { 
        this->bb = bb;
        if (!this->bb->has(key)) throw std::runtime_error("BlackboardVariable[" + key + "]: initial_value was not provided but no such variable exists in blackboard.");
    }


    explicit BlackboardVariable(std::shared_ptr<Blackboard> bb, const std::string& key, const T& initial_value) : key{key}
    { 
        this->bb = bb;

        if (!this->bb->has(key))
        {
            auto data_adap = std::make_shared<uavsdk::data_adapters::cxx::MutexDefendedDataAdapter<T>>(initial_value);
            this->bb->add_data(key, data_adap);
        }
    }

    BlackboardVariable() = delete;
    BlackboardVariable(const BlackboardVariable&) = default;
    BlackboardVariable(BlackboardVariable&&) = default;
    BlackboardVariable& operator=(const BlackboardVariable&) = default;
    BlackboardVariable& operator=(BlackboardVariable&&) = default;
    ~BlackboardVariable() noexcept = default;

 
    void operator=(const T& data)
    {
        auto data_adap = std::make_shared<uavsdk::data_adapters::cxx::MutexDefendedDataAdapter<T>>(data);
        this->bb->add_data(key, data_adap);
    }


    T operator*()
    {
        this->check_variable_existance(this->key);
        T variable = this->bb->at<uavsdk::data_adapters::cxx::MutexDefendedDataAdapter<T>>(this->key)->get_data();
        return variable;
    }


  protected:
    std::shared_ptr<Blackboard> bb;
    const std::string key;


  private:
    bool check_variable_existance(const std::string& key)
    {
        // if (!this->bb->has(key)) throw std::runtime_error("BlackboardVariable[" + key + "]: doesn't exist");
        if (!this->bb->has(key)) throw std::runtime_error("BlackboardVariable[" + key + "]: variable is invalid in blackboard.");
        if (!this->bb->at(key)) throw std::runtime_error("BlackboardVariable[" + key + "]: variable is invalid in blackboard.");
        return true;
    }
};


class BulkBlackboard : public Blackboard, public IBulkAtAbleContainer<std::string, std::shared_ptr<UniMapStr>>
{
public:
    using Blackboard::at;

    virtual std::shared_ptr<useful_di::TypeInterface> at(const std::string& data_identifier) override
    {
        if (not this->data_storage.count(data_identifier))
        {
            std::string msg = "UniMapStr: Error! No such key in blackboard. Key = " + data_identifier + ".\n";
            throw std::runtime_error(msg);
        }
        auto type_interface = this->data_storage.at(data_identifier);
        return type_interface;
    }


    virtual std::shared_ptr<UniMapStr> at(const std::vector<std::string>& data_identifiers) override
    {
        std::lock_guard<std::mutex> llock(bb_mutex);

        std::shared_ptr<UniMapStr> ret;

        for (const auto& data_identifier : data_identifiers)
        {
            std::shared_ptr<useful_di::TypeInterface> data = nullptr;
            try
            {
                data = this->blackboard->at(data_identifier);
                ret->add_data(data_identifier, data);
            }
            catch(const std::out_of_range e)
            {
                std::string msg(std::string(e.what()) + std::string("\n\tKey was ") + data_identifier + std::string("\n"));
                throw std::runtime_error(msg);
            }
        }

        this->last_returned = ret;
        return ret;
    }


    virtual std::shared_ptr<UniMapStr> get_all() override
    {
        auto data_identifiers = this->get_keys_from_blackboard();
        
        std::lock_guard<std::mutex> llock(bb_mutex);
        // fprintf(stdout, "BulkBlackboard %i\n", ++i);
        std::shared_ptr<UniMapStr> ret = std::make_shared<UniMapStr>();
        // fprintf(stdout, "BulkBlackboard %i\n", ++i);

        for (const auto& data_identifier : data_identifiers)
        {
            // fprintf(stdout, "BulkBlackboard %i, %i", ++i, i);
            std::shared_ptr<useful_di::TypeInterface> data = nullptr;
            try
            {
                data = this->blackboard->at(data_identifier);
                ret->add_data(data_identifier, data);
            }
            catch(const std::out_of_range e)
            {
                std::string msg(std::string(e.what()) + std::string("\n\tKey was ") + data_identifier + std::string("\n"));
                throw std::runtime_error(msg);
            }
        }

        this->last_returned = ret;
        return ret;
    }

private:
    std::shared_ptr<UniMapStr> last_returned;
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



class DataObserverWithCallback : public IDataObserver
{
public:
    DataObserverWithCallback(std::function<void(std::shared_ptr<TypeInterface>)> callback)
    {
        this->set_callback(callback);
    }


    virtual void be_notified(std::shared_ptr<TypeInterface> input_data) override
    {
        this->callback(input_data);
    }


    bool callback_ready()
    {
        return this->callback_set;
    }

protected:
    void callback(std::shared_ptr<TypeInterface> msg) 
    {
        if (callback_set)
            this->callback_obj(msg);
        else 
            throw std::runtime_error("DataObserverWithCallback: Callback was not set but has already been called.");
    }

private:
    std::function<void(std::shared_ptr<TypeInterface>)> callback_obj;
    bool callback_set{false};

    
    void set_callback(std::function<void(std::shared_ptr<TypeInterface>)> callback)
    {
        this->callback_obj = callback;
        callback_set = true;
    }
};



class StrictDataObserver : public DataObserverWithCallback, public IHasExpectations<ObserverCollectorExpectations>
{
public:
    StrictDataObserver(std::function<void(std::shared_ptr<TypeInterface>)> callback_) : DataObserverWithCallback(callback_)
    {}

    virtual void be_notified(std::shared_ptr<TypeInterface> input_data) override
    {
        // std::lock_guard<std::mutex> lock(data_mx);
        if (input_data->___get_type() != this->get_expectations().get_data_type()) throw std::runtime_error("StrictDataObserver::be_notified(): Expected " + this->get_expectations().get_data_type() + " but got " + input_data->___get_type());
        this->callback(input_data);
    }

    ObserverCollectorExpectations get_expectations() override
    {
        return this->expectations;
    }


    bool match_expectations(const ObserverCollectorExpectations &other_expectations) override
    {
        return this->expectations == other_expectations;
    }


protected:
    // std::mutex data_mx;
    void set_expectations(ObserverCollectorExpectations new_expectations) override
    {
        this->expectations = new_expectations;
    }
};


class StrictDataCollector : public IDataCollector, public IHasExpectations<ObserverCollectorExpectations>
// class StrictDataCollector : public IDataCollector, public IHasExpectations<ObserverCollectorExpectations>
{
public:
    StrictDataCollector(const ObserverCollectorExpectations new_expectations)
    {
        this->set_expectations(new_expectations);
    }


    StrictDataCollector(const std::shared_ptr<const ObserverCollectorExpectations> new_expectations)
    {
        this->set_expectations(*new_expectations);
    }


    ObserverCollectorExpectations get_expectations() override
    {
        return this->expectations;
    }


    bool match_expectations(const ObserverCollectorExpectations &other_expectations) override
    {
        return this->expectations == other_expectations;
    }


    void update_data(std::shared_ptr<TypeInterface> data) override
    {
        std::lock_guard<std::mutex> lock(data_mx);
        if (data->___get_type() != this->get_expectations().get_data_type()) throw std::runtime_error("StrictDataCollector::update_data(): Expected " + this->get_expectations().get_data_type() + " but got " + data->___get_type());
        this->current_data = data;
        this->notify_observers();
    }

    void attach_observer(std::shared_ptr<IObserver<TypeInterface>> observer) override
    {
        std::lock_guard<std::mutex> lock(data_mx);
        
        if (!observer) {
            throw std::runtime_error("StrictDataCollector: Tried to attach a null observer.");
        }

        auto observer_p = std::dynamic_pointer_cast<StrictDataObserver>(observer);

        if (!observer_p) throw std::runtime_error("StrictDataCollector: Tried to attach an incompatible observer. Should derive from StrictDataObserver");
        if (!this->match_expectations(observer_p->get_expectations())) throw std::runtime_error("StrictDataCollector: Tried to bind incompatible expectations. This expectations: " + this->expectations.to_str() + "Other expectations: " + observer_p->get_expectations().to_str());

        this->observers.push_back(observer);
    }

protected:
    std::mutex data_mx;
    std::shared_ptr<TypeInterface> current_data;
    std::vector<std::shared_ptr<IObserver<TypeInterface>>> observers;
    
    void notify_observers() override
    {
        // std::lock_guard<std::mutex> lock(data_mx);
        for (const auto &observer : observers)
        {
            observer->be_notified(current_data);
        }
    }


    void set_expectations(ObserverCollectorExpectations new_expectations) override
    {
        this->expectations = new_expectations;
    }
};




class StrictBindableDataObserver : public StrictDataObserver, public IBindable<std::shared_ptr<StrictDataCollector>>, public IBoundCheckAble
{
public:
    StrictBindableDataObserver(ObserverCollectorExpectations initial_expectations, std::function<void(std::shared_ptr<TypeInterface>)> callback_) : StrictDataObserver(callback_)
    {
        this->set_expectations(initial_expectations);
    }


    StrictBindableDataObserver(const std::shared_ptr<const ObserverCollectorExpectations> initial_expectations, std::function<void(std::shared_ptr<TypeInterface>)> callback_) : StrictDataObserver(callback_)
    {
        this->set_expectations(*initial_expectations);
    }

    void bind(std::shared_ptr<StrictDataCollector> collector) override 
    {
        // std::lock_guard<std::mutex> lock(data_mx);
        if (!this->match_expectations(collector->get_expectations())) throw std::runtime_error("StrictBindableDataObserver: Tried to bind incompatible expectations. This expectations: " + this->expectations.to_str() + "Other expectations: " + collector->get_expectations().to_str());
        std::shared_ptr<StrictBindableDataObserver> this_p(this);
        collector->attach_observer(this_p);
        this_is_bound = true;
    }


    bool is_bound() override
    {
        // std::lock_guard<std::mutex> lock(data_mx);
        return this->this_is_bound;
    }

protected:
    bool this_is_bound{false};
};



// template <typename Id>
// class StrictlyInitializableDataDependencyManager 
// :   public IDataDependencyManager<Id>, 
//     public IInitializeAble<std::pair<
//     std::map<Id, std::shared_ptr<StrictBindableDataObserver>>, 
//     std::map<Id, std::shared_ptr<StrictDataCollector>>
//     >>,
//     public IHasStringConvertibleIdentifier<Id>
// {
// public:
//     StrictlyInitializableDataDependencyManager(std::pair<std::map<Id, std::shared_ptr<StrictBindableDataObserver>>, std::map<Id, std::shared_ptr<StrictDataCollector>>> init_data_dependencies)
//     {
//         for (auto it = init_data_dependencies.first.begin(); it != init_data_dependencies.first.end(); it++)
//         {
//             if (!it.second->callback_ready()) throw std::runtime_error("StrictlyInitializableDataDependencyManager: provided initialization has an observer with unprepared callback at key " + it.first);
//             if (!it.second->is_bound()) throw std::runtime_error("StrictlyInitializableDataDependencyManager: provided initialization has an unbound observer at key " + it.first);
//         }
//         this->initialize(init_data_dependencies);
//     }
// };


// class DataDependencyManagerStr : public useful_di::StrictlyInitializableDataDependencyManager<std::string>
// {

// };
};
