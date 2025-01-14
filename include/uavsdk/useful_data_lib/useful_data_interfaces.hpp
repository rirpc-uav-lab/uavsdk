#pragma once
#include <memory>
#include <map>

namespace useful_di
{
    class TypeInterface
    {
        public:
        TypeInterface()
        {
            // this->___set_type();
        }


        std::string ___get_type() 
        {
            if (not this->___type.empty()) return this->___type;
            else throw std::runtime_error("TypeInterface: _set_type was not called before get_type");
        }


        protected:
        std::string ___type;


        /**
         * @brief Этот метод указывает тип текущего объекта. 
         * Его необходимо определять в "листьях" - финальных реализациях 
         * последовательности интерфейсов, образованной наследованием 
         * от этого класса. Если в своей реализации этого метода вы
         * не уверены, финальная ли она, сделайте реализацию этого метода
         * виртуальной, чтобы ее можно было переопределить
         */
        virtual void ___set_type() = 0;
    };


    /**
     * @tparam SubjectType тип изначального объекта данных
     * @tparam UniversalDataFormat универсальный тип данных 
     */
    template<typename SubjectType, typename UniversalDataFormat>
    class DataInterface : public TypeInterface
    {
        public:
        // void set_data(UniversalDataFormat _data)
        // {
        //     this->_set_data(_data);
        // }
        

        /**
         * @brief Returns universal data structure of type UniversalDataFormat
         */
        UniversalDataFormat get_data()
        {
            return this->_get_data();
        }


        // void set_msg(const SubjectType& _msg)
        // {
        //     this->_set_msg(_msg);
        // }


        /**
         * @brief returns original data structure of type SubjectType
         */
        SubjectType get_msg()
        {
            return this->_get_msg();
        }



        protected:
        UniversalDataFormat data;
        SubjectType msg;


        // virtual void _set_data(UniversalDataFormat& _data)
        // {
        //     this->data = _data;
        // }


        virtual UniversalDataFormat _get_data()
        {
            return this->data;
        }


        virtual SubjectType _get_msg()
        {
            return this->msg;
        }


        // virtual void _set_msg(const SubjectType& _msg)
        // {
        //     this->msg = _msg;
        // }
    };


    /**
     * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
     */
    template <typename Id>
    class IdFactoryInterface 
    {
        public:
            virtual Id get_id(std::shared_ptr<TypeInterface> data) = 0;
    };


    /**
     * @tparam SubjectType тип изначального объекта данных
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
     */
    template <typename Id, typename IdFactoryType>
    class DataStorageInterface
    {
    public:
        Id add_data(const std::shared_ptr<TypeInterface>& _data)
        {
            return this->_add_data(_data);
        }

        void remove_data(const Id& data_identifier)
        {
            this->_remove_data(data_identifier);
        }

        std::shared_ptr<TypeInterface> at(const Id& data_identifier)
        {
            return this->_at(data_identifier);
        }



        size_t size()
        {
            return this->_size();
        }
        
        
        void modify_data(const Id& data_identifier, const std::shared_ptr<TypeInterface>& _data)
        {
            this->_modify_data(data_identifier, _data);
        }


    protected:
        std::shared_ptr<IdFactoryType> _id_factory;
        virtual Id _get_id_for_data(const std::shared_ptr<TypeInterface>& _data) = 0;
        virtual Id _add_data(const std::shared_ptr<TypeInterface>& _data) = 0;
        virtual void _remove_data(const Id& data_identifier) = 0;
        virtual std::shared_ptr<TypeInterface> _at(const Id& data_identifier) = 0;
        virtual size_t _size() = 0;
        virtual void _modify_data(const Id& data_identifier, const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    //  * @tparam CompositeSubjectType - композит объектов  изначальных типов
    /**
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
     * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
     */
    template <typename UniversalDataFormat, typename Id, typename IdFactoryType, typename StorageType>
    class DataCompositeInterface : public DataInterface<std::shared_ptr<DataStorageInterface<Id, IdFactoryType>>, UniversalDataFormat>
    {
        static_assert(std::is_base_of<DataStorageInterface<Id, IdFactoryType>, StorageType>::value, "DataCompositeInterface: provided StorageType is not derived from DataStorageInterface");
    protected:
        // std::shared_ptr<DataStorageInterface<Msg, UniversalDataFormat, Id>> data_storage;
    
    public:
        DataCompositeInterface()
        {
            this->msg = std::make_shared<StorageType>();
        }

        virtual void add_data(std::shared_ptr<TypeInterface> data) = 0;
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
            virtual void notify(std::shared_ptr<useful_di::TypeInterface> input_data) = 0;
            virtual std::shared_ptr<useful_di::TypeInterface> get_data() = 0;
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
            virtual void update_data(std::shared_ptr<useful_di::TypeInterface> data) = 0;

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