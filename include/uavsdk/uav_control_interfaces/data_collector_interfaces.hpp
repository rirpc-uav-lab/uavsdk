#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <variant>
#include <mutex>
#include <type_traits>

#include <nlohmann/json.hpp>


namespace uav_ci /// uav_control_interfaces
{
    namespace data_ci /// data_collector_interfaces
    {
        /**
         * @tparam SubjectType тип изначального объекта данных
         * @tparam UniversalDataFormat универсальный тип данных 
         */
        template <typename SubjectType, typename UniversalDataFormat>
        class DataInterface
        {
        public:
            virtual UniversalDataFormat get_data() = 0;
            virtual SubjectType get_msg() = 0;
        };


        /**
         * @tparam SubjectType тип изначального объекта данных
         * @tparam UniversalDataFormat универсальный тип данных 
         * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
         */
        template <typename UniversalDataFormat, typename Id, typename IdFactoryType, typename... SubjectTypes>
        class DataStorageInterface
        {
        using ContainedDataImpl = std::variant<std::shared_ptr<DataInterface<SubjectTypes, UniversalDataFormat>>...>;
        public:
            virtual Id add_data(ContainedDataImpl data) = 0;
            virtual void remove_data(Id data_identifier) = 0;
            virtual ContainedDataImpl at(Id data_identifier) = 0;
            virtual size_t size() = 0;
            virtual void modify_data(Id data_identifier, ContainedDataImpl data) = 0;
        protected:
            std::shared_ptr<IdFactoryType> _id_factory;

            virtual Id get_id_for_data(ContainedDataImpl data) = 0;
        };


        /**
         * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
         * @tparam UniversalDataFormat универсальный тип данных 
         * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
         */
        template <typename Id, typename UniversalDataFormat, typename... SubjectTypes>
        class IdFactoryInterface 
        {
            using ContainedDataImpl = std::variant<std::shared_ptr<DataInterface<SubjectTypes, UniversalDataFormat>...>>; 
            public:
                virtual Id get_id(ContainedDataImpl data) = 0;
        };


        //  * @tparam CompositeSubjectType - композит объектов  изначальных типов
        /**
         * @tparam UniversalDataFormat универсальный тип данных 
         * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
         * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
         */
        template <typename UniversalDataFormat, typename Id, typename IdFactoryType, typename... SubjectTypes>
        class DataCompositeInterface : public DataInterface<DataStorageInterface<UniversalDataFormat, Id, IdFactoryType, SubjectTypes...>, UniversalDataFormat>
        {
        protected:
            // std::shared_ptr<DataStorageInterface<Msg, UniversalDataFormat, Id>> data_storage;
            std::shared_ptr<DataStorageInterface<UniversalDataFormat, Id, IdFactoryType, SubjectTypes...>> data_storage;
        
        public:
            DataCompositeInterface(){}

            virtual void add_data(std::variant<std::shared_ptr<DataInterface<SubjectTypes, UniversalDataFormat>>...> data) = 0;
        };


        /**
         * @brief Интерфейс класса для подписки на получение данных из объекта DataCollectorInterface
         * @tparam SubjectType тип изначального объекта данных
         * @tparam UniversalDataFormat универсальный тип данных 
         */
        template <typename SubjectType, typename UniversalDataFormat>
        class DataObserverInterface
        {
        public:
            /**
             * @brief метод, который будет вызван, когда DataCollectorInterface<DataType> изменит свое состояние
             */
            virtual void notify(std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> input_data) = 0;
            virtual std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> get_data() = 0;
        };

        /**
         * 
         * @tparam SubjectType тип изначального объекта данных
         * @tparam UniversalDataFormat универсальный тип данных 
         */
        template <typename SubjectType, typename UniversalDataFormat>
        class DataCollectorInterface
        {
        public:
            /**
             * @brief метод для изменения состояния субъекта
             */
            virtual void update_data(std::shared_ptr<DataInterface<SubjectType, UniversalDataFormat>> data) = 0;

            /**
             * @brief метод, который добавляет наблюдателя к набору наблюдателей, которые хотят получать уведомления о изменении состояния субъекта
             */
            virtual void attach_observer(std::shared_ptr<DataObserverInterface<SubjectType, UniversalDataFormat>> observer) = 0;

        protected:
            /**
             * @brief метод, который уведомляет всех наблюдателей о изменении состояния субъекта, и передает им текущее состояние
             */
            virtual void notify_observers() = 0;
        };
    };
};
