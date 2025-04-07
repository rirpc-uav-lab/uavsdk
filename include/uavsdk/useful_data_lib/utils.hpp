#include <uavsdk/useful_data_lib/base_interfaces.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>

namespace useful_di
{
namespace data_utils
{

template <typename T>
std::shared_ptr<T> convert(std::shared_ptr<TypeInterface> obj_p)
{
    if (obj_p->___get_type() != utils::cppext::get_type<T>()) throw std::runtime_error("useful_di::utils::convert(): Tried to convert TypeInterface with type " + obj_p->___get_type() + "to type" + utils::cppext::get_type<T>());
    if (!std::is_base_of<TypeInterface, T>()) throw std::runtime_error("useful_di::utils::convert(): Tried to convert TypeInterface with type " + obj_p->___get_type() + "to type" + utils::cppext::get_type<T>() + " which is not dervied from TypeInterface.");
    return std::dynamic_pointer_cast<T>(obj_p);
}

} // namespace utils
} // namespace useful_di