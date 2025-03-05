
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
        class MarkerVisible : public uavsdk::command_manager::SingleProccessCommandInterface, public uavsdk::command_manager::IBlackboard, public uavsdk::command_manager::IIdentification<std::string>
        {
            public:
            MarkerVisible(std::shared_ptr<useful_di::UniMapStr> _blackboard) 
                : uavsdk::command_manager::IBlackboard(_blackboard)
            {
                this->set_id("marker_visible");
            }


            ExecutionResult logic_tick() override
            {
                auto marker_visible = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<bool>>("marker_visible")->get_data();
                if (marker_visible)
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