#include <string>
#include <memory>

#include <gtest/gtest.h>

#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/data_adapters/cxx/cxx.hpp"

#include "uavsdk/data_adapters/cxx/cxx.hpp"
#include "uavsdk/useful_data_lib/useful_data_impl.hpp"

using namespace useful_di;
using uavsdk::data_adapters::cxx::BasicDataAdapter;


TEST(useful_data_imp, BlackboardGetKeys)
{
    // Проверка создания и передачи ключей.

    Blackboard bb_test;
    std::vector<std::string> test_key;
    
    std::string test_id = "test_id";
    auto testData = std::make_shared<BasicDataAdapter<int>>(1);   

    bb_test.add_data(test_id,testData);

    test_key = bb_test.get_keys_from_blackboard();
    EXPECT_FALSE(test_key.empty())  << "TEST_MESSAGE:the keys were not sent\n";
    EXPECT_EQ(test_key.size(),1)    << "TEST_MESSAGE: dimension mismatch get_present_keys_from_blackboard()\n";

    bb_test.remove_data(test_id);
    test_key = bb_test.get_keys_from_blackboard();

    EXPECT_FALSE(test_key.size()) << "TEST_MESSAGE: error deleting key\n";
}

TEST(useful_data_imp, BlackboardGetKeys_BREAK_TEST)
{
    // Проверка создания и передачи ключей. Попытка сломать тест
    Blackboard bb_test;
    std::vector<std::string> test_key;
    
    test_key = bb_test.get_keys_from_blackboard();
    EXPECT_TRUE(test_key.empty()) << "TEST_MESSAGE: keys is not empty\n";
}

TEST(useful_data_imp,BlackboardAddData_BREAK_TEST)
{
    // Проверка добавления данных в blackboard + попытка сломать код
    Blackboard bb_test;
    std::string test_id = "test_id";
    auto testDataInt1 = std::make_shared<BasicDataAdapter<int>>(1);   
    auto testDataInt2 = std::make_shared<BasicDataAdapter<int>>(2);   

    auto testDataStr = std::make_shared<BasicDataAdapter<std::string>>("testValue");   

    EXPECT_NO_THROW(bb_test.add_data(test_id,testDataInt1)) << "\nTEST_MESSAGE: error adding data to blackboard. type-1";
    EXPECT_NO_THROW(bb_test.add_data(test_id,testDataInt2)) << "\nTEST_MESSAGE: error adding data to blackboard. type-2";

    EXPECT_EQ(bb_test.at(test_id), testDataInt2) << "\nTEST_MESSAGE: variable overwrite error";

    EXPECT_ANY_THROW(bb_test.add_data(test_id,testDataStr)) << "\nTEST_MESSAGE: No type conflict detected";
}

TEST(useful_data_imp,BlackboardModifyData)
{
    // Проверка изменения данных в blackboard + попытка сломать код

    Blackboard bb_test;
    std::string test_id = "test_id";    
    std::string break_test_id = "break_test_id";

    auto testDataInt1 = std::make_shared<BasicDataAdapter<int>>(1);   
    auto testDataInt2 = std::make_shared<BasicDataAdapter<int>>(2);   

    auto testDataStr = std::make_shared<BasicDataAdapter<std::string>>("testValue");   

    bb_test.add_data(test_id,testDataInt1);

    EXPECT_NO_THROW(bb_test.modify_data(test_id,testDataInt2)) << "\nTEST_MESSAGE: error modify data to blackboard. type-2";
    EXPECT_ANY_THROW(bb_test.modify_data(test_id,testDataStr)) << "\nTEST_MESSAGE: No type conflict detected";

    EXPECT_ANY_THROW(bb_test.modify_data(break_test_id,testDataInt1)) << "\nTEST_MESSAGE: no attempt detected to change value by non-existent key";
}

TEST(usefel_data_imp,BlackboardRemoveData)
{
    // Проверка удаления данных в blackboard + попытка сломать код

    Blackboard bb_test;
    std::string test_id = "test_id";    

    auto testDataInt = std::make_shared<BasicDataAdapter<int>>(1);   

    bb_test.add_data(test_id,testDataInt);

    EXPECT_NO_THROW(bb_test.remove_data(test_id)) << "\nTEST_MESSAGE: data deletion error";
    EXPECT_ANY_THROW(bb_test.at(test_id)) << "\nTEST_MESSAGE: The key was not deleted";

    EXPECT_ANY_THROW(bb_test.remove_data(test_id)) << "\nTEST_MESSAGE: No attempt to delete a non-existent key was detected";
}

TEST(useful_data_imp,BlackboardSize)
{
    // Проверка size

    Blackboard bb_test;

    EXPECT_EQ(bb_test.size(),0) << "\nTEST_MESSAGE: size() not empty when created";

    std::string test_id_int = "test_id_int";
    std::string test_id_bool = "test_id_bool";
    std::string test_id_string = "test_id_string";
    std::string test_id_double = "test_id_double";

    auto testDataInt = std::make_shared<BasicDataAdapter<int>>(1);   
    auto testDataBool = std::make_shared<BasicDataAdapter<bool>>(0);   
    auto testDataString = std::make_shared<BasicDataAdapter<std::string>>("14");   
    auto testDataDouble = std::make_shared<BasicDataAdapter<double>>(8.8);   
    auto testDataInt2 = std::make_shared<BasicDataAdapter<int>>(98);

    bb_test.add_data(test_id_int,testDataInt);
    bb_test.add_data(test_id_bool,testDataBool);
    bb_test.add_data(test_id_string,testDataString);
    bb_test.add_data(test_id_double,testDataDouble);

    EXPECT_EQ(bb_test.size(),4) << "\nTEST_MESSAGE: incorrect size";

    bb_test.remove_data(test_id_double);

    EXPECT_EQ(bb_test.size(),3) << "\nTEST_MESSAGE: failed to delete one item from Blackboard";

    bb_test.modify_data(test_id_int,testDataInt2);

    EXPECT_EQ(bb_test.size(),3) << "\nTEST_MESSAGE: ???";

    bb_test.remove_data(test_id_int);
    bb_test.remove_data(test_id_bool);
    bb_test.remove_data(test_id_string);

    EXPECT_EQ(bb_test.size(),0) << "\nTEST_MESSAGE: failed to delete all item from Blackboard";
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
