
#pragma once
#include <memory>
#include <thread>
#include <mutex>

#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        class MarkerSearchIsSetUp : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
        {
            public:
            MarkerSearchIsSetUp(std::shared_ptr<useful_di::UniMapStr> _blackboard) 
                : uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>(_blackboard)
            {
                this->set_id("marker_search_is_set_up");
            }


            ExecutionResult logic_tick() override
            {
                auto marker_search_is_set_up = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<bool>>("marker_search_is_set_up")->get_data();
                if (marker_search_is_set_up)
                {
                    return ExecutionResult::SUCCESS;
                }
                else
                {
                    return ExecutionResult::FAILED;
                }
            }
        };
    }
}