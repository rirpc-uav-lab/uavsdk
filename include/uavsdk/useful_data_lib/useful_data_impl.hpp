#pragma once

#include <random>

#include <iostream>
#include <set>
#include <mutex>

#include <nlohmann/json.hpp>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{
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
    

    class UniMapStr : public useful_di::MapLikeDataStorageInterface<std::string>
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


            // std::string add_data(const std::shared_ptr<TypeInterface>& _data) 
            // {
            //     return this->_add_data(_data);
            // }


            // void add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data)
            // {
            //     this->_add_data(_key, _data);
            // }


            // virtual std::string add_data(const std::shared_ptr<TypeInterface>& _data) override
            // {

            //     std::mt19937 generator(std::random_device{}()); // Seed with real randomness
            //     std::uniform_int_distribution<int> distribution(0, RAND_MAX);

            //     std::string _key = std::to_string(distribution(generator));
            //     if (not this->data_storage.count(_key))
            //     {
            //         this->keys.push_back(_key);
            //         this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(_key), std::dynamic_pointer_cast<TypeInterface>(_data)));
            //     }
            //     else 
            //     {
            //         this->modify_data(_key, _data);
            //         std::cout << "ATTENTION!!! Your new randomly generated key already exists! Overwriting data...\n";
            //         // throw std::runtime_error("UniMapStr::_add_data(id) - _key random generation failed. Randomly generated key is the same as one of existing keys");
            //     }

            //     return _key;
            // }


            virtual void add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data) override
            {
                
                if (not this->data_storage.count(_key))
                {
                    std::string tmp_key = _key;
                    
                    this->keys.push_back(_key);
                    this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(tmp_key), std::dynamic_pointer_cast<TypeInterface>(_data)));
                }
                else 
                {
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
                if (not this->data_storage.count(data_identifier))
                {
                    std::string msg = "UniMapStr: Error! No such key in blackboard. Key = " + data_identifier + ".\n";
                }
                return this->data_storage.at(data_identifier);
            }


            void modify_data(const std::string& data_identifier, const std::shared_ptr<TypeInterface>& new_data) override
            {
                if (this->data_storage.count(data_identifier))
                {
                    this->data_storage.at(data_identifier) = new_data;
                }
                else
                {
                    throw std::runtime_error("UniMapStr: tried to modify data with key " + data_identifier + " but no such key exists");
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
                for (int i = 0; i < this->keys.size(); i++)
                {
                    if (this->keys.at(i) == search_key) return i;
                }
            }
        
        protected:
            std::vector<std::string> keys;
            // Id _add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data)
    };


    class Blackboard : public MapLikeDataStorageInterface<std::string>
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


        std::vector<std::string> get_keys_from_blackboard()
        {
            std::lock_guard<std::mutex> llock(bb_mutex);
            return blackboard->get_present_keys();
        }

        #warning No interface for the following function?
        template <typename T>
        std::shared_ptr<T> at(std::string key)
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
        }


        size_t size() override
        {
            return blackboard->size();
        }


        private:
        std::shared_ptr<useful_di::UniMapStr> blackboard; // shared_resource
        std::mutex bb_mutex; // blackboard mutex


        void init_blackboard(std::shared_ptr<useful_di::UniMapStr> init_bb)
        {
            this->blackboard = init_bb;
        }
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
        void attach_observer(std::shared_ptr<IDataObserver> observer) override 
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
        std::shared_ptr<IDataObserver> observer;
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
            std::lock_guard<std::mutex> lock(data_mx);

            if (utils::cppext::get_type<SubjectType>() != input_data->___get_type()) throw std::runtime_error("DataSubscriber::be_notified(): Provided input data type is not equal to SubjectType. SubjectType is " + utils::cppext::get_type<SubjectType>() + ", input type is " + input_data->___get_type() + ".\n");
            // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
            this->data = std::dynamic_pointer_cast<SubjectType>(input_data);
            // std::cout << "After:  " << this->data->get_data() << std::endl;
        }


        std::shared_ptr<SubjectType> get_data() override 
        { 
            std::lock_guard<std::mutex> lock(data_mx);
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



    class StrictDataObserver : public DataObserverWithCallback, public IHasExpectations<ObserverCollectorExpectations>
    {
    public:
        StrictDataObserver(std::function<void(std::shared_ptr<TypeInterface>)> callback_) : DataObserverWithCallback(callback_)
        {}

        virtual void be_notified(std::shared_ptr<TypeInterface> input_data) override
        {
            if (input_data->___get_type() != this->get_expectations().get_data_type()) throw std::runtime_error("StrictDataObserver::be_notified(): Expected " + this->get_expectations().get_data_type() + " but got " + input_data->___get_type());
            this->callback(input_data);
        }

        ObserverCollectorExpectations get_expectations() override
        {
            return this->expectations;
        }


        void set_expectations(ObserverCollectorExpectations new_expectations) override
        {
            this->expectations = new_expectations;
        }


        const bool match_expectations(const ObserverCollectorExpectations &other_expectations) override
        {
            return this->expectations == other_expectations;
        }
    };

    
    class StrictDataCollector : public IDataCollector, public IHasExpectations<ObserverCollectorExpectations>
    // class StrictDataCollector : public IDataCollector, public IHasExpectations<ObserverCollectorExpectations>
    {
    public:
        StrictDataCollector(ObserverCollectorExpectations new_expectations)
        {
            this->set_expectations(new_expectations);
        }

        ObserverCollectorExpectations get_expectations() override
        {
            return this->expectations;
        }


        void set_expectations(ObserverCollectorExpectations new_expectations) override
        {
            this->expectations = new_expectations;
        }


        const bool match_expectations(const ObserverCollectorExpectations &other_expectations) override
        {
            return this->expectations == other_expectations;
        }


        void update_data(std::shared_ptr<TypeInterface> data) override
        {
            if (data->___get_type() != this->get_expectations().get_data_type()) throw std::runtime_error("StrictDataCollector::update_data(): Expected " + this->get_expectations().get_data_type() + " but got " + data->___get_type());
            this->current_data = data;
            this->notify_observers();
        }

        void attach_observer(std::shared_ptr<IDataObserver> observer) override
        {
            auto observer_p = std::dynamic_pointer_cast<StrictDataObserver>(observer);

            if (!observer_p) throw std::runtime_error("StrictDataCollector: Tried to attach an incompatible observer. Should derive from StrictDataObserver");
            if (!this->match_expectations(observer_p->get_expectations())) throw std::runtime_error("StrictDataCollector: Tried to bind incompatible expectations. This expectations: " + this->expectations.to_str() + "Other expectations: " + observer_p->get_expectations().to_str());

            this->observers.push_back(observer);
        }

    protected:
        std::shared_ptr<TypeInterface> current_data;
        std::vector<std::shared_ptr<IDataObserver>> observers;
        
        void notify_observers() override
        {
            for (const auto &observer : observers)
            {
                observer->be_notified(current_data);
            }
        }
    };




    class StrictBindableDataObserver : public StrictDataObserver, public IBindable<std::shared_ptr<StrictDataCollector>>, public IBoundCheckAble
    {
    public:
        StrictBindableDataObserver(ObserverCollectorExpectations initial_expectations, std::function<void(std::shared_ptr<TypeInterface>)> callback_) : StrictDataObserver(callback_)
        {
            this->set_expectations(initial_expectations);
        }

        void bind(std::shared_ptr<StrictDataCollector> collector) override 
        {
            if (!this->match_expectations(collector->get_expectations())) throw std::runtime_error("StrictBindableDataObserver: Tried to bind incompatible expectations. This expectations: " + this->expectations.to_str() + "Other expectations: " + collector->get_expectations().to_str());
            std::shared_ptr<StrictBindableDataObserver> this_p(this);
            collector->attach_observer(this_p);
            this_is_bound = true;
        }


        bool is_bound() override
        {
            return this->this_is_bound;
        }
    
    private:
        bool this_is_bound{false};
    };



    template <typename Id>
    class StrictlyInitializableDataDependencyManager 
    :   public IDataDependencyManager<Id>, 
        public IInitializeAble<std::pair<
        std::map<Id, std::shared_ptr<StrictBindableDataObserver>>, 
        std::map<Id, std::shared_ptr<StrictDataCollector>>
        >>,
        public IHasStringConvertibleIdentifier<Id>
    {
    public:
        StrictlyInitializableDataDependencyManager(std::pair<std::map<Id, std::shared_ptr<StrictBindableDataObserver>>, std::map<Id, std::shared_ptr<StrictDataCollector>>> init_data_dependencies)
        {
            for (auto it = init_data_dependencies.first.begin(); it != init_data_dependencies.first.end(); it++)
            {
                if (!it.second->callback_ready()) throw std::runtime_error("StrictlyInitializableDataDependencyManager: provided initialization has an observer with unprepared callback at key " + it.first);
                if (!it.second->is_bound()) throw std::runtime_error("StrictlyInitializableDataDependencyManager: provided initialization has an unbound observer at key " + it.first);
            }
            this->initialize(init_data_dependencies);
        }
    };


    // class DataDependencyManagerStr : public useful_di::StrictlyInitializableDataDependencyManager<std::string>
    // {

    // };
};