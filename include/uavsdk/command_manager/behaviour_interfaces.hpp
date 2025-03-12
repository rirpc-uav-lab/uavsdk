#pragma once

#include <memory>
#include <future>
#include <iostream>
#include <string>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        enum class StartExecutionResult
        {
            STARTED = 0,
            ALREADY_RUNNING = 1,
        };


        enum class ExecutionResult
        {
            INVALID,
            RUNNING,
            SUCCESS,
            FAILED,
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


        class IdString : public IIdentification<std::string>
        {
            public:
            void set_postfix(std::string postfix)
            {
                this->id += postfix;
            }
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


        class IStateManager
        {
            public:
            ExecutionResult get_last_execution_result()
            {
                return this->last_result;
            }


            virtual std::shared_ptr<useful_di::UniMapStr> get_state() = 0;


            protected:
            ExecutionResult last_result = ExecutionResult::INVALID;
        };


        class BaseCommandInterface : public IExecutable, public IResultProvider, public IStoppable, public IInitializable, public useful_di::TypeInterface, public IStateManager
        {
            protected:
            std::shared_ptr<useful_di::Blackboard> blackboard;


            /**
             * @brief Periodically executes core logic of the command.
             *
             * Calls logic_tick() and waits for the next tick based on tick_rate_ms.
             */
            ExecutionResult _tick()
            {
                std::cout << this->___get_type() << ": tick()\n"; 
                auto res = this->logic_tick();
                std::string msg;
                
                if (res == uavsdk::command_manager::ExecutionResult::FAILED) msg = "FAILED";
                if (res == uavsdk::command_manager::ExecutionResult::SUCCESS) msg = "SUCCESS";
                if (res == uavsdk::command_manager::ExecutionResult::RUNNING) msg = "RUNNING";
                if (res == uavsdk::command_manager::ExecutionResult::INVALID) msg = "INVALID";

                std::cout << this->___get_type() << " result was = " << msg << "\n";

                return res;
            }


            virtual void handle_stop()
            {
                this->initialize();
            }
        };


        class SingleProccessCommandInterface : public BaseCommandInterface, public uavsdk::command_manager::IdString
        {
            public:
            SingleProccessCommandInterface()
            {
                this->initialize();
            }


            virtual std::shared_ptr<useful_di::UniMapStr> get_state() override
            {
                auto state = std::make_shared<useful_di::UniMapStr>();
            
                auto name = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::string>>(this->get_id());
                auto last_state = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<ExecutionResult>>(this->get_last_execution_result());
                auto child_num = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<size_t>>(0);
                
                std::string node_type = "action";

                auto node_type_adap = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<std::string>>(node_type);
                state->add_data("node_type", node_type_adap);


                state->add_data("name", name);
                state->add_data("last_state", last_state);
                state->add_data("child_num", child_num);

                return state;
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


            virtual void initialize() override 
            {
                this->last_result = ExecutionResult::RUNNING;
                std::cout << typeid(*this).name() << " was initialized\n";
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
    }
}