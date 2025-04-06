#pragma once
#include <memory>
#include <map>

#include <uavsdk/useful_data_lib/base_interfaces.hpp>

#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{

// ##########################################
// #########    Base Interfaces     #########
// ##########################################


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
    class UniversalDataInterface : public virtual TypeInterface, public IRetrievableDataContainer<UniversalDataFormat>
    {
        public:
            /**
             * @brief Returns universal data structure of type UniversalDataFormat
             */
            UniversalDataFormat get_data() override
            {
                return this->data;
            }


        protected:
            UniversalDataFormat data; // Данные в универсальном формате
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
    class IBaseStorage : public virtual TypeInterface, public IRemoveAbleContainer<Id>, public IAtAbleContainer<Id>, public IModifyAbleContainer<Id>, public ISizeAbleContainer
    {};



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
    class IDataObserver : virtual public TypeInterface
    {
    public:
        /**
         * @brief метод, который будет вызван, когда DataCollectorInterface<DataType> изменит свое состояние
         */
        virtual void be_notified(std::shared_ptr<TypeInterface> input_data) = 0;
        // virtual std::shared_ptr<SubjectType> get_data() = 0;
    };


    template <typename SubjectType>
    class RetrievableDataObserver : public IDataObserver, public IRetrievableDataContainer<std::shared_ptr<SubjectType>>
    {
        static_assert(std::is_base_of<TypeInterface, SubjectType>(), "RetrievableDataObserver: provided SubjectType is not derived from TypeInterface.");
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
                std::runtime_error("DataObserverWithCallback: Callback was not set but has already been called.");
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


    /**
     * 
     * @tparam SubjectType тип изначального объекта данных
     */
    // template <typename SubjectType>
    class IDataCollector : public virtual TypeInterface
    {
    public:
        // static_assert(std::is_base_of<useful_di::TypeInterface, SubjectType>::value, "DataCollectorInterface: provided SubjectType must be derived from TypeInterface (currently it is not).");
        /**
         * @brief метод для изменения состояния субъекта
         */
        virtual void update_data(std::shared_ptr<TypeInterface> data) = 0;

        /**
         * @brief метод, который добавляет наблюдателя к набору наблюдателей, которые хотят получать уведомления о изменении состояния субъекта
         */
        virtual void attach_observer(std::shared_ptr<IDataObserver> observer) = 0;

    protected:
        /**
         * @brief метод, который уведомляет всех наблюдателей о изменении состояния субъекта, и передает им текущее состояние
         */
        virtual void notify_observers() = 0;
    };


    template <typename Id>
    class IDataDependencyManager : public virtual TypeInterface
    {
    public:
        virtual void bind_input_to(Id identifier, std::shared_ptr<DataObserverWithCallback> output) = 0; 
        virtual void bind_output_to(Id identifier, std::shared_ptr<IDataObserver> input) = 0; 

    protected:
        std::map<Id, std::shared_ptr<DataObserverWithCallback>> data_inputs;
        std::map<Id, std::shared_ptr<IDataCollector>> data_outputs;
    };
};