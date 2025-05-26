#include <gtest/gtest.h>

#include "uavsdk//utils/cpp_custom_extras.hpp"
#include "uavsdk/useful_data_lib/base_interfaces.hpp"



TEST(base_interfaces_test, TypeInterfaceSetTypeWorks)
{
    class Herit : public useful_di::TypeInterface
    {
    public:
        Herit()
        {
            this->___set_type();
        }
    };

    Herit object;
    ASSERT_EQ(object.___get_type(), utils::cppext::get_type<Herit>()) << "TEST_MESSAGE: Herit did not get the type. object.___get_type(): " << object.___get_type() << ". utils::cppext::get_type<Herit>(): " << utils::cppext::get_type<Herit>();
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
