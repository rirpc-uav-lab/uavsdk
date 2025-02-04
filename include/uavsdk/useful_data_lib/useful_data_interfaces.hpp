#pragma once
#include <memory>
#include <map>

namespace useful_di
{

    /**
     * @brief Интерфейс типа. Любой класс, являющися объектом данных и 
     * предполагающий совместимость с композитной системой библиотеки useful_di, 
     * должен напрямую или не напрямую унаследовать от этого класса. Каждый 
     * класс наследник должен определить функцию ___set_type() и вызывать ее, 
     * например, в конструкторе, чтобы можно было идентифицировать тип объекта 
     * данных
     */
    class TypeInterface
    {
        public:
        TypeInterface()
        {
            // this->___set_type();
        }


        /**
         * @brief Возвращает строку, хранящую тип данного объекта, заданный
         * с помощью функции ___set_type()
         */
        std::string ___get_type() 
        {
            if (not this->___type.empty()) return this->___type;
            else throw std::runtime_error("TypeInterface: _set_type was not called before get_type");
        }


        protected:
        std::string ___type; // Тип этого объекта данных


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
     * @brief Класс, являющийся интерфейсом для адаптации используемых
     * объектов данных к универсальному типу данных, в котором можно
     * выразить любую структуру данных. Примерами таких типов являются:
     * 
     * - json
     * 
     * - yaml
     * 
     * - битовые строки
     * 
     * и другие.
     * 
     * @tparam UniversalDataFormat выбранный универсальный тип
     * 
     * @warning Внимание! Установка значения параметра data - Ваша обязанность!
     * Лучше всего это делать в конструкторе, так как этот класс подразумевался
     * как постоянный объект, хранящий выражение объекта-оригинала в универсальном
     * формате
     */
    template <typename UniversalDataFormat>
    class UniversalDataInterface : public TypeInterface
    {
        public:
            /**
             * @brief Returns universal data structure of type UniversalDataFormat
             */
            UniversalDataFormat get_data()
            {
                return this->_get_data();
            }


        protected:
            UniversalDataFormat data; // Данные в универсальном формате


            /**
             * @brief может быть использована для кастомизации логики работы
             * публичного метода get_data()
             */
            virtual UniversalDataFormat _get_data()
            {
                return this->data;
            }
    };


    /**
     * @brief Продолжение UniversalDataInterface, которое позволяет конвертировать 
     * хранимые универсальные интерфейсы в их конкретные реализации и извлекать
     * объекты-оригиналы.
     * 
     * @tparam SubjectType тип изначального объекта данных
     * @tparam UniversalDataFormat универсальный тип данных 
     * 
     * @warning Внимание! Установка значения параметров data и msg - Ваша обязанность!
     * Лучше всего это делать в конструкторе, так как этот класс подразумевался
     * как постоянный контейнер, хранящий объект-оригинал и его вырежние в 
     * универсальном формате
     */
    template<typename SubjectType, typename UniversalDataFormat>
    class DataInterface : public UniversalDataInterface<UniversalDataFormat>
    {
        public:
        /**
         * @brief returns original data structure of type SubjectType
         */
        SubjectType get_msg()
        {
            return this->_get_msg();
        }


        protected:
        SubjectType msg; // Объект-оригинал


        /**
         * @brief Позволяет кастомизировать работу публичного метода get_msg()
         */
        virtual SubjectType _get_msg()
        {
            return this->msg;
        }
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
    template <typename Id, typename IdFactoryType, typename UniversalDataFormat>
    class DataStorageInterface
    {
    public:
        Id add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data)
        {
            return this->_add_data(_data);
        }

        void remove_data(const Id& data_identifier)
        {
            this->_remove_data(data_identifier);
        }

        std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> at(const Id& data_identifier)
        {
            return this->_at(data_identifier);
        }


        size_t size()
        {
            return this->_size();
        }
        
        
        void modify_data(const Id& data_identifier, const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data)
        {
            this->_modify_data(data_identifier, _data);
        }


    protected:
        std::shared_ptr<IdFactoryType> _id_factory;
        virtual Id _get_id_for_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data) = 0;
        virtual Id _add_data(const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data) = 0;
        virtual void _remove_data(const Id& data_identifier) = 0;
        virtual std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> _at(const Id& data_identifier) = 0;
        virtual size_t _size() = 0;
        virtual void _modify_data(const Id& data_identifier, const std::shared_ptr<UniversalDataInterface<UniversalDataFormat>>& _data) = 0;
    };


    /**
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
     * @tparam SubjectTypes множество типов изначальных объектов данных, которые могут использоваться в контейтере, индексируемом перечислением, которое создает этот класс
     */
    template <typename UniversalDataFormat, typename Id, typename IdFactoryType, typename StorageType>
    class DataCompositeInterface : public DataInterface<std::shared_ptr<DataStorageInterface<Id, IdFactoryType, UniversalDataFormat>>, UniversalDataFormat>
    {
        static_assert(std::is_base_of<DataStorageInterface<Id, IdFactoryType, UniversalDataFormat>, StorageType>::value, "DataCompositeInterface: provided StorageType is not derived from DataStorageInterface");
    protected:
        // std::shared_ptr<DataStorageInterface<Msg, UniversalDataFormat, Id>> data_storage;
    
    public:
        DataCompositeInterface()
        {
            this->msg = std::make_shared<StorageType>();
        }

        virtual void add_data(std::shared_ptr<UniversalDataInterface<UniversalDataFormat>> data) = 0;
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
            virtual void be_notified(std::shared_ptr<useful_di::UniversalDataInterface<UniversalDataFormat>> input_data) = 0;
            virtual std::shared_ptr<useful_di::UniversalDataInterface<UniversalDataFormat>> get_data() = 0;
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
            virtual void update_data(std::shared_ptr<useful_di::UniversalDataInterface<UniversalDataFormat>> data) = 0;

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