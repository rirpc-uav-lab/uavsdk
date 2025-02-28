#pragma once
#include <memory>
#include <thread>
#include <mutex>

#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        class WhileNotSuccess : public uavsdk::command_manager::SingleProccessCommandInterface, public uavsdk::command_manager::IBlackboard, public uavsdk::command_manager::IIdentification<std::string>
        {
            public:
            WhileNotSuccess(std::shared_ptr<useful_di::UniMapStr> _blackboard, 
                std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> _node ) 
                : uavsdk::command_manager::IBlackboard(_blackboard)
            {
                this->node = _node;
                this->set_id("while_not_success");
            }
            ExecutionResult logic_tick() override
            {
                if (node->tick() != ExecutionResult::SUCCESS)
                {
                    return ExecutionResult::RUNNING;
                }
                else return ExecutionResult::SUCCESS;
            }

            protected:
            std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> node;           
        };
    }
}