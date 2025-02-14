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


        enum CommandImplementationChoice
        {
            SINGLE_THREADED,
            MULTI_THREADED,
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
                // tick_rate_ms = 10;
            }


            /**
             * @brief Gets a future representing the command's execution result.
             *
             * The future will be completed with either SUCCESS or FAILED when the command finishes.
             */
            virtual std::shared_future<uavsdk::command_manager::ExecutionResult> get_result_future()
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


            // virtual void set_tick_rate(unsigned int rate_ms)
            // {
            //     this->tick_rate_ms = rate_ms;
            // }


            protected:
            std::promise<ExecutionResult> result_promise;
            std::future<ExecutionResult> res_future;


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


            /**
             * @brief Periodically executes core logic of the command.
             *
             * Calls logic_tick() and waits for the next tick based on tick_rate_ms.
             */
            void _tick()
            {
                this->logic_tick();
            }
        };



        class SingleProccessCommandInterface : public BaseCommandInterface
        {
            public:
            void tick()
            {
                // std::cout <<"\nSingleProccessCommandInterface::tick()\n";
                if (not stop_requested)
                {
                    this->_tick();
                    // std::cout <<"\nSingleProccessCommandInterface::tick() -- not stop requested\n";
                }
                else
                {
                    // std::cout <<"\nSingleProccessCommandInterface::tick() -- stop requested\n";
                    this->handle_stop();
                }
            }
            
            
            void stop(ExecutionResult result=ExecutionResult::FAILED, std::string debug="")
            {

                std::string res;
                
                if (result == ExecutionResult::FAILED) res = "failed";
                else res = "success";
                
                std::cout <<"\nSingleProccessCommandInterface::stop() with result: " + res + "\n";
                this->result_promise.set_value(result);
                stop_requested = true;

                if (debug != "")
                {
                    std::cout << "stop debug: " << debug << "\n";
                }
                // try 
                // {std::scoped_lock lock(command_mutex);
                //     stopper->request_stop();
                //     execution_thread->join();
                //     this->execution_thread = nullptr;
                //     std::cout << "BaseCommandInterface: DESTROYED COMMAND\n";
                // }
                // catch (std::exception& e)
                // {
                //     std::cout << "BaseCommandInterface: FAILED TO DESTROY COMMAND: " << e.what() << "\n";
                // }
                // this->handle_stop();

            }


            private:
            bool stop_requested = false;
            // unsigned int tick_rate_ms;
            // std::promise<ExecutionResult> result_promise;
        };


        class StagedCommandInterface : public SingleProccessCommandInterface
        {
            public:
            virtual void add_stage(std::shared_ptr<SingleProccessCommandInterface> new_stage)
            {
                stages.push_back(new_stage);
            }

            protected:
            std::vector<std::shared_ptr<SingleProccessCommandInterface>> stages;
            std::shared_future<ExecutionResult> current_stage_res_future;


            virtual void logic_tick() override
            {
                // std::cout << "StagedCommandInterface::logic_tick()\n";
                if (!stages.empty())
                {
                    std::cout <<"\t!stages.empty()\n";
                    if (!current_stage_res_future.valid())
                    {
                        std::cout <<"\t\t!current_stage_res_future.valid()\n";
                        current_stage_res_future = this->stages.at(0)->get_result_future();
                    }

                    if (current_stage_res_future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready)
                    {
                        std::cout <<"\t\tcurrent_stage_res_future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready\n";
                        this->stages.at(0)->tick();
                    }
                    else 
                    {
                        std::cout <<"\t\tcurrent_stage_res_future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready\n";
                        ExecutionResult stage_status = current_stage_res_future.get();
                        if (stage_status == ExecutionResult::SUCCESS)
                        {
                            std::cout <<"\t\t\tstages.erase()\n";
                            stages.erase(stages.begin());
                            current_stage_res_future = std::shared_future<ExecutionResult>();
                        }
                        else
                        {
                            std::cout <<"\t\t\texecution failed.\n";
                            this->stop(ExecutionResult::FAILED);
                        }
                    }
                }
                else
                {
                    std::cout <<"\tstages.empty()\n";
                    this->stop(ExecutionResult::SUCCESS);
                }
            }
        };


        // class MultithreadedCommand : public BaseCommandInterface
        // {
        //     public:
        //     MultithreadedCommand()
        //     {
        //         stopper = std::make_shared<std::stop_source>();
        //     }


        //     void set_tick_rate(unsigned int rate_ms) override
        //     {
        //         std::scoped_lock lock(tick_rate_mutex);
        //         this->tick_rate_ms = rate_ms;
        //     }


        //     /**
        //      * @brief Gets a future representing the command's execution result.
        //      *
        //      * The future will be completed with either SUCCESS or FAILED when the command finishes.
        //      */
        //     std::shared_future<uavsdk::command_manager::ExecutionResult>  get_result_future() override
        //     {
        //         std::scoped_lock lock(promise_mutex);
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


        //     /**
        //      * @brief Starts executing the command.
        //      *
        //      * If a command is already running (exists in execution_thread), returns ALREADY_RUNNING.
        //      * Otherwise, starts a new thread and returns STARTED.
        //      */
        //     StartExecutionResult execute()
        //     {
        //         std::scoped_lock lock(command_mutex);
        //         if (execution_thread)
        //         {
        //             return StartExecutionResult::ALREADY_RUNNING;
        //         }
        //         else
        //         {
        //             execution_thread = std::make_shared<std::jthread>(std::bind(&MultithreadedCommand::_loop, this));
        //             return StartExecutionResult::STARTED;
        //         }
        //     }


        //     private:
        //     std::shared_ptr<std::stop_source> stopper;
        //     std::shared_ptr<std::jthread> execution_thread;

        //     std::mutex command_mutex;
        //     std::mutex promise_mutex;
        //     std::mutex tick_rate_mutex;

        //     /**
        //      * @brief Main loop for command execution.
        //      *
        //      * Monitors the stopper and calls periodic logic tick.
        //      */
        //     void _loop()
        //     {
        //         while (not stopper->stop_requested())
        //         {
        //             std::scoped_lock lock(tick_rate_mutex);
        //             _tick();
        //         }
        //         this->handle_stop();
        //     }
        // };


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
            }

            void add_data_to_bb(std::string key, std::shared_ptr<useful_di::TypeInterface> data)
            {
                std::scoped_lock(bb_mutex);
                blackboard->add_data(key, data);
            }


            std::vector<std::string> get_keys_from_blackboard()
            {
                std::scoped_lock(bb_mutex);
                return blackboard->get_present_keys();
            }


            void remove_data_from_blackboard(std::string key)
            {
                std::scoped_lock(bb_mutex);
                blackboard->remove_data(key);
            }


            void modify_data_on_blackboard(std::string key, std::shared_ptr<useful_di::TypeInterface> new_data)
            {
                std::scoped_lock(bb_mutex);
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