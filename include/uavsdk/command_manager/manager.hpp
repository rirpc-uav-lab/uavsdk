#pragma once
#include <memory>
#include <thread>
#include <mutex>

#include <uavsdk/command_manager/command_interface.hpp>



namespace uavsdk
{
    namespace command_manager
    {
        class CommandExecutorInterface
        {
            public: 
            void set_command(std::shared_ptr<uavsdk::command_manager::BaseCommandInterface> _command)
            {
                this->current_command = _command;
                
            }
        
            virtual uavsdk::command_manager::StartExecutionResult start_execution() = 0;
        
            virtual void stop_execution() = 0;
        
        
            protected:
            std::shared_ptr<uavsdk::command_manager::BaseCommandInterface> current_command;
        };
        
        
        template<typename Id>
        class CommandWithIdExecutorInterface : public CommandExecutorInterface
        {
            public:
            Id get_current_command_id()
            {
                return current_command_id;
            }
        
            protected: 
            Id current_command_id;
        };
        
        
        
        template <typename Id> 
        class CommandExecutor : public CommandWithIdExecutorInterface<Id>
        {
            public:
            CommandExecutor()
            {
                this->ex_res_promise = std::make_unique<std::promise<uavsdk::command_manager::ExecutionResult>>();
            }


            void set_sleep_period_ms(int ms)
            {
                std::lock_guard<std::mutex> lock(this->command_mutex);
                this->sleep_period_ms = ms;
            }


            bool is_running()
            {

                // bool _command_executing = false;
                // {
                //     std::lock_guard<std::mutex> lock(command_mutex);
                //     _command_executing = this->allowed_to_execute;
                // }
                return allowed_to_execute.load();
            }


            uavsdk::command_manager::StartExecutionResult start_execution() override
            {
                if (stop_requested_inside.load())
                {
                    // stop_execution();
                    throw std::runtime_error("CommandManager: start_execution() was called but the command has not yet stopped after finishing execution (stop_requested_inside is true).");
                }
        
                if (not allowed_to_execute.load())
                {
                    // std::cout << "Maxim " //pidor
                    // << dopishi siaty
                    allowed_to_execute.store(true);
                    
                    {
                        std::lock_guard<std::mutex> lock(command_mutex);
                        this->ex_res_promise = std::make_unique<std::promise<uavsdk::command_manager::ExecutionResult>>();
                        this->executor_thread = std::make_unique<std::thread>(std::bind(&CommandExecutor::_loop, this));
                        this->executor_thread->detach();
                    }
                    this->thread_finished.store(false);
                    this->_set_current_command_id(std::dynamic_pointer_cast<uavsdk::command_manager::IIdentification<Id>>(this->current_command)->get_id());
        
                    return uavsdk::command_manager::StartExecutionResult::STARTED;
                } 
                else
                {
                    return uavsdk::command_manager::StartExecutionResult::ALREADY_RUNNING;
                }
            }


            std::shared_future<std::shared_ptr<useful_di::TypeInterface>> get_current_command_result_future()
            {
                return this->current_command->get_result_future();
            } 
        

            std::shared_future<uavsdk::command_manager::ExecutionResult> get_execution_result_future()
            {
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    ex_res_future = this->ex_res_promise->get_future();
                }
                return ex_res_future.share();
            }

            void reset_execution_result() 
            {
                std::lock_guard<std::mutex> lock(command_mutex);
                ex_res_promise = std::make_unique<std::promise<ExecutionResult>>(); // Создаем новый promise
            }

            void stop_execution() override
            {
                // std::cout << "!&!&!&!&!CommandExecutor::stop_execution\n";
                if (not allowed_to_execute.load() and not stop_requested_inside.load())
                {
                    // std::cout << "!&!&!&!&!CommandExecutor::stop_execution::if-1-!&!&!&!&!\n";
                    return;
                }
                else if (allowed_to_execute.load() and not stop_requested_inside.load())
                {
                    // std::cout << "!&!&!&!&!CommandExecutor::stop_execution::if-2-!&!&!&!&!\n";
                    this->allowed_to_execute.store(false);
                    this->stop_requested_inside.store(false);

                    {
                        std::lock_guard<std::mutex> lock(command_mutex);
                        this->ex_res_promise->set_value(uavsdk::command_manager::ExecutionResult::INVALID);
                    }
                }
                else
                {
                    // std::cout << "!&!&!&!&!CommandExecutor::stop_execution::if-3-!&!&!&!&!\n";
                    this->allowed_to_execute.store(false);
                    this->stop_requested_inside.store(false);
                }

                // std::cout << "! ! ! ! ! EXECUTOR NULLIFIED ! ! ! ! !\n";
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    // this->executor_thread->~thread();
                    while (!thread_finished.load())
                    {
                        // std::cout << "! ! ! ! ! Waiting for thread to fi ! ! ! ! !\n";
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    this->executor_thread = nullptr;
                    // this->executor_thread->join();
                }
                // std::cout << "! ! ! ! ! EXECUTOR NULLIFIED ! ! ! ! !\n is it really? It is " << !this->executor_thread << "\n";
                this->_set_current_command_id_to_idle();
                // std::cout << "OKKAK\n";
            }
        
        
            protected:
            void _set_current_command_id(Id id)
            {
                this->current_command_id = id;
            }
        
        
            virtual void _set_current_command_id_to_idle() = 0;
        
        
            private:
            int sleep_period_ms = 100;
            std::unique_ptr<std::promise<uavsdk::command_manager::ExecutionResult>> ex_res_promise;
            std::future<uavsdk::command_manager::ExecutionResult> ex_res_future;
            std::mutex command_mutex;
            std::atomic<bool> allowed_to_execute{false};
            std::atomic<bool> stop_requested_inside{false};
            std::atomic<bool> thread_finished{true};
            std::unique_ptr<std::thread> executor_thread; 

            
            void _loop()
            {
                while (allowed_to_execute.load())
                {
                    // std::cout << "!&!&!&!&!while step\n";
                    uavsdk::command_manager::ExecutionResult res;

                    // std::cout << "!&!&!&!&! !allowed_to_execute.load()" << !allowed_to_execute.load() << "\n";
                    // std::cout << "!&!&!&!&! !stop_requested_inside.load()" << !stop_requested_inside.load() << "\n";
                    // std::cout << "!&!&!&!&! this->get_current_command_id()" << this->get_current_command_id() << "\n";
                    if (allowed_to_execute.load() and !stop_requested_inside.load()) 
                    {
                        // std::cout << "!&!&!&!&! tick\n";
                        // std::cout << "!&!&!&!&! allowed_to_execute.load()" << allowed_to_execute.load() << "\n";
                        // std::cout << "!&!&!&!&! !stop_requested_inside.load()" << !stop_requested_inside.load() << "\n";
                        res = this->_executor_tick();
                    }

                    
                    // std::cout << "!&!&!&!&! res == uavsdk::command_manager::ExecutionResult::SUCCESS" << bool(res == uavsdk::command_manager::ExecutionResult::SUCCESS) << "\n";
                    // std::cout << "!&!&!&!&! res == uavsdk::command_manager::ExecutionResult::FAILED" << bool(res == uavsdk::command_manager::ExecutionResult::FAILED) << "\n";
                    // std::cout << "!&!&!&!&! res" << static_cast<int>(res) << "\n";
                    if (res == uavsdk::command_manager::ExecutionResult::SUCCESS || res == uavsdk::command_manager::ExecutionResult::FAILED)
                    {
                        // std::cout << "!&!&!&!&!Command finished\n";
                        // this->stop_execution(res);

                        this->stop_requested_inside.store(true);
                        this->allowed_to_execute.store(false);

                        // _command_executing = false;
                        {
                            std::lock_guard<std::mutex> lock(command_mutex);
                            this->ex_res_promise->set_value(res);
                        }
                        
                        // this->stop_execution();
                        break;
                    }
                    // std::cout << "WHILEENDDD\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                }
                this->thread_finished.store(true);

                // std::cout << "current_command " << this->get_current_command_id() << "\n";
                // std::cout << "allowed_to_execute " << allowed_to_execute.load() << "\n";
                // std::cout << "stop_requested_inside " << stop_requested_inside.load() << "\n";
            }
        
            virtual uavsdk::command_manager::ExecutionResult _executor_tick()
            {
                uavsdk::command_manager::ExecutionResult res = std::dynamic_pointer_cast<uavsdk::command_manager::SingleProccessCommandInterface>(this->current_command)->tick();
                
                // if (res == uavsdk::command_manager::ExecutionResult::SUCCESS or res == uavsdk::command_manager::ExecutionResult::FAILED)
                // {
                //     this->stop_execution(res);
                // }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_period_ms));
                return res;
            }
        };


        template <typename Id> 
        class ObservableCommandExecutor : public CommandExecutor<Id>, public useful_di::IDataCollector
        {
            public:
            void update_data(std::shared_ptr<useful_di::TypeInterface> data) override
            {
                this->data = std::dynamic_pointer_cast<useful_di::UniMapStr>(data);
                this->notify_observers();
            }


            void attach_observer(std::shared_ptr<useful_di::IObserver<useful_di::TypeInterface>> observer) override
            {
                if (observer)
                {
                    this->observers_list.push_back(observer);
                }
                else
                {
                    throw std::runtime_error("ObservableCommandExecutor::attach_observer: Error! Observer pointer is invalid.\n");
                }
            }


            protected:
            void notify_observers() override
            {
                size_t i = 0;
                std::vector<size_t> deletion_list = {};

                for (const auto& observer : this->observers_list)
                {
                    if (observer)
                    {
                        observer->be_notified(this->data);
                    }
                    else
                    {
                        std::cout << "ObservableCommandExecutor::notify_observers: Warning! Observer pointer is invalid. Deleting\n";
                        deletion_list.push_back(i);
                    }
                    i++;
                }

                // for (size_t idx = deletion_list.size() - 1; idx >= 0; idx--)
                // {
                //     observers_list.erase(observers_list.begin() + deletion_list.at(idx));
                // }
            }

            private:
            std::shared_ptr<useful_di::UniMapStr> data;
            std::vector<std::shared_ptr<useful_di::IObserver<useful_di::TypeInterface>>> observers_list = {};
            
            virtual uavsdk::command_manager::ExecutionResult _executor_tick() override
            {
                auto command = std::dynamic_pointer_cast<uavsdk::command_manager::SingleProccessCommandInterface>(this->current_command);
                
                uavsdk::command_manager::ExecutionResult res = command->tick();

                auto state = command->get_state();
                this->update_data(state);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                return res;
            }
        };
    }
}






// #pragma once
// #include <any>
// #include <map>
// #include <string>
// #include <iostream>
// #include <memory>


// using namespace std;

// template <typename T>
// std::string get_type()
// {
//     return typeid(T).name();
// }

// class Element
// {
//     public:
//     void doSomething()
//     {
        // std::cout << "default action"<< std::endl;
//     }
// };

// class Robot:Element
// {

// };

// class BlackBoard
// {
// private:
//     map<string, std::any> bb_;
// public:

//     template<typename Elem>
//     void addElement()
//     {
//         string name = get_type<Elem>();
//         auto it = bb_.find(name);
//         if (it == bb_.end())
//         bb_[name] = std::shared_ptr<Elem>();
//     }

//     BlackBoard(
        
//         /* args */);
//     ~BlackBoard();
// };















                    // switch (res) {
                    //     case ExecutionResult::FAILED:
                    //         std::cout << "FAILED_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_FAILED\n";
                    //         break;
                    //     case ExecutionResult::RUNNING:
                    //         std::cout << "RUNNING_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_RUNNING\n";
                    //         break;
                    //     case ExecutionResult::SUCCESS:
                    //         std::cout << "SUCCESS_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_SUCCESS\n";
                    //         break;
                    //     case ExecutionResult::INVALID:
                    //         std::cout << "INVALID_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_INVALID\n";
                    //         break;
                    //     default:
                    //         std::cout << "UNKNOWN_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_PIDORSTEAM_UNKNOWN\n";
                    //         break;
                    // }