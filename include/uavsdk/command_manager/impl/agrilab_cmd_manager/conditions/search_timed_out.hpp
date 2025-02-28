
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
        class SearchTimedOut : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
        {
            public:
            SearchTimedOut(std::shared_ptr<useful_di::UniMapStr> _blackboard) 
                : uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>(_blackboard)
            {
                this->set_id("search_timed_out");
            }


            ExecutionResult logic_tick() override
            {
                auto search_timed_out = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<bool>>("search_timed_out")->get_data();
                if (search_timed_out)
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