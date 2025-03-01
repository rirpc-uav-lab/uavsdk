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
            INVALID,
            RUNNING,
            SUCCESS,
            FAILED,
        };


        enum CommandImplementationChoice
        {
            SINGLE_THREADED,
            MULTI_THREADED,
        };


        class IExecutable
        {
            public:
            virtual ExecutionResult logic_tick() = 0;
        };


        template <typename Id>
        class IIdentification
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


        class IInitializable
        {
            public:
            virtual void initialize() = 0;
        };


        class IBlackboard
        {
            public:
            IBlackboard(std::shared_ptr<useful_di::UniMapStr> _blackboard)
            {
                init_blackboard(_blackboard);
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
                    auto bb_type = blackboard->at(key)->___get_type();
                    auto data_type = data->___get_type();
                    if (bb_type != data_type) throw std::runtime_error("IBlackboard: OVERWITE ERROR! Called add_data_to_bb(key, data) on key" + key + ", but this key is alredy in blackboard with a different type. ");
                    else
                    {
                        blackboard->add_data(key, data);
                    }
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
                
                // std::cout << "bb at key = " << key << "\n";

                if (data->___get_type() == utils::cppext::get_type<T>())
                {
                    return std::dynamic_pointer_cast<T>(data);
                }
                else
                {
                    std::string msg = "CommandInterfaceWithBlackboard::at<T>(std::string key) error: tried conversion of data withe key " + key + " from type " + data->___get_type() + " to " + utils::cppext::get_type<T>() + "\n";
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


        class BaseCommandInterface : public IExecutable, public IResultProvider, public IStoppable, public IInitializable, public useful_di::TypeInterface
        {
            public:
            ExecutionResult get_last_execution_result()
            {
                return this->last_result;
            }


            protected:
            ExecutionResult last_result = ExecutionResult::INVALID;
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
                if (res == uavsdk::command_manager::ExecutionResult::INVALID) msg = "INVALID";

                std::cout << this->___get_type() << ": tick() res = " << msg << "\n";
                return res;
            }


            virtual void handle_stop()
            {
                this->initialize();
            }
        };


        class SingleProccessCommandInterface : public BaseCommandInterface
        {
            public:
            SingleProccessCommandInterface()
            {
                this->initialize();
            }


            ExecutionResult tick()
            {
                if (not stop_requested)
                {
                    if (this->last_result != ExecutionResult::RUNNING)
                    {
                        std::string msg = std::string(typeid(*this).name()) + " returned ExecutionResult::INVALID whic must not be possible. Check your code's logic\n";
                        throw std::runtime_error(msg);
                        return ExecutionResult::INVALID;
                    }
                    this->last_result = this->_tick();
                    return this->last_result;
                }
                else
                {
                    this->handle_stop();
                }
            }


            void initialize() override 
            {
                this->last_result = ExecutionResult::RUNNING;
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
    };
};