#pragma once
#include <memory>
#include <map>

#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{

// ##########################################
// #########    Base Interfaces     #########
// ##########################################

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
        virtual void ___set_type()
        {
            this->___type = typeid(*this).name();
        };
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
    class UniversalDataInterface : public virtual TypeInterface
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





// ##########################################
// #########    Storage Interfaces     #########
// ##########################################

    template <typename Id>
    class IAppendAbleContainer
    {
        public:
        virtual Id add_data(const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    template <typename Id>
    class IInsertAbleContainer
    {
        public:
        virtual void add_data(const Id _key, const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    template <typename Id>
    class IRemoveAbleContainer
    {
        public:
            virtual void remove_data(const Id& data_identifier) = 0;
    };


    template <typename Id>
    class IAtAbleContainer
    {
        public:
            virtual std::shared_ptr<TypeInterface> at(const Id& data_identifier) = 0;
    };


    class ISizeAbleContainer
    {
        public:
            virtual size_t size() = 0;
    };


    template <typename Id>
    class IModifyAbleContainer
    {
        public:
            virtual void modify_data(const Id& data_identifier, const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    template <typename Id>
    class IBaseStorage : public virtual TypeInterface, public IRemoveAbleContainer<Id>, public IAtAbleContainer<Id>, public IModifyAbleContainer<Id>, public ISizeAbleContainer
    {};


    /**
     * @tparam SubjectType тип изначального объекта данных
     * @tparam UniversalDataFormat универсальный тип данных 
     * @tparam Id тип идентификатора объекта данных. Как правило, заранее определенный enum
     */
    template <typename Id>
    class DataStorageInterface : public IBaseStorage<Id>, public IAppendAbleContainer<Id>
    {};


    template <typename Id>
    class MapLikeDataStorageInterface : public IBaseStorage<Id>, public IInsertAbleContainer<Id>
    {
        public:

        protected:
        std::map<Id, std::shared_ptr<TypeInterface>> data_storage;
    };




// ##########################################
// #########    Pattern Interfaces     #########
// ##########################################


    template<typename Id, typename ThisStorageType>
    class IDataComposite : public ThisStorageType
    {
        static_assert(std::is_base_of<IBaseStorage<Id>, ThisStorageType>::value, "IDataComposite: provided StorageType is not derived from IBaseStorage");
    };


    // template <typename UniversalDataFormat, typename Id>
    // class ListLikeDataComposite : public DataCompositeInterface<UniversalDataFormat, 



    /**
     * @brief Интерфейс класса для подписки на получение данных из объекта DataCollectorInterface
     */
    template <typename SubjectType>
    class IDataObserver : public virtual TypeInterface
    {
    public:
        static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "IDataObserver: provided SubjectType must be derived from TypeInterface (currently it is not).");
        /**
         * @brief метод, который будет вызван, когда DataCollectorInterface<DataType> изменит свое состояние
         */
        virtual void be_notified(std::shared_ptr<SubjectType> input_data) = 0;
        virtual std::shared_ptr<SubjectType> get_data() = 0;
    };


    template <typename SubjectType>
    class DataObserverWithCallback : public IDataObserver<SubjectType>
    {
    public:
        virtual void set_callback(std::function<void(std::shared_ptr<SubjectType>)> callback)
        {
            this->callback_obj = callback;
            callback_set = true;
        }

        void callback(std::shared_ptr<SubjectType> msg) 
        {
            if (callback_set)
                this->callback_obj(msg);
            else 
                std::runtime_error("DataObserverWithCallback: Callback was not set but has already been called.");
        }

    
    protected:
        std::function<void(std::shared_ptr<SubjectType>)> callback_obj;
        bool callback_set{false};
    };


    /**
     * 
     * @tparam SubjectType тип изначального объекта данных
     */
    template <typename SubjectType>
    class DataCollectorInterface 
    {
    public:
        static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "DataCollectorInterface: provided SubjectType must be derived from TypeInterface (currently it is not).");
        /**
         * @brief метод для изменения состояния субъекта
         */
        virtual void update_data(std::shared_ptr<SubjectType> data) = 0;

        /**
         * @brief метод, который добавляет наблюдателя к набору наблюдателей, которые хотят получать уведомления о изменении состояния субъекта
         */
        virtual void attach_observer(std::shared_ptr<IDataObserver<SubjectType>> observer) = 0;

    protected:
        /**
         * @brief метод, который уведомляет всех наблюдателей о изменении состояния субъекта, и передает им текущее состояние
         */
        virtual void notify_observers() = 0;
    };
};