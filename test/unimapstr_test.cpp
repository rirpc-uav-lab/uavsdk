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

TEST(useful_data_imp, UniMapStrWorksTestKey)
{
    UniMapStr UMS_test;

    std::vector<std::string> test_key;
    
    std::string test_id = "test_id";
    auto testData = std::make_shared<TypeInterface>();

    EXPECT_NO_THROW(UMS_test.add_data(test_id,testData)) << "\nTEST_MESSAGE: add_data throw exeption\n";

    test_key = UMS_test.get_present_keys();
    EXPECT_FALSE(test_key.empty()) << "TEST_MESSAGE: key has not sent\n";
}

TEST(useful_data_imp, UniMapStrWorksModifaiData)
{
    UniMapStr UMS_test;

    std::string test_id = "test_id";
    
    auto testData = std::make_shared<TypeInterface>();
    auto testModifyData = std::make_shared<useful_di::TypeInterface>(); // узнать, как присвоить значение. В остальном тест готов

    EXPECT_NO_THROW(UMS_test.add_data(test_id,testData)) << "\nTEST_MESSAGE: add_data() throw exeption";
    EXPECT_NO_THROW(UMS_test.modify_data("test_id",testModifyData)) << "\nTEST_MESSAGE: modify_data() throw exeption";

    EXPECT_NE(testData, UMS_test.at(test_id)) << "\nTEST_MESSAGE: data has not changed";
}

TEST(useful_data_imp, UniMapStrWorksAtAndAddData)
{
    UniMapStr UMS_test;
    std::string test_id = "test_id";
    auto testData = std::make_shared<TypeInterface>();

    EXPECT_NO_THROW(UMS_test.add_data(test_id,testData)) << "\nTEST_MESSAGE: add_data() has throw exeption";
    EXPECT_NO_THROW(UMS_test.at(test_id)) << "\nTEST_MESSAGE: at() has throw exeption";

    EXPECT_EQ(testData,UMS_test.at(test_id)) << "\nTEST_MESSAGE: vars in at is not equel";
}

TEST(useful_data_imp, UniMapStrWorksRemoveData)
{
    UniMapStr UMS_test;
    std::string test_id = "test_id";
    auto testData = std::make_shared<TypeInterface>();

    EXPECT_NO_THROW(UMS_test.add_data(test_id,testData)) << "\nTEST_MESSAGE: add_data() has throw exeption";

    UMS_test.remove_data(test_id);

    EXPECT_ANY_THROW(UMS_test.at(test_id)) << "\nTEST_MESSAGE: data has not removed";

    EXPECT_NO_THROW(UMS_test.at(test_id)) << "\nTEST_MESSAGE: test of work capabillity system";
}

TEST(useful_data_imp, UniMapStrWorksSize)
{
    UniMapStr UMS_test;
    std::string test_id = "test_id";
    auto testData = std::make_shared<TypeInterface>();

    EXPECT_NO_THROW(UMS_test.add_data(test_id,testData)) << "\nTEST_MESSAGE: add_data() has throw exeption";

    EXPECT_TRUE(UMS_test.size()) << "UMS is empty";
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}





































// template <typename Type>
// std::map<std::string, bool> test_generic_data_storage(std::shared_ptr<useful_di::MapLikeDataStorageInterface<Type>> storage)
// {   
//     using uavsdk::data_adapters::cxx::BasicDataAdapter;

//     auto num = std::make_shared<BasicDataAdapter<double>>(321.03);

//     bool TEST_ADD_DATA_WORKS = false;
//     Type test_key = storage->add_data(num);
    
//     auto num2 = std::dynamic_pointer_cast<BasicDataAdapter<double>>(storage->at(test_key));

//     TEST_ADD_DATA_WORKS = num->get_data() == num2->get_data();

//     NUM_REAL_EQ = TEST_ADD_DATA_WORKS;

//     bool TEST_SIZE_WORKS = false;

//     size_t size = storage->size();
    
//     if (size == 1) 
//     TEST_SIZE_WORKS = true;


//     bool TEST_MODIFICATION_WORKS = false;
    
//     auto num_mod = std::make_shared<BasicDataAdapter<double>>(567.89);
//     auto num_orig = std::dynamic_pointer_cast<BasicDataAdapter<double>>(storage->at(test_key));
    
//     storage->modify_data(test_key, num_mod);

//     TEST_MODIFICATION_WORKS = (num_mod->get_data() != num_orig->get_data());

//     MODE_EQ = TEST_MODIFICATION_WORKS;

//     bool TEST_DATA_REMOVED = false;
//     storage->remove_data(test_key);

//     // try 
//     // {
//     //     auto num3 = storage->at(test_key);
//     // } 
//     // catch (std::exception e) 
//     // {
//     //     TEST_DATA_REMOVED = true;
//     //     EXECPTION_EXPECT = TEST_DATA_REMOVED;
//     //     DELETE_TEST = TEST_DATA_REMOVED;
//     // }

//     std::map<std::string, bool> results;
//     results.insert(std::make_pair<std::string, bool>("TEST_SIZE_WORKS", std::move(TEST_SIZE_WORKS)));

//     results.insert(std::make_pair<std::string, bool>("TEST_ADD_DATA_WORKS", std::move(TEST_ADD_DATA_WORKS)));

//     results.insert(std::make_pair<std::string, bool>("TEST_MODIFICATION_WORKS", std::move(TEST_MODIFICATION_WORKS)));
//     results.insert(std::make_pair<std::string, bool>("TEST_DATA_REMOVED", std::move(TEST_DATA_REMOVED)));
//     results.insert(std::make_pair<std::string, bool>("MOCK_FAIL", false));

//     return results;
// }
