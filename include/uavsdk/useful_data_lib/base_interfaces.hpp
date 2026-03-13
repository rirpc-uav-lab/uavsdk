
#pragma once
#include <memory>
#include <vector>
#include <stdexcept>
#include <string>

#include <uavsdk/utils/cpp_custom_extras.hpp>



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
            // this->___set_type(); // К сожалению, дает свой тип любому наследнику.
        }
        virtual ~TypeInterface() = default; 

        /**
         * @brief Возвращает строку, хранящую тип данного объекта, заданный
         * с помощью функции ___set_type()
         */
        std::string ___get_type() const
        {
            if (not this->___type.empty()) return this->___type;
            else throw std::runtime_error("TypeInterface: _set_type was not called before get_type");
        }


    protected:
        std::string ___type; // Тип этого объекта данных


        /**
         * @brief Этот метод указывает тип текущего объекта. 
         * Должен быть вызван во всех классах, наследующих TypeInterface прямо или косвенно, внутри конструктора.
         */
        virtual void ___set_type()
        {
            this->___type = utils::cppext::demangle(typeid(*this).name());
        };
    };


    /**
     * @brief Инетрфейс для объектов, хранящих данные, копию которых можно извлечь
     * @tparam Retrieved тип данных, хранящихся в контейнере
     */
    template <typename Retrieved>
    class IRetrievableDataContainer
    {
    public:
        virtual ~IRetrievableDataContainer() = default;
        
        /**
         * @brief Возвращает копию хранящихся данных
         */
        virtual Retrieved get_data() = 0;
    };


    /**
     * @brief Контейнер для данных, в которые можно добавить элемент в конец
     * @tparam Id Тип данных идентификатора, по которому можно обращаться к объектам в контейнере
     */
    template <typename Id>
    class IAppendAbleContainer
    {
    public:
        virtual ~IAppendAbleContainer() = default;
        
        /**
         * @brief Добавляет данные в конец контейнера
         * @param _data Оъект-наследник TypeInterface, являющийся сохраняемыми данными
         * @returns Id идентификатор добваленного объекта
         */
        virtual Id add_data(const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    /**
     * @brief Контейнер для данных, в которые можно добавить элемент на определенную позицию по Id
     * @tparam Id Тип данных идентификатора, по которому можно обращаться к объектам в контейнере
     */
    template <typename Id>
    class IInsertAbleContainer
    {
    public:
        virtual ~IInsertAbleContainer() = default;
        
        /**
         * @brief Добавляет данные в указанную позицию
         * @param _key идентификатор позиции
         * @param _data добавляемые данные 
         */
        virtual void add_data(const Id _key, const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    /**
     * @brief Интерфейс контейнера, из которого можно удалять элементы
     * @tparam Id Тип данных идентификатора, по которому можно обращаться к объектам в контейнере
     */
    template <typename Id>
    class IRemoveAbleContainer
    {
    public:
        virtual ~IRemoveAbleContainer() = default;
        
        /**
         * @brief Удаляет объект из контейнера
         * @param data_identifier идентификатор удаляемого объекта
         */
        virtual void remove_data(const Id& data_identifier) = 0;
    };



    /**
     * @brief Конетйнер, из которого можно получать данные функцией at
     * @tparam Id Тип данных идентификатора, по которому можно обращаться к объектам в контейнере
     * @tparam RetrievedT Тип данных хранимых в контейнер объектов     
     */
    template <typename Id, typename RetrievedT>
    class IAtAbleContainerGeneric
    {
    public:
        virtual ~IAtAbleContainerGeneric() = default;
        
        /**
         * @brief Возвращает объект из контейнера
         * @param data_identifier идентификатор возвращаемого объекта
         * @returns std::shared_ptr<TypeInterface> интерфейс данных. Вы должны заранее знать, какой тип хранится в этом месте контенера,
         * после чего попытаться привести тип данного указателя к желаемому через функцию useful_di::data_utils::convert<T>(std::shared_ptr<TypeInterface> obj_p) или другим способом.
         */
        virtual RetrievedT at(const Id& data_identifier) = 0;
    };


    /**
     * @brief Конетйнер, из которого можно получать данные функцией at
     * @tparam Id Тип данных идентификатора, по которому можно обращаться к объектам в контейнере
     */
    template <typename Id>
    class IAtAbleContainer : public virtual IAtAbleContainerGeneric<Id, std::shared_ptr<TypeInterface>>
    {
    public:
        virtual ~IAtAbleContainer() = default;
    };


    /**
     * @brief Контейнер, данные из которого можно получать не только по одному, но и сразу несколько элементов за раз
     * @tparam Id тип идентификатора 
     * @tparam ReturnedContainerT возвращаемый тип контейнера
     */
    template <typename Id, typename ReturnedContainerT>
    class IBulkAtAbleContainer : public virtual IAtAbleContainer<Id>, public virtual IAtAbleContainerGeneric<std::vector<Id>, ReturnedContainerT>
    {
        public:
            virtual ~IBulkAtAbleContainer() = default;
            
            /**
             * @brief Возвращает все объекты, хранящиеся в контейнере
             * @returns ReturnedContainerT все объекты в контейнере
             */
            virtual ReturnedContainerT get_all() = 0;
    };


    /**
     * @brief Контейнер, размер которого можно получить 
     */
    class ISizeAbleContainer
    {
        public:
            virtual ~ISizeAbleContainer() = default;
            
            /**
             * @brief Возвращает размер контейнера
             */
            virtual size_t size() = 0;
    };


    
    /**
     * @brief Контейнер, данные в котором можно изменять по ключу
     * @tparam Id тип идентификатора 
     */
    template <typename Id>
    class IModifyAbleContainer
    {
    public:
        virtual ~IModifyAbleContainer() = default;
        
        /**
         * @brief Заменяет объект данных в контейнере
         * @param data_identifier ключ
         * @param _data новые данные 
         */
        virtual void modify_data(const Id& data_identifier, const std::shared_ptr<TypeInterface>& _data) = 0;
    };


    /**
     * @brief Интерфейс для объектов, которые можно инициализировать
     * @tparam InitializedType тип объекта параметров инициализации
     */
    template <typename InitializedType>
    class IInitializeAble
    {
    public:
        virtual ~IInitializeAble() = default;
        
        /**
         * @brief Функция инициализации объекта
         * @param init_data объект параметров инициализации
         */
        virtual void initialize(InitializedType init_data) = 0;
    };


    /**
     * @brief Интерфейс для объектов, которые можно привязывать
     * @tparam BindT Тип объекта, к которому привязываем текущий объект
     */ 
    template <typename BindT>
    class IBindable
    {
    public:
        virtual ~IBindable() = default;
        
        /**
         * @brief Функция привязки текущего объекта
         * @param obj объект, к которому привязываемся  
         */
        virtual void bind(BindT obj) = 0;
    };


    /**
     * @bried Интерфейс для объектов, привязку которых можно проверять
     */
    class IBoundCheckAble
    {
    public:
        virtual ~IBoundCheckAble() = default;
        
        /**
         * @brief Функция проверки привязки текущего объекта
         * @returns bool true если привязан, иначе false
         */
        virtual bool is_bound() = 0;
    };


    /**
     * @brief Интерфейс для объектов, у которых есть идентификатор
     * @tparam Id тип идентификатора
     */
    template <typename Id>
    class IHasIdentifier
    {
    public:
        /**
         * @brief Возвращает текущее значение идентификатора
         */
        Id get_id() { return this->id; }
        
        /**
         * @brief Функция для установки нового значения идентификатора
         * @param new_id Новое значение идентификатора
         */
        void set_id(Id new_id) { this->id = new_id; }
    protected:
        Id id; /// Объект идентификатора
    };


    /**
     * @brief Интерфейс для объектов, имеющих идентификатор, конвертируемый в std::string
     * @tparam Id тип идентификатора
     */
    template <typename Id>
    class IHasStringConvertibleIdentifier : public virtual IHasIdentifier<Id> 
    {
    public:
        virtual ~IHasStringConvertibleIdentifier() = default;
        // virtual std::string to_str(Id id) = 0;
        /**
         * @brief Возвращает идентификатор в формате строки
         */ 
        virtual std::string get_str_id() = 0;
    };


    /**
     * @brief Интерфейс для объектов, которые могут предъявлять определенные требования к входящим параметрам и аргументам методов.
     * Определяет стандартный протокол взаимодействия объектов, имеющих требования к входным данным в условиях динамического полиморфизма
     *
     * @tparam ExpectationsType тип объекта ожиданий
     */
    template <typename ExpectationsType>
    class IHasExpectations
    {
    public:
        virtual ~IHasExpectations() = default;
        /**
         * @brief Возвращает объект ожиданий
         */
        virtual ExpectationsType get_expectations() = 0;
        
        /**
         * @brief Функция для сравнения ожиданий двух объектов
         * @param other_expectations Объект ожиданий другого объекта, с которым ожидания сравниваются
         * @returns bool true, если ожидания совместимы, иначе false
         */
        virtual bool match_expectations(const ExpectationsType &other_expectations) = 0;
    protected:
        ExpectationsType expectations; /// Объект ожиданий
        /**
         * @brief Устанавливает новое значение обхекта ожиданий
         * @param new_expectations объект новых ожиданий
         */ 
        virtual void set_expectations(ExpectationsType new_expectations) = 0;
    };


    /**
     * @brief An interface for containers that implement check on if the value with the given Id is present in the contatiner
     * @tparam Id data type of id
     */
    template <typename Id>
    class IHasValueForKey
    {
    public:
        virtual ~IHasValueForKey() = default;

        /**
         * @brief Checks if a value at a given key is present in the container
         * @param key Id to search value for key
         * @returns bool true if found, false if not
         */
        virtual bool has(const Id& key) = 0;
        virtual std::vector<std::string> get_present_keys() = 0;
    };
} // namespace useful_di
