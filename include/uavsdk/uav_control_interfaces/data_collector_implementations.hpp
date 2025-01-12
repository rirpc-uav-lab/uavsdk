#pragma once
#include <uavsdk/uav_control_interfaces/data_collector_interfaces.hpp>

namespace uav_ci /// uav_control_interfaces
{
    namespace data_ci /// data_collector_interfaces
    {
        /**
         * @tparam SubjectType тип изначального объекта данных
         * @tparam UniversalDataFormat универсальный тип данных 
         */
        template <typename SubjectType, typename UniversalDataFormat>
        class DataObject : public DataInterface<SubjectType, UniversalDataFormat>
        {
        protected:
            SubjectType msg;
            UniversalDataFormat data;

        public:
            virtual UniversalDataFormat get_data() override { return data; }
            SubjectType get_msg() override { return msg; };
        };

        /**
         * @tparam SubjectType тип изначального объекта данных
         */
        template <typename SubjectType>
        class DataObjectJson : public DataObject<SubjectType, nlohmann::json>
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
            TypedDataObject(std::string type, std::string name) : DataObjectJson<SubjectType>(name)
            { 
                this->type = type; 
            }

            // TypedDataObject& operator=(TypedDataObject other) 
            // {
            //     this->msg = other.msg;
            //     this->data = other.data;
            //     this->name = other.name; 
            //     this->type = other.type;
            //     return *this; 
            // }

            void set_type(std::string type) { this->type = type; }
            std::string get_type() { return this->type; }

            nlohmann::json get_data() override 
            { 
                this->data["name"] = this->name;
                this->data["type"] = this->type;
                return this->data; 
            }
        };


        /**
         * @tparam UniversalDataFormat универсальный тип данных 
         * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
         * @tparam ConcreteIdFactory реализация IdFactoryInterface
         * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в этом контейтере, индексируемом перечислением типа Id
         */
        template <typename UniversalDataFormat, typename Id, typename ConcreteIdFactory, typename... SubjectTypes>
        class UniTypeDataStorage : public DataStorageInterface<UniversalDataFormat, Id, ConcreteIdFactory, SubjectTypes...>
        {
        private:
            using ContainedDataImpl = std::variant<std::shared_ptr<DataInterface<SubjectTypes, UniversalDataFormat>>...>;
            std::map<Id, ContainedDataImpl> data_storage;

            Id get_id_for_data(ContainedDataImpl data) override
            {
                return this->_id_factory->get_id(data);
            }
        
        public:
            Id add_data(ContainedDataImpl data) override 
            {
                if (std::is_base_of<IdFactoryInterface<Id, UniversalDataFormat, SubjectTypes...>, ConcreteIdFactory>::value == false)
                {
                    throw std::exception("Provided ConcreteIdFactory is not an implemenatation of IdFactoryInterface.");
                } // Not a SOLID violation because I make sure that ConcreteIdFactory is an implementation of IdFactoryInterface

                Id id = this->get_id_for_data(data);
                this->data_storage.insert(std::pair<Id, ContainedDataImpl>(id, data)); 
                return id;
            }

            void remove_data(Id data_identifier) override
            {
                this->data_storage.erase(data_identifier);
            }

            ContainedDataImpl at(Id data_identifier) override
            {
                return this->data_storage.at(data_identifier);
            }


            ContainedDataImpl at(std::map<Id, ContainedDataImpl>::iterator iterator)
            {
                return this->data_storage.at(iterator);
            }



            size_t size() override
            {
                return this->data_storage->size();
            }


            std::map<Id, ContainedDataImpl>::iterator begin() { return this->data_storage.begin(); }
            std::map<Id, ContainedDataImpl>::iterator end() { return this->data_storage.end(); }


            void modify_data(Id data_identifier, ContainedDataImpl new_data) override
            {
                this->data_storage.at(data_identifier) = new_data;
            }
        };


        /**
         * @tparam UniversalDataFormat универсальный тип данных 
         * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
         * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в этом контейтере, индексируемом перечислением типа Id
         */
        template <typename Id, typename IdFactoryType, typename... SubjectTypes>
        class DataCompositeJson
        : public DataCompositeInterface
                <
                    nlohmann::json, 
                    Id,
                    IdFactoryType,
                    // DataStorageInterface
                    // <
                    //     nlohmann::json, 
                    //     Id, 
                    //     SubjectTypes...
                    // >,
                    SubjectTypes...
                >
        {
            using ContainedDataImpl = std::variant<std::shared_ptr<DataInterface<SubjectTypes, nlohmann::json>>...>;
            public:
            nlohmann::json get_data() override 
            {

                nlohmann::json json_composite;

                for (auto i = this->data_storage->begin(); i != this->data_storage->end(); i++)
                {
                    auto data_element = std::get<0>(this->data_storage->at(i->first)); 
                    json_composite[data_element.get_name()] = data_element.get_data();
                }

                return json_composite;
            }


            void add_data(ContainedDataImpl data) override 
            { 
                this->data_storage->add_data(data); 
            }


            std::shared_ptr<DataStorageInterface<nlohmann::json, Id, SubjectTypes...>> get_msg() override
            {
                return std::dynamic_pointer_cast<DataStorageInterface<nlohmann::json, Id, SubjectTypes...>>(this->data_storage);
            }

            // std::shared_ptr<DataStorageInterface<Msg, UniversalDataType, Id>> get_msg()
            // {
            //     return std::dynamic_pointer_cast<std::shared_ptr<DataStorageInterface<nlohmann::json, Id, Msg>>>(this->data_storage);
            // }
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

            void update_data(std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> data) override 
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
            std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> data;
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
            DataSubscriber(std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> input_data) { this->data = input_data; }
            // DataSubscriber() { this->data = std::make_shared<DataInterface<SubjectType, UniversalDataFormat>>(); }

            /**
             * @brief Обновляет данные по указателю this->data.
             */
            void notify(std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> input_data) override
            {
                std::lock_guard<std::mutex> lock(data_mx);
                // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
                this->data = input_data;
                // std::cout << "After:  " << this->data->get_data() << std::endl;
            }


            std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> get_data() override 
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
            std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> data; ///< Указатель на объект, который будет обновляться при изменении состояния субъекта DataCollectorInterface.
            std::mutex data_mx;
        };
    };
};