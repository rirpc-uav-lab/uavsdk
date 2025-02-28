#pragma once

#include <random>

#include <iostream>
#include <set>

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
        
        virtual nlohmann::json _get_data() override 
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

            virtual nlohmann::json _get_data() override 
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



        #warning Developer warning. Next two methods should be in the interface:
        std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator begin() { return this->data_storage.begin(); }
        std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator end() { return this->data_storage.end(); }

    protected:
        std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>> data_storage;
        std::shared_ptr<IdFactoryInterface<Id>> _id_factory;

        Id _get_id_for_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& data)
        {
            return this->_id_factory->get_id(data);
        }


        virtual Id _add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& data) override 
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


        virtual void _remove_data(const Id& data_identifier) override
        {
            this->data_storage.erase(data_identifier);
        }


        std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> _at(const Id& data_identifier) override
        {
            return this->data_storage.at(data_identifier);
        }


        std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> _at(const std::map<Id, std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>>::iterator iterator)
        {
            return this->data_storage.at(iterator);
        }


        void _modify_data(const Id& data_identifier, const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& new_data) override
        {
            this->data_storage.at(data_identifier) = new_data;
        }


        virtual size_t _size() override
        {
            return this->data_storage.size();
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


            std::string add_data(const std::shared_ptr<TypeInterface>& _data) 
            {
                this->_add_data(_data);
            }


            void add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data)
            {
                this->_add_data(_key, _data);
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
            virtual std::string _add_data(const std::shared_ptr<TypeInterface>& _data) override
            {

                std::mt19937 generator(std::random_device{}()); // Seed with real randomness
                std::uniform_int_distribution<int> distribution(0, RAND_MAX);

                std::string _key = std::to_string(distribution(generator));
                if (not this->data_storage.count(_key))
                {
                    this->keys.push_back(_key);
                    this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(_key), _data));
                }
                else 
                {
                    this->modify_data(_key, _data);
                    std::cout << "ATTENTION!!! Your new randomly generated key already exists! Overwriting data...\n";
                    // throw std::runtime_error("UniMapStr::_add_data(id) - _key random generation failed. Randomly generated key is the same as one of existing keys");
                }

                return _key;
            }


            virtual void _add_data(const std::string _key, const std::shared_ptr<TypeInterface>& _data) override
            {
                
                if (not this->data_storage.count(_key))
                {
                    std::string tmp_key = _key;
                    
                    this->keys.push_back(_key);

                    this->data_storage.insert(std::make_pair<std::string, std::shared_ptr<TypeInterface>>(std::move(tmp_key), _data));
                }
                else 
                {
                    // std::cout << "UniMapStr: Warning! Called _add_data(id, data), but this id is alredy in map. Calling modify_data(id, data). \n";
                    
                    // std::runtime_error("Called _add_data(id, data), but this id is alredy in map");
                    this->modify_data(_key, _data);
                }
            }

            virtual void _remove_data(const std::string& data_identifier) override
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


            std::shared_ptr<TypeInterface> _at(const std::string& data_identifier) override
            {
                return this->data_storage.at(data_identifier);
            }



            void _modify_data(const std::string& data_identifier, const std::shared_ptr<TypeInterface>& new_data) override
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


            virtual size_t _size() override
            {
                return this->data_storage.size();
            }
    };


    // template <typename UniversalDataFormat>
    // class DescriptedUniMap : public DataStorageInterface<std::string, UniversalDataFormat>


    class DataCompositeJson : public useful_di::DataCompositeInterface<nlohmann::json, std::string, UniMapStr>
    {
        void ___set_type() override
        {
            this->___type = utils::cppext::get_type<DataCompositeJson>();
        }


        public: 
        void add_data(std::shared_ptr<TypeInterface> data) override
        {
            this->msg->add_data(data);
        }


        void add_data(std::string key, std::shared_ptr<TypeInterface> data)
        {
            std::dynamic_pointer_cast<UniMapStr>(this->msg)->add_data(key, data);
        }


        protected:
        nlohmann::json _get_data() override
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
                    new_data[json_data["name"]] = json_data;
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


    
    class DataCompositeJsonMap : public useful_di::DataCompositeInterface<nlohmann::json, std::string, UniMapStr>
    {
        void ___set_type() override
        {
            this->___type = utils::cppext::get_type<DataCompositeJsonMap>();
        }


        public: 
        void add_data(std::shared_ptr<TypeInterface> data) override
        {
            this->msg->add_data(data);
        }


        void add_data(std::string key, std::shared_ptr<TypeInterface> data)
        {
            std::dynamic_pointer_cast<UniMapStr>(this->msg)->add_data(key, data);
        }


        std::vector<std::string> get_storage_keys()
        {
            return std::dynamic_pointer_cast<UniMapStr>(this->msg)->get_present_keys();
        }


        protected:
        nlohmann::json _get_data() override
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
    class SingleObserverDataCollector : public DataCollectorInterface<SubjectType>
    {
    public:
        static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "SingleObserverDataCollector: provided SubjectType is not derived from TypeInterface");
        void attach_observer(std::shared_ptr<DataObserverInterface<SubjectType>> observer) override 
        {
            this->observer = observer;
        }

        void update_data(std::shared_ptr<SubjectType> data) override 
        {

            // std::cout << "DataSubscriber::update_data: " << data->get_data() << std::endl;
            this->data = data;
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
        std::shared_ptr<DataObserverInterface<SubjectType>> observer;
    };


    template <typename SubjectType>
    class MultiObserverDataCollector : public DataCollectorInterface<SubjectType>
    {
    public:
        static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "MultiObserverDataCollector: provided SubjectType is not derived from TypeInterface");
        std::string attach_observer(std::shared_ptr<DataObserverInterface<SubjectType>> observer) override 
        {
            // this->observer = observer;
            auto id = this->observer_map.add_data(observer);
            return id;
        }


        void remove_observer(std::string id)
        {
            std::vector<std::string> keys = this->observer_map.get_present_keys();

            bool id_present = false;

            for (const auto& key : keys)
            {
                if (id == key)
                {
                    id_present = true;
                    break;
                }
            }

            if (id_present)
            {
                this->observer_map.remove_data(id);
            }
            else
            {
                throw std::runtime_error("MultiObserverDataCollector: Id " + id + " is not present in MultiObserverDataCollector::observers_map.");
            }
        }


        void update_data(std::shared_ptr<SubjectType> data) override 
        {

            // std::cout << "DataSubscriber::update_data: " << data->get_data() << std::endl;
            this->data = data;
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
        // std::vector<std::shared_ptr<DataObserverInterface<SubjectType>>> observer;
        useful_di::UniMapStr observer_map;
    };



    template <typename SubjectType>
    class DataSubscriber : public DataObserverInterface<SubjectType>
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
        void be_notified(std::shared_ptr<SubjectType> input_data) override
        {
            std::lock_guard<std::mutex> lock(data_mx);
            // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
            this->data = input_data;
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
        std::shared_ptr<SubjectType> data; ///< Указатель на объект, который будет обновляться при изменении состояния субъекта DataCollectorInterface.
        std::mutex data_mx;
    };
};