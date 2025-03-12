#pragma once

#include <thread>
#include <memory>
#include <future>
#include <iostream>
#include <string>
#include <functional>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/command_manager/behaviour_interfaces.hpp>
#include <uavsdk/command_manager/executors.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        class StagedCommandInterface : public SingleProccessCommandInterface
        {
            public:
            void set_tree_postfix(std::string postfix)
            {
                this->set_postfix(postfix);

                for (size_t i = 0; i < stages.size(); i++)
                {
                    stages.at(i)->set_postfix(postfix);
                }
            }


            virtual void add_stage(std::shared_ptr<SingleProccessCommandInterface> new_stage)
            {
                stages.push_back(new_stage);
            }


            void set_execution_strategy(std::shared_ptr<uavsdk::command_manager::executors::IExecutionStrategy> new_exec_strat)
            {
                this->execution_strategy = new_exec_strat;
            }


            virtual void initialize() override 
            {
                this->last_result = ExecutionResult::RUNNING;
                for (const auto& stage : this->stages)
                {
                    stage->initialize();
                }
                std::cout << typeid(*this).name() << " was initialized\n";
            }


            virtual std::shared_ptr<useful_di::UniMapStr> get_state() override
            {
                auto state = std::make_shared<useful_di::UniMapStr>();
            
                auto name = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::string>>(this->get_id());
                auto last_state = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<ExecutionResult>>(this->get_last_execution_result());
                auto child_nodes = std::make_shared<useful_di::UniMapStr>();
                auto child_num = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<size_t>>(this->stages.size());
                
                std::string node_type = "action";

                if (execution_strategy->___get_type() == utils::cppext::get_type<uavsdk::command_manager::executors::SequentialExecutionStrategy>()) node_type = "sequence";
                else if (execution_strategy->___get_type() == utils::cppext::get_type<uavsdk::command_manager::executors::FallbackExecutionStrategy>()) node_type = "fallback";
                else if (execution_strategy->___get_type() == utils::cppext::get_type<uavsdk::command_manager::executors::ParallelStrictExecutionStrategy>()) node_type = "parallel_strict";
                else if (execution_strategy->___get_type() == utils::cppext::get_type<uavsdk::command_manager::executors::ParallelHopefulExecutionStrategy>()) node_type = "parallel_hopeful";

                auto node_type_adap = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::string>>(node_type);

                // for (const auto& stage : this->stages)
                for(size_t i = 0; i < this->stages.size(); i++)
                {
                    auto data_pair = std::make_pair<std::string, std::shared_ptr<useful_di::UniMapStr>>(stages.at(i)->get_id(), stages.at(i)->get_state());
                    auto data = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::pair<std::string, std::shared_ptr<useful_di::UniMapStr>>>>(data_pair);
                    child_nodes->add_data(std::to_string(i), data);
                }
                
                std::vector<bool> exec_stat = this->execution_strategy->get_last_was_executed_stage_status();
                auto exec_stat_adap = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::vector<bool>>>(exec_stat);

                state->add_data("name", name);
                state->add_data("last_state", last_state);
                state->add_data("child_nodes", child_nodes);
                state->add_data("child_num", child_num);
                state->add_data("child_exec_stat", exec_stat_adap);
                state->add_data("node_type", node_type_adap);
                
                return state;
            }


            protected:
            std::shared_ptr<uavsdk::command_manager::executors::IExecutionStrategy> execution_strategy;
            std::vector<std::shared_ptr<SingleProccessCommandInterface>> stages;
            std::shared_future<std::shared_ptr<useful_di::TypeInterface>> current_stage_res_future;


            virtual ExecutionResult logic_tick() override
            {
                auto res = this->execution_strategy->execute_stages(this->stages);
                
            }
        };
    };
};