#pragma once

#include <thread>
#include <memory>
#include <future>
#include <iostream>
#include <functional>

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
            SUCCESS = 0,
            FAILED = 1,
        };


        /**
         * @brief Interface for command implementations.
         *
         * The BaseCommandInterface provides an abstraction layer for implementing commands in the UAVSDK.
         * It manages thread execution, stopping, and resource management. Commands must implement specific logic through pure virtual methods.
         * 
         * Methods that need to be implemented:
         * 
         * - void logic_tick()
         * - void handle_stop()
         */
        class BaseCommandInterface
        {
            public:
            BaseCommandInterface()
            {
                stopper = std::make_shared<std::stop_source>();
                tick_rate_ms = 100;
            }


            /**
             * @brief Gets a future representing the command's execution result.
             *
             * The future will be completed with either SUCCESS or FAILED when the command finishes.
             */
            std::future<ExecutionResult> get_result_future()
            {
                return this->result_promise.get_future();
            }


            /**
             * @brief Starts executing the command.
             *
             * If a command is already running (exists in execution_thread), returns ALREADY_RUNNING.
             * Otherwise, starts a new thread and returns STARTED.
             */
            StartExecutionResult execute()
            {
                if (execution_thread)
                {
                    return StartExecutionResult::ALREADY_RUNNING;
                }
                else
                {
                    execution_thread = std::make_shared<std::jthread>(std::bind(&BaseCommandInterface::_loop, this));
                    return StartExecutionResult::STARTED;
                }
            }


            /**
             * @brief Stops the currently executing command. 
             *
             * @param result execution result
             * 
             * Initiates a stop request and waits for the command to terminate.
             * Sets the result to FAILED upon completion by default.
             */
            void stop(ExecutionResult result=ExecutionResult::FAILED)
            {
                try 
                {std::scoped_lock lock(command_mutex);
                    stopper->request_stop();
                    execution_thread->join();
                    this->execution_thread = nullptr;
                    std::cout << "BaseCommandInterface: DESTROYED COMMAND\n";
                }
                catch (std::exception& e)
                {
                    std::cout << "BaseCommandInterface: FAILED TO DESTROY COMMAND: " << e.what() << "\n";
                }
                this->result_promise.set_value(result);
            }

            protected:
            /**
             * @brief Pure virtual method to be implemented by child classes.
             *
             * Contains the core logic of command execution, called periodically.
             */
            virtual void logic_tick() = 0;

            /**
             * @brief Pure virtual method to be implemented by child classes.
             *
             * Handles stopping the command and cleaning up resources.
             */
            virtual void handle_stop() = 0;

            private:
            unsigned int tick_rate_ms;
            std::promise<ExecutionResult> result_promise;
            std::shared_ptr<std::stop_source> stopper;
            std::shared_ptr<std::jthread> execution_thread;
            std::mutex command_mutex;
            // Id id;
            

            /**
             * @brief Main loop for command execution.
             *
             * Monitors the stopper and calls periodic logic tick.
             */
            void _loop()
            {
                while (true)
                {
                    if (stopper->stop_requested())
                    {
                        this->handle_stop();
                    }
                    _tick();
                }
            }


            /**
             * @brief Periodically executes core logic of the command.
             *
             * Calls logic_tick() and waits for the next tick based on tick_rate_ms.
             */
            void _tick()
            {
                this->logic_tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(this->tick_rate_ms));
            }
        };


        template <typename Id>
        class CommandInterfaceWithId : public BaseCommandInterface
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
    };
};