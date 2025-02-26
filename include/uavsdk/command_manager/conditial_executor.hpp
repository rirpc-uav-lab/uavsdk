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
        class ConditionalExecutor : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
        {
            public:
            ConditionalExecutor(std::shared_ptr<useful_di::UniMapStr> _blackboard, 
                std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> _condition, 
                std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> _true_cmd, 
                std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> _false_cmd ) 
                : uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>(_blackboard)
            {
                this->condition = _condition;
                this->true_cmd = _true_cmd;
                this->false_cmd = _false_cmd;
            }
            ExecutionResult logic_tick() override
            {
                if (condition->logic_tick() == ExecutionResult::SUCCESS)
                {
                    return true_cmd->logic_tick();
                }
                if (condition->logic_tick() == ExecutionResult::FAILED)
                {
                    return false_cmd->logic_tick();
                }
                if (condition->logic_tick() == ExecutionResult::RUNNING)
                {
                    return ExecutionResult::RUNNING;
                }
            }

            protected:
            std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> condition;
            std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> true_cmd;
            std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface> false_cmd;            
        };
    }
}