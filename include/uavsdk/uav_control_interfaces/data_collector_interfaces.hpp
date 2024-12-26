#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>

#include <nlohmann/json.hpp>


namespace uav_ci /// uav_control_interfaces
{
    namespace data_ci /// data_collector_interfaces
    {
        // template <typename Msg> class DataCollectorInterface;
        // template <typename Msg> class DataObserverInterface;
        // template <typename Msg> class DataSubscriber : public DataObserverInterface<Msg>;

        // template <typename Msg> class DataInterface;
        // template <typename Msg> class DataObject : public DataInterface<Msg>;
        // template <typename Msg> class DataComposite : public DataInterface<Msg>;


        template <typename Msg, typename UniversalDataFormat>
        class DataInterface
        {
        public:
            virtual UniversalDataFormat get_data() = 0;
            virtual Msg get_msg() = 0;
        };


        template <typename Msg, typename UniversalDataFormat>
        class DataObject : public DataInterface<Msg, UniversalDataFormat>
        {
        protected:
            Msg msg;
            UniversalDataFormat data;

        public:

            DataObject& operator=(DataObject<Msg, UniversalDataFormat> other)
            {
                this->msg = other.msg;
                this->data = other.data;
                return *this;
            }

            virtual UniversalDataFormat get_data() override { return data; }
            Msg get_msg() override { return msg; };
        };


        template <typename Msg>
        class DataObjectJson : public DataObject<Msg, nlohmann::json>
        {
            private:
            std::string name;

            public: 
            DataObjectJson(std::string name) { set_name(name); }

            // DataObjectJson& operator=(DataObjectJson other) 
            // { 
            //     this->msg = other.msg;
            //     this->data = other.data;
            //     this->name = other.name; 
            //     return *this; 
            // }
            
            void set_name(std::string name) { this->name = name; }
            
            std::string get_name() { return this->name; }
            
            virtual nlohmann::json get_data() 
            { 
                this->data["name"] = this->name;
                return this->data; 
            }
        };


        template <typename Msg>
        class TypedDataObject : public DataObjectJson<Msg>
        {
        private:
            std::string type;

        public:
            TypedDataObject(std::string type, std::string name) : DataObjectJson<Msg>(name)
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
                this->data["type"] = this->type;
                return this->data; 
            }
        };


        template <typename Msg, typename UniversalDataFormat, typename Id>
        class DataStorageInterface
        {
        public:
            virtual void add_data(DataInterface<Msg, UniversalDataFormat> data) = 0;
            virtual void remove_data(Id data_identifier) = 0;
            virtual DataInterface<Msg, UniversalDataFormat> at(Id data_identifier) = 0;
            virtual size_t size(Id data_identifier) = 0;
            virtual void modify_data(Id data_identifier, DataInterface<Msg, UniversalDataFormat> data) = 0;
        };



        template <typename Msg, typename UniversalDataFormat>
        class VectorStorage : public DataStorageInterface<Msg, UniversalDataFormat, size_t>
        {
        private:
            std::vector<DataInterface<Msg, UniversalDataFormat>> data_storage;
        
        public:
            void add_data(DataInterface<Msg, UniversalDataFormat> data) override 
            { 
                this->data_storage.push_back(data); 
            }

            void remove_data(size_t data_identifier) override
            {
                this->data_storage.erase(this->data_storage.begin() + data_identifier);
            }

            DataInterface<Msg, UniversalDataFormat> at(size_t data_identifier) override
            {
                return this->data_storage.at(data_identifier);
            }


            void modify_data(size_t data_identifier, DataInterface<Msg, UniversalDataFormat> new_data) override
            {
                this->data_storage.at(data_identifier) = new_data;
            }
        };


        template <typename Msg, typename UniversalDataFormat>
        class DataComposite : public DataInterface<Msg, UniversalDataFormat>
        {
        private:
            VectorStorage<Msg, UniversalDataFormat> data_storage;
        
        public:
            DataComposite(){}


            void add_data(DataInterface<Msg, UniversalDataFormat> data) 
            { 
                data_storage.add_data(data); 
            }
            // nlohmann::json get_json() override 
            // { 
            //     nlohmann::json json_composite;

            //     for (auto data : data_storage)
            //     {
            //         json_composite[data->name] = data->get_json();
            //     }

            //     return json_composite;
            // };
        };


        template <typename Msg>
        class DataCompositeJson : public DataComposite<Msg, nlohmann::json>
        {
            nlohmann::json get_data() override 
            {
                nlohmann::json json_composite;

                for (size_t i = 0; i < this->data_storage.size(); i++)
                {
                    json_composite[this->data_storage.at(i).get_name()] = this->data_storage.at(i).get_data();
                }

                return json_composite;
            }
        };


        /**
         * @brief Интерфейс класса для подписки на получение данных из объекта DataCollectorInterface
         */
        template <typename Msg, typename UniversalDataFormat>
        class DataObserverInterface
        {
        public:
            /**
             * @brief метод, который будет вызван, когда DataCollectorInterface<DataType> изменит свое состояние
             */
            virtual void notify(std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> input_data) = 0;
            virtual std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> get_data() = 0;
        };


        template <typename Msg, typename UniversalDataFormat>
        class DataCollectorInterface
        {
        public:
            /**
             * @brief метод для изменения состояния субъекта
             */
            virtual void update_data(std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> data) = 0;

            /**
             * @brief метод, который добавляет наблюдателя к набору наблюдателей, которые хотят получать уведомления о изменении состояния субъекта
             */
            virtual void attach_observer(std::shared_ptr<DataObserverInterface<Msg, UniversalDataFormat>> observer) = 0;

        protected:
            /**
             * @brief метод, который уведомляет всех наблюдателей о изменении состояния субъекта, и передает им текущее состояние
             */
            virtual void notify_observers() = 0;
        };


        template <typename Msg, typename UniversalDataFormat>
        class SingleObserverDataCollector : public DataCollectorInterface<Msg, UniversalDataFormat>
        {
        public:
            void attach_observer(std::shared_ptr<DataObserverInterface<Msg, UniversalDataFormat>> observer) override 
            {
                this->observer = observer;
            }

            void update_data(std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> data) override 
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
            std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> data;
            std::shared_ptr<DataObserverInterface<Msg, UniversalDataFormat>> observer;
        };



        /**
         * @brief Класс, который подписывается на обновления состояния DataCollectorInterface.
         */

        template <typename Msg, typename UniversalDataFormat>
        class DataSubscriber : public DataObserverInterface<Msg, UniversalDataFormat>
        {
        public:
            /**
             * @brief Конструктор объекта DataSubscriber.
             */
            DataSubscriber(std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> input_data) { this->data = input_data; }
            // DataSubscriber() { this->data = std::make_shared<DataInterface<Msg, UniversalDataFormat>>(); }

            /**
             * @brief Обновляет данные по указателю this->data.
             */
            void notify(std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> input_data) override
            {
                std::lock_guard<std::mutex> lock(data_mx);
                // std::cout << "\nBefore:  " << input_data->get_data() << std::endl;
                this->data = input_data;
                // std::cout << "After:  " << this->data->get_data() << std::endl;
            }


            std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> get_data() override 
            { 
                std::lock_guard<std::mutex> lock(data_mx);
                return this->data; 
            }


            // std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> get_data() 
            // { 
            //     return this->data;
            //     // return std::make_shared<DataInterface<Msg, UniversalDataFormat>>();
            // }


        protected:
            std::shared_ptr<DataInterface<Msg, UniversalDataFormat>> data; ///< Указатель на объект, который будет обновляться при изменении состояния субъекта DataCollectorInterface.
            std::mutex data_mx;
        };
    };
};
