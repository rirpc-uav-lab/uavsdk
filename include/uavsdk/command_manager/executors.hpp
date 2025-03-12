#pragma once

#include <thread>
#include <memory>
#include <future>
#include <iostream>
#include <string>
#include <functional>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/command_manager/behaviour_interfaces.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        namespace executors
        {
            class IExecutionStrategy : public useful_di::TypeInterface
            {
                public:
                virtual uavsdk::command_manager::ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) = 0;
                virtual std::vector<bool> get_last_was_executed_stage_status() { return this->last_was_executed_stage_status; }
                
                protected:
                std::vector<bool> last_was_executed_stage_status;
            };


            // All stages must return SUCCESS execution result
            class SequentialExecutionStrategy : public IExecutionStrategy
            {
                public:
                SequentialExecutionStrategy()
                {
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult execute_stages(std::vector<std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface>>& stages) override
                {
                    if (stages.empty())
                    {
                        throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                    }
                    else
                    {
                        this->last_was_executed_stage_status.resize(stages.size());

                        for(size_t i = 0; i < this->last_was_executed_stage_status.size(); i++)
                        {
                            this->last_was_executed_stage_status.at(i) = false;
                        }

                        uavsdk::command_manager::ExecutionResult result = uavsdk::command_manager::ExecutionResult::INVALID;
                        if (stages.at(stage_index)->get_last_execution_result() == uavsdk::command_manager::ExecutionResult::RUNNING)
                        {
                            result = stages.at(stage_index)->tick();
                            this->last_was_executed_stage_status.at(stage_index) = true;
                        }
                        if (result == uavsdk::command_manager::ExecutionResult::SUCCESS)
                        {
                            stage_index++;
                            if (stage_index == stages.size())
                            {
                                stage_index = 0;
                                return uavsdk::command_manager::ExecutionResult::SUCCESS;
                            }
                            return uavsdk::command_manager::ExecutionResult::RUNNING;
                        }
                        
                        if (result == uavsdk::command_manager::ExecutionResult::FAILED)
                        {
                            stage_index = 0;
                        }

                        return result;
                    }
                }


                private:
                size_t stage_index = 0;
            };


            class FallbackExecutionStrategy : public IExecutionStrategy
            {
                public:
                FallbackExecutionStrategy()
                {
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult execute_stages(std::vector<std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface>>& stages) override
                {
                    if (stages.empty())
                    {
                        throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                    }
                    else
                    {
                        this->last_was_executed_stage_status.resize(stages.size());

                        for(size_t i = 0; i < this->last_was_executed_stage_status.size(); i++)
                        {
                            this->last_was_executed_stage_status.at(i) = false;
                        }

                        uavsdk::command_manager::ExecutionResult result = uavsdk::command_manager::ExecutionResult::INVALID;
                        if (stages.at(stage_index)->get_last_execution_result() == uavsdk::command_manager::ExecutionResult::RUNNING)
                        {
                            result = stages.at(stage_index)->tick();
                            this->last_was_executed_stage_status.at(stage_index) = true;
                        }
                        if (result == uavsdk::command_manager::ExecutionResult::FAILED)
                        {
                            stage_index++;

                            if (stage_index == stages.size())
                            {
                                stage_index = 0;
                                return uavsdk::command_manager::ExecutionResult::FAILED;
                            }
                            return uavsdk::command_manager::ExecutionResult::RUNNING;
                        }

                        if (result == uavsdk::command_manager::ExecutionResult::SUCCESS)
                        {
                            stage_index = 0;
                        }

                        return result;
                    }
                }

                private:
                size_t stage_index = 0;
            };


            class ParallelStrictExecutionStrategy : public IExecutionStrategy
            {
                public:
                ParallelStrictExecutionStrategy()
                {
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult execute_stages(std::vector<std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface>>& stages) override
                {
                    if (stages.empty())
                    {
                        throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                    }

                    if (stage_results.empty()) stage_results.resize(stages.size());

                    this->last_was_executed_stage_status.resize(stages.size());

                    for(size_t i = 0; i < this->last_was_executed_stage_status.size(); i++)
                    {
                        this->last_was_executed_stage_status.at(i) = false;
                    }

                    bool all_sucess = true;

                    for (size_t i = 0; i < stages.size(); i++)
                    {
                        if (stage_results.at(i) == ExecutionResult::SUCCESS or stage_results.at(i) == ExecutionResult::FAILED)
                        {
                            stages.at(i)->initialize();
                        }

                        stage_results.at(i) = stages.at(i)->tick();
                        this->last_was_executed_stage_status.at(i) = true;

                        if (stage_results.at(i) == uavsdk::command_manager::ExecutionResult::RUNNING)
                        {
                            all_sucess = false;
                        }

                        if (stage_results.at(i) == uavsdk::command_manager::ExecutionResult::FAILED)
                        {
                            for (const auto& stage : stages)
                            {
                                stage->stop();
                            }
                            return uavsdk::command_manager::ExecutionResult::FAILED;
                        }
                    }

                    if (all_sucess) return uavsdk::command_manager::ExecutionResult::SUCCESS;
                    else return uavsdk::command_manager::ExecutionResult::RUNNING;
                }

                private:
                std::vector<uavsdk::command_manager::ExecutionResult> stage_results;
            };


            class ParallelHopefulExecutionStrategy : public IExecutionStrategy
            {
                public:
                ParallelHopefulExecutionStrategy()
                {
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult execute_stages(std::vector<std::shared_ptr<uavsdk::command_manager::SingleProccessCommandInterface>>& stages) override
                {
                    if (stages.empty())
                    {
                        throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                    }

                    if (stage_results.empty()) stage_results.resize(stages.size());

                    this->last_was_executed_stage_status.resize(stages.size());

                    for(size_t i = 0; i < this->last_was_executed_stage_status.size(); i++)
                    {
                        this->last_was_executed_stage_status.at(i) = false;
                    }

                    bool all_failed = true;

                    for (size_t i = 0; i < stages.size(); i++)
                    {
                        if (stage_results.at(i) == ExecutionResult::SUCCESS or stage_results.at(i) == ExecutionResult::FAILED)
                        {
                            stages.at(i)->initialize();
                        }

                        stage_results.at(i) = stages.at(i)->tick();
                        this->last_was_executed_stage_status.at(i) = true;

                        if (stage_results.at(i) == uavsdk::command_manager::ExecutionResult::RUNNING)
                        {
                            all_failed = false;
                        }

                        if (stage_results.at(i) == uavsdk::command_manager::ExecutionResult::SUCCESS)
                        {
                            for (const auto& stage : stages)
                            {
                                stage->stop();
                            }
                            return uavsdk::command_manager::ExecutionResult::SUCCESS;
                        }
                    }

                    if (all_failed) return uavsdk::command_manager::ExecutionResult::FAILED;
                    else return uavsdk::command_manager::ExecutionResult::RUNNING;
                }

                private:
                std::vector<uavsdk::command_manager::ExecutionResult> stage_results;
            };
        };


    };
};