#pragma once

#include <thread>
#include <memory>
#include <future>
#include <iostream>
#include <string>
#include <functional>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        enum StartExecutionResult
        {
            STARTED = 0,
            ALREADY_RUNNING = 1,
        };


        enum ExecutionResult
        {
            RUNNING,
            SUCCESS,
            FAILED,
        };


        enum CommandImplementationChoice
        {
            SINGLE_THREADED,
            MULTI_THREADED,
        };


        // /**
        //  * @brief Interface for command implementations.
        //  *
        //  * The BaseCommandInterface provides an abstraction layer for implementing commands in the UAVSDK.
        //  * It manages thread execution, stopping, and resource management. Commands must implement specific logic through pure virtual methods.
        //  * 
        //  * Methods that need to be implemented:
        //  * 
        //  * - void logic_tick()
        //  * - void handle_stop()
        //  */
        // class BaseCommandInterface : public useful_di::TypeInterface
        // {
        //     public:
        //     BaseCommandInterface()
        //     {
        //         // tick_rate_ms = 10;
        //     }


        //     /**
        //      * @brief Gets a future representing the command's execution result.
        //      *
        //      * The future will be completed with either SUCCESS or FAILED when the command finishes.
        //      */
        //     virtual std::shared_future<std::shared_ptr<useful_di::TypeInterface>> get_result_future()
        //     {
        //         try
        //         {
        //             res_future = this->result_promise.get_future();
        //             return res_future.share();
        //         }
        //         catch (std::exception e)
        //         {
        //             std::string msg = "Tried to get command result future more than one time: " + std::string(e.what()) + "\n";
        //             throw std::runtime_error(msg);
        //         }
        //     }


        //     protected:
        //     std::promise<std::shared_ptr<useful_di::TypeInterface>> result_promise;
        //     std::future<std::shared_ptr<useful_di::TypeInterface>> res_future;


        //     /**
        //      * @brief Pure virtual method to be implemented by child classes.
        //      *
        //      * Contains the core logic of command execution, called periodically.
        //      */
        //     virtual ExecutionResult logic_tick() = 0;

        //     /**
        //      * @brief Pure virtual method to be implemented by child classes.
        //      *
        //      * Handles stopping the command and cleaning up resources.
        //      */
        //     virtual void handle_stop() = 0;


        //     /**
        //      * @brief Periodically executes core logic of the command.
        //      *
        //      * Calls logic_tick() and waits for the next tick based on tick_rate_ms.
        //      */
        //     ExecutionResult _tick()
        //     {
        //         auto res = this->logic_tick();
        //         std::string msg;
                
        //         if (res == uavsdk::command_manager::ExecutionResult::FAILED) msg = "FAILED";
        //         if (res == uavsdk::command_manager::ExecutionResult::SUCCESS) msg = "SUCCESS";
        //         if (res == uavsdk::command_manager::ExecutionResult::RUNNING) msg = "RUNNING";

        //         std::cout << this->___get_type() << ": tick() res = " << msg << "\n";
        //         return res;
        //     }
        // };


        class IExecutable
        {
            public:
            virtual ExecutionResult logic_tick() = 0;
        };


        class IResultProvider
        {
            public:
            /**
             * @brief Gets a future representing the command's execution result.
             *
             * The future will be completed with either SUCCESS or FAILED when the command finishes.
             */
            virtual std::shared_future<std::shared_ptr<useful_di::TypeInterface>> get_result_future()
            {
                try
                {
                    res_future = this->result_promise.get_future();
                    return res_future.share();
                }
                catch (std::exception e)
                {
                    std::string msg = "Tried to get command result future more than one time: " + std::string(e.what()) + "\n";
                    throw std::runtime_error(msg);
                }
            }


            protected:
            std::promise<std::shared_ptr<useful_di::TypeInterface>> result_promise;
            std::future<std::shared_ptr<useful_di::TypeInterface>> res_future;
        };


        class IStoppable 
        {
            public:
            virtual void handle_stop() = 0; 
        };


        class BaseCommandInterface : public IExecutable, public IResultProvider, public IStoppable, public useful_di::TypeInterface
        {
            protected:
            /**
             * @brief Periodically executes core logic of the command.
             *
             * Calls logic_tick() and waits for the next tick based on tick_rate_ms.
             */
            ExecutionResult _tick()
            {
                auto res = this->logic_tick();
                std::string msg;
                
                if (res == uavsdk::command_manager::ExecutionResult::FAILED) msg = "FAILED";
                if (res == uavsdk::command_manager::ExecutionResult::SUCCESS) msg = "SUCCESS";
                if (res == uavsdk::command_manager::ExecutionResult::RUNNING) msg = "RUNNING";

                std::cout << this->___get_type() << ": tick() res = " << msg << "\n";
                return res;
            }
        };


        class SingleProccessCommandInterface : public BaseCommandInterface
        {
            public:
            ExecutionResult tick()
            {
                if (not stop_requested)
                {
                    return this->_tick();
                }
                else
                {
                    this->handle_stop();
                }
            }
            
            
            void stop(std::string debug="")
            {
                stop_requested = true;

                if (debug != "")
                {
                    std::cout << "stop debug: " << debug << "\n";
                }
            }


            private:
            bool stop_requested = false;
        };



        class IExecutionStrategy
        {
            public:
            virtual ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) = 0;
        };


        // All stages must return SUCCESS execution result
        class SequentialExecutionStrategy : public IExecutionStrategy
        {
            public:
            ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) override
            {
                if (stages.empty())
                {
                    throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                }
                else
                {
                    ExecutionResult result = stages.at(stage_index)->tick();
                    if (result == ExecutionResult::SUCCESS)
                    {
                        stage_index++;
                        if (stage_index == stages.size())
                        {
                            stage_index = 0;
                            return ExecutionResult::SUCCESS;
                        }
                        return ExecutionResult::RUNNING;
                    }
                    
                    if (result == ExecutionResult::FAILED)
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
            ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) override
            {
                if (stages.empty())
                {
                    throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                }
                else
                {
                    ExecutionResult result = stages.at(stage_index)->tick();
                    if (result == ExecutionResult::FAILED)
                    {
                        stage_index++;

                        if (stage_index == stages.size())
                        {
                            stage_index = 0;
                            return ExecutionResult::FAILED;
                        }
                        return ExecutionResult::RUNNING;
                    }

                    if (result == ExecutionResult::SUCCESS)
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
            ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) override
            {
                if (stages.empty())
                {
                    throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                }

                if (stage_results.empty()) stage_results.resize(stages.size());

                bool all_sucess = true;

                for (size_t i = 0; i < stages.size(); i++)
                {
                    if (stage_results.at(i) != ExecutionResult::SUCCESS)
                    {
                        stage_results.at(i) = stages.at(i)->tick();
                    }

                    if (stage_results.at(i) == ExecutionResult::RUNNING)
                    {
                        all_sucess = false;
                    }

                    if (stage_results.at(i) == ExecutionResult::FAILED)
                    {
                        for (const auto& stage : stages)
                        {
                            stage->stop();
                        }
                        return ExecutionResult::FAILED;
                    }
                }

                if (all_sucess) return ExecutionResult::SUCCESS;
                else return ExecutionResult::RUNNING;
            }

            private:
            std::vector<ExecutionResult> stage_results;
        };


        class ParallelHopefulExecutionStrategy : public IExecutionStrategy
        {
            public:
            ExecutionResult execute_stages(std::vector<std::shared_ptr<SingleProccessCommandInterface>>& stages) override
            {
                if (stages.empty())
                {
                    throw std::runtime_error("SequentialExecutionStrategy: stages vector is empty.");
                }

                if (stage_results.empty()) stage_results.resize(stages.size());

                bool all_failed = true;

                for (size_t i = 0; i < stages.size(); i++)
                {
                    if (stage_results.at(i) != ExecutionResult::FAILED)
                    {
                        stage_results.at(i) = stages.at(i)->tick();
                    }

                    if (stage_results.at(i) == ExecutionResult::RUNNING)
                    {
                        all_failed = false;
                    }

                    if (stage_results.at(i) == ExecutionResult::SUCCESS)
                    {
                        for (const auto& stage : stages)
                        {
                            stage->stop();
                        }
                        return ExecutionResult::SUCCESS;
                    }
                }

                if (all_failed) return ExecutionResult::FAILED;
                else return ExecutionResult::RUNNING;
            }

            private:
            std::vector<ExecutionResult> stage_results;
        };


        class StagedCommandInterface : public SingleProccessCommandInterface
        {
            public:
            virtual void add_stage(std::shared_ptr<SingleProccessCommandInterface> new_stage)
            {
                stages.push_back(new_stage);
            }


            void set_execution_strategy(std::shared_ptr<IExecutionStrategy> new_exec_strat)
            {
                this->execution_strategy = new_exec_strat;
            }


            protected:
            std::shared_ptr<IExecutionStrategy> execution_strategy;
            std::vector<std::shared_ptr<SingleProccessCommandInterface>> stages;
            std::shared_future<std::shared_ptr<useful_di::TypeInterface>> current_stage_res_future;


            virtual ExecutionResult logic_tick() override
            {
                // if (!stages.empty())
                // {

                //     ExecutionResult subres = this->stages.at(0)->tick();

                //     if (subres == ExecutionResult::SUCCESS)
                //     {
                //         this->stages.erase(this->stages.begin());
                //         return ExecutionResult::RUNNING;
                //     }
                //     else
                //     {
                //         return subres;
                //     }
                // }
                // else
                // {
                //     this->stop();
                //     return ExecutionResult::SUCCESS;
                // }
                return this->execution_strategy->execute_stages(this->stages);
            }
        };


        template <typename Id>
        class CommandInterfaceWithId : public StagedCommandInterface
        {
            public:

            /**
             * @brief Sets the command's ID.
             *
             * Allows assigning an unique identifier to the command.
             */
            void set_id(Id new_id)
            {
                this->id = new_id;
            }


            /**
             * @brief Retrieves the current command ID.
             *
             * Returns the assigned command ID.
             */
            Id get_id()
            {
                return this->id;
            }

            protected:
            Id id;
        };


        /**
         * Protected methods that need to be implemented (pure-virtual):
         * 
         * - void logic_tick()
         * 
         * - void handle_stop()
         */
        template <typename Id, typename ExternalResource, typename CommandData>
        class CommandInterface : public CommandInterfaceWithId<Id>
        {
            public:
            // std::shared_ptr<useful_di::UniMapStr> get_blackboard_pointer()
            // {
            //     return blackboard;
            // }
            void set_external_resource(std::shared_ptr<ExternalResource> new_ext)
            {
                external_resource = new_ext;
            }


            std::shared_ptr<ExternalResource> get_external_resource()
            {
                return external_resource;
            }


            void set_command_data(std::shared_ptr<CommandData> new_cmd_data)
            {
                command_data = new_cmd_data;
            }


            std::shared_ptr<CommandData> get_command_data()
            {
                return command_data;
            }

            protected:
            std::shared_ptr<ExternalResource> external_resource;
            std::shared_ptr<CommandData> command_data;
        };


        template <typename Id, typename ExternalResource, typename CommandData>
        class CommandInterfaceWithBlackboard : public CommandInterface<Id, ExternalResource, CommandData>
        {
            public:
            CommandInterfaceWithBlackboard(std::shared_ptr<useful_di::UniMapStr> _blackboard)
            {
                init_blackboard(_blackboard);
                this->execution_strategy = std::make_shared<SequentialExecutionStrategy>();
            }

            void add_data_to_bb(std::string key, std::shared_ptr<useful_di::TypeInterface> data)
            {
                std::scoped_lock llock(bb_mutex);

                auto keys = this->blackboard->get_present_keys();
                
                bool key_not_in_bb = true;
                
                for (const auto& bb_key : keys)
                {
                    if (bb_key == key)
                    {
                        key_not_in_bb = false;
                        break;
                    }
                }
                
                if (key_not_in_bb)
                {
                    blackboard->add_data(key, data);
                }
                else
                {
                    std::runtime_error("Called add_data_to_bb(key, data), but this key is alredy in blackboard");
                }
            }


            std::vector<std::string> get_keys_from_blackboard()
            {
                std::scoped_lock llock(bb_mutex);
                return blackboard->get_present_keys();
            }


            void remove_data_from_blackboard(std::string key)
            {
                std::scoped_lock llock(bb_mutex);
                blackboard->remove_data(key);
            }


            void modify_data_on_blackboard(std::string key, std::shared_ptr<useful_di::TypeInterface> new_data)
            {
                std::scoped_lock llock(bb_mutex);
                blackboard->modify_data(key, new_data);
            }


            template <typename T>
            std::shared_ptr<T> bb_at(std::string key)
            {
                std::shared_ptr<useful_di::TypeInterface> data = this->blackboard->at(key);
                
                if (data->___get_type() == utils::cppext::get_type<T>())
                {
                    return std::dynamic_pointer_cast<T>(data);
                }
                else
                {
                    std::string msg = "CommandInterfaceWithBlackboard::at<T>(std::string key) error: tried conversion of type " + data->___get_type() + " to " + utils::cppext::get_type<T>() + "\n";
                    throw std::runtime_error(msg);
                }
            }


            size_t blackboard_size()
            {
                return blackboard->size();
            }


            protected:
            std::shared_ptr<useful_di::UniMapStr> get_bb_p()
            {
                return blackboard;
            }


            private:
            std::shared_ptr<useful_di::UniMapStr> blackboard; // shared_resource
            std::mutex bb_mutex; // blackboard mutex


            void init_blackboard(std::shared_ptr<useful_di::UniMapStr> init_bb)
            {
                this->blackboard = init_bb;
            }
        };
    };
};