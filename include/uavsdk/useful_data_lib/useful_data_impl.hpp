#pragma once

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
        
        virtual nlohmann::json get_data() 
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
                this->type = utils::cppext::get_type<SubjectType>(); 
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

            nlohmann::json get_data() override 
            { 
                this->data["name"] = this->name;
                this->data["type"] = this->type;
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
    template <typename Id, typename ConcreteIdFactory>
    class RegistryDataStorage : public DataStorageInterface<Id, ConcreteIdFactory>
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



        std::map<Id, std::shared_ptr<TypeInterface>>::iterator begin() { return this->data_storage.begin(); }
        std::map<Id, std::shared_ptr<TypeInterface>>::iterator end() { return this->data_storage.end(); }


    protected:
        std::map<Id, std::shared_ptr<TypeInterface>> data_storage;

        Id _get_id_for_data(const std::shared_ptr<TypeInterface>& data) override
        {
            return this->_id_factory->get_id(data);
        }


        virtual Id _add_data(const std::shared_ptr<TypeInterface>& data) override 
        {
            Id id = this->_get_id_for_data(data);
            if (not this->data_storage.count(id))
            {
                this->data_storage.insert(std::pair<Id, std::shared_ptr<TypeInterface>>(id, data)); 
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


        std::shared_ptr<TypeInterface> _at(const Id& data_identifier) override
        {
            return this->data_storage.at(data_identifier);
        }


        std::shared_ptr<TypeInterface> _at(const std::map<Id, std::shared_ptr<TypeInterface>>::iterator iterator)
        {
            return this->data_storage.at(iterator);
        }


        void _modify_data(const Id& data_identifier, const std::shared_ptr<TypeInterface>& new_data) override
        {
            this->data_storage.at(data_identifier) = new_data;
        }


        virtual size_t _size() override
        {
            return this->data_storage.size();
        }
    };


    template <typename Id, typename ConcreteIdFactory>
    class DataCompositeJson : public useful_di::DataCompositeInterface<nlohmann::json, Id, ConcreteIdFactory, RegistryDataStorage<Id, ConcreteIdFactory>>
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


        protected:
        nlohmann::json _get_data() override
        {
            nlohmann::json new_data;
            for (typename std::map<Id, std::shared_ptr<useful_di::TypeInterface>>::iterator iter = std::dynamic_pointer_cast<RegistryDataStorage<Id, ConcreteIdFactory>>(this->msg)->begin(); iter != std::dynamic_pointer_cast<RegistryDataStorage<Id, ConcreteIdFactory>>(this->msg)->end(); iter++)
            {
                auto type = iter->second->___get_type();
                // if (type == utils::cppext::get_type<ABC>())
                // {
                auto data = std::dynamic_pointer_cast<DataObjectJson<int>>(iter->second);
                new_data[iter->first] = data->get_data();
                // }
                // else if (type == utils::cppext::get_type<BCA>())
                // {
                //     auto data = std::dynamic_pointer_cast<BCA>(iter->second);
                //     new_data["BCA"] = data->get_data();
                // }
                // else 
                // {
                //     throw std::runtime_error("Unknown type");
                // }
            }

            this->data = new_data;
            return this->data;
        }
    };



    /**
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam SubjectType тип изначального объекта данных
     */
    template <typename SubjectType, typename UniversalDataFormat>
    class SingleObserverDataCollector : public DataCollectorInterface<SubjectType, UniversalDataFormat>
    {
    public:
        void attach_observer(std::shared_ptr<DataObserverInterface<SubjectType, UniversalDataFormat>> observer) override 
        {
            this->observer = observer;
        }

        void update_data(std::shared_ptr<useful_di::TypeInterface> data) override 
        {

            // std::cout << "DataSubscriber::update_data: " << data->get_data() << std::endl;
            this->data = data;
            this->notify_observers();
        }


        void notify_observers() override 
        {
            if (this->observer != nullptr)
            {
                this->observer->notify(this->data);
            }
        }

    private:
        std::shared_ptr<useful_di::TypeInterface> data;
        std::shared_ptr<DataObserverInterface<SubjectType, UniversalDataFormat>> observer;
    };


    /**
     * @tparam SubjectType тип изначального объекта данных
     * @tparam UniversalDataFormat универсальный тип данных 
     */
    template <typename SubjectType, typename UniversalDataFormat>
    class DataSubscriber : public DataObserverInterface<SubjectType, UniversalDataFormat>
    {
    public:
        /**
         * @brief Конструктор объекта DataSubscriber.
         */
        DataSubscriber(std::shared_ptr<useful_di::TypeInterface> input_data) { this->data = input_data; }
        // DataSubscriber() { this->data = std::make_shared<DataInterface<SubjectType, UniversalDataFormat>>(); }

        /**
         * @brief Обновляет данные по указателю this->data.
         */
        void notify(std::shared_ptr<useful_di::TypeInterface> input_data) override
        {
            std::lock_guard<std::mutex> lock(data_mx);
            // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
            this->data = input_data;
            // std::cout << "After:  " << this->data->get_data() << std::endl;
        }


        std::shared_ptr<useful_di::TypeInterface> get_data() override 
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
        std::shared_ptr<useful_di::TypeInterface> data; ///< Указатель на объект, который будет обновляться при изменении состояния субъекта DataCollectorInterface.
        std::mutex data_mx;
    };
};