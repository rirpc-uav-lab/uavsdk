#include <map>
#include <string>
#include <memory>

#include <gtest/gtest.h>

#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/utils/cpp_custom_extras.hpp"

#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/useful_data_lib/useful_data_interfaces.hpp"
#include "uavsdk/useful_data_lib/useful_data_impl.hpp"

using namespace useful_di;
using uavsdk::data_adapters::cxx::BasicDataAdapter;

TEST(useful_data_imp, UniMapStrWorksTestKey)
{
    // Проверка создания и передачи ключей.

    UniMapStr UMS_test;
    std::vector<std::string> test_key;
    
    std::string test_id = "test_id";
    auto testData = std::make_shared<BasicDataAdapter<int>>(1);   

    UMS_test.add_data(test_id,testData);
    
    test_key = UMS_test.get_present_keys();
    EXPECT_FALSE(test_key.empty()) << "TEST_MESSAGE:the keys were not sent\n";
}

TEST(useful_data_imp, UniMapStrWorksTestKey_BREAK_TEST)
{
    // Проверка создания и передачи ключей. Попытка сломать тест

    UniMapStr UMS_test;
    std::vector<std::string> test_key;
    
    test_key = UMS_test.get_present_keys();
    EXPECT_TRUE(test_key.empty()) << "TEST_MESSAGE: keys is not empty\n";
}


TEST(useful_data_imp, UniMapStrWorksModifaiDataIntToInt)
{
    // Проверка замены данных в ячейке данными того же типа.

    UniMapStr UMS_test;
    std::string test_id = "test_id";

    auto testData = std::make_shared<BasicDataAdapter<int>>(1);    
    auto testModifyData = std::make_shared<BasicDataAdapter<int>>(2);    

    UMS_test.add_data(test_id,testData);

    EXPECT_NO_THROW(UMS_test.modify_data("test_id",testModifyData)) << "\nTEST_MESSAGE: modify_data() mod error";
    EXPECT_NE(testData, UMS_test.at(test_id)) << "\nTEST_MESSAGE: the data has not been modified";
}

TEST(useful_data_imp, UniMapStrWorksModifaiDataIntToStr_BREAK_TEST)
{
    // Проверка замены данных в ячейке данными иного типа. Попытка сломать тест.

    UniMapStr UMS_test;
    std::string test_id = "test_id";

    auto testData = std::make_shared<BasicDataAdapter<int>>(1);       
    auto testModifyDataIntToStr = std::make_shared<BasicDataAdapter<std::string>>("testValue");    

    UMS_test.add_data(test_id, testData);
    EXPECT_NO_THROW(UMS_test.modify_data("test_id",testModifyDataIntToStr)) << "\nTEST_MESSAGE: no type conflict detected";
}

TEST(useful_data_imp, UniMapStrWorksRemoveData)
{
    // Нормальная проверка удаления данных

    UniMapStr UMS_test;
    std::string test_id = "test_id";
    auto testData = std::make_shared<BasicDataAdapter<int>>(1);    
    
    UMS_test.add_data(test_id,testData);
    EXPECT_NO_THROW(UMS_test.remove_data(test_id)) << "\n TEST_MESSAGE: remove_data() attempt to delete non-existent element";

    EXPECT_ANY_THROW(UMS_test.at(test_id)) << "\nTEST_MESSAGE: data was not removed";
}

TEST(useful_data_imp, UniMapStrWorksRemoveData_BREAK_TEST)
{
    // Провека удаления данных с целью сломать тест

    UniMapStr UMS_test;
    std::string test_id = "test_id";
    
    EXPECT_ANY_THROW(UMS_test.remove_data(test_id)) << "\n TEST_MESSAGE: attempt to delete non-existent element";
}

TEST(useful_data_imp, UniMapStrWorksSize)
{
    // Проверка функции size

    UniMapStr UMS_test;
    std::string test_id = "test_id";
    auto testData = std::make_shared<BasicDataAdapter<int>>(1);    

    UMS_test.add_data(test_id,testData);
    EXPECT_TRUE(UMS_test.size()) << "UMS is empty";
}

TEST(useful_data_imp, UniMapStrWorksSize_BREAK_TEST)
{
    // Проверка функции size. Попытка сломать тест.

    UniMapStr UMS_test;
   
    EXPECT_FALSE(UMS_test.size()) << "UMS is not empty";
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}