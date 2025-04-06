
#pragma once
#include <memory>
// #include <map>

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
            this->___type = utils::cppext::demangle(typeid(*this).name());
        };
    };


    template <typename Retrieved>
    class IRetrievableDataContainer
    {
    public:
        virtual Retrieved get_data() = 0;
    };



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


    template <typename InitializedType>
    class IInitializeAble
    {
    public:
        virtual void initialize(InitializedType init_data) = 0;
    };


    template <typename BindT>
    class IBindable 
    {
    public:
        virtual void bind(BindT obj) = 0;
    };


    class IBoundCheckAble
    {
    public:
        virtual bool is_bound() = 0;
    };


    template <typename Id>
    class IHasStringConvertibleIdentifier 
    {
    public:
        virtual std::string to_str(Id id) = 0;
    };


    template <typename ExpectationsType>
    class IHasExpectations
    {
    public:
        virtual ExpectationsType get_expectations() = 0;
        virtual void set_expectations(ExpectationsType new_expectations) = 0;
        virtual const bool match_expectations(const ExpectationsType &other_expectations) = 0;
    protected:
        ExpectationsType expectations;
    };
} // namespace useful_di