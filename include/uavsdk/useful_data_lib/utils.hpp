#pragma once
#include <stdexcept>
#include <uavsdk/useful_data_lib/base_interfaces.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{
namespace data_utils
{

/**
* @brief Конвертирует указатель из TypeInterface в желаемый тип, выполняя все необходимые проверки
* @tparam T тип, к которому вы хотите привести данный указатель
* @param obj_p объект указателя, который вы пытаетесь конвертировать
* @returns std::shared_ptr<T> Указатель, приведенный к желаемому типу
*
* @warning Функция может бросать исключения в следующих случаях:
* 1. obj_p == nullptr
* 2. Тип объекта, хранящегося в obj_p не совпадает с типом T, в который требуется конвертация
* 3. Тип T не является наследником TypeInterface
*/
template <typename T>
std::shared_ptr<T> convert(std::shared_ptr<TypeInterface> obj_p)
{
    if (!obj_p) throw std::runtime_error("useful_di::utils::convert(): Invalid pointer passed.");
    if (obj_p->___get_type() != utils::cppext::get_type<T>()) throw std::runtime_error("useful_di::utils::convert(): Tried to convert TypeInterface with type " + obj_p->___get_type() + "to type" + utils::cppext::get_type<T>());
    if (!std::is_base_of<TypeInterface, T>()) throw std::runtime_error("useful_di::utils::convert(): Tried to convert TypeInterface with type " + obj_p->___get_type() + "to type" + utils::cppext::get_type<T>() + " which is not dervied from TypeInterface.");
    return std::dynamic_pointer_cast<T>(obj_p);
}

} // namespace utils
} // namespace useful_di
