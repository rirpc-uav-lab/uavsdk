
#include <string>
#include <memory>

#include <gtest/gtest.h>

#include "uavsdk/data_adapters/cxx/cxx.hpp"

#include "uavsdk/useful_data_lib/useful_data_impl.hpp"

using namespace useful_di;
using uavsdk::data_adapters::cxx::MutexDefendedDataAdapter;


TEST(useful_data_imp, BlackboardVariableCreateWithInitialValue)
{
    std::shared_ptr<Blackboard> bb = std::make_shared<Blackboard>();
    std::string test_key = "id";
    int test_data = 1;

    BlackboardVariable<int> var(bb, test_key, test_data);


    auto v1 = *var;
    auto v2 = bb->at<MutexDefendedDataAdapter<int>>(test_key)->get_data();

    // FAIL() << bb->get_keys_from_blackboard().at(0);
    // FAIL() << bb->has(test_key);

    ASSERT_EQ(test_data, v2) << "TEST_MESSAGE: Blackboard::at<T>() did not return a valid value.";
    EXPECT_EQ(v1, v2) << "TEST_MESSAGE: BlackboardVariable::() did not return a valid value.";
}



TEST(useful_data_imp, BlackboardVariableCreateWithoutInitialValue)
{
    std::shared_ptr<Blackboard> bb = std::make_shared<Blackboard>();
    std::string test_key = "id";
    int test_data = 1;

    EXPECT_ANY_THROW(BlackboardVariable<int> var(bb, test_key)) << "TEST_MESSAGE: BlackboardVariable constructed correctly without the initial value and the variable was not present in the Blackboard.";

    auto test_data_adap = std::make_shared<MutexDefendedDataAdapter<int>>(test_data);
    bb->add_data(test_key, test_data_adap);

    ASSERT_NO_THROW(BlackboardVariable<int> var(bb, test_key)) << "TEST_MESSAGE: BlackboardVariable threw in the constructor without the initial value and the variable was present in the Blackboard.";

    BlackboardVariable<int> var(bb, test_key, test_data);

    auto v1 = *var;
    auto v2 = bb->at<MutexDefendedDataAdapter<int>>(test_key)->get_data();

    ASSERT_EQ(test_data, v2) << "TEST_MESSAGE: Blackboard::at<T>() did not return a valid value.";
    EXPECT_EQ(v1, v2) << "TEST_MESSAGE: BlackboardVariable::() did not return a valid value.";
}



TEST(useful_data_imp, BlackboardVariableSet)
{
    std::shared_ptr<Blackboard> bb = std::make_shared<Blackboard>();
    std::string test_key = "id";
    int test_data = 1;

    BlackboardVariable<int> var(bb, test_key, test_data);


    auto v1 = *var;
    auto v2 = bb->at<MutexDefendedDataAdapter<int>>(test_key)->get_data();

    ASSERT_EQ(test_data, v2) << "TEST_MESSAGE: Blackboard::at<T>() did not return a valid value.";
    EXPECT_EQ(v1, v2) << "TEST_MESSAGE: BlackboardVariable::() did not return a valid value.";

    var = 2;
    v1 = *var;

    EXPECT_EQ(v1, 2) << "TEST_MESSAGE: BlackboardVariable::() did not change the value correctly.";
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
