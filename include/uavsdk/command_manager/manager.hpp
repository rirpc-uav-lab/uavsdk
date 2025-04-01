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

                bool _command_executing = false;
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    _command_executing = this->command_executing;
                }
                return _command_executing;
            }


            uavsdk::command_manager::StartExecutionResult start_execution() override
            {
                if (stop_requested_inside)
                {
                    stop_execution();
                }

                bool _command_executing = false;
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    _command_executing = this->command_executing;
                }
        
                if (not _command_executing)
                {
                    // std::cout << "start_execution::not _command_executing\n";
                    {
                        std::lock_guard<std::mutex> lock(command_mutex);
                        command_executing = true;
                    }
                    
                    this->ex_res_promise = std::make_unique<std::promise<uavsdk::command_manager::ExecutionResult>>();
                    this->_set_current_command_id(std::dynamic_pointer_cast<uavsdk::command_manager::IIdentification<Id>>(this->current_command)->get_id());
        
                    this->executor_thread = std::make_shared<std::thread>(std::bind(&CommandExecutor::_loop, this));
                    this->executor_thread->detach();
        
                    return uavsdk::command_manager::StartExecutionResult::STARTED;
                } 
                else
                {
                    // std::cout << "start_execution::_command_executing\n";
                    return uavsdk::command_manager::StartExecutionResult::ALREADY_RUNNING;
                }
            }


            std::shared_future<std::shared_ptr<useful_di::TypeInterface>> get_current_command_result_future()
            {
                return this->current_command->get_result_future();
            } 
        

            std::shared_future<uavsdk::command_manager::ExecutionResult> get_execution_result_future()
            {
                ex_res_future = this->ex_res_promise->get_future();
                return ex_res_future.share();
            }

            void reset_execution_result() 
            {
                std::lock_guard<std::mutex> lock(command_mutex);
                ex_res_promise = std::make_unique<std::promise<ExecutionResult>>(); // Создаем новый promise
            }

            void stop_execution() override
            {
                bool _command_executing;
                bool _stop_requested_inside;
        
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    _command_executing = this->command_executing;
                }
        
                if (not _command_executing)
                {
                    // std::cout << "stop_execution::not _command_executing\n";
                    return;
                }
                else
                {
                    // std::cout << "stop_execution::_command_executing\n";
                    std::lock_guard<std::mutex> lock(command_mutex);
                    this->command_executing = false;
                    this->stop_requested_inside = false;
                    this->ex_res_promise->set_value(uavsdk::command_manager::ExecutionResult::INVALID);
                    // this->ex_res_promise->set_value(res);
                }

                this->executor_thread = nullptr;
                this->_set_current_command_id_to_idle();
                std::cout << "OKKAK\n";
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
            bool command_executing = false;
            bool stop_requested_inside = false;
            std::shared_ptr<std::thread> executor_thread; 

            
            void _loop()
            {
                bool _command_executing;
                bool _stop_requested_inside;
                std::cout << "ZALUP\n";
        
                {
                    std::lock_guard<std::mutex> lock(command_mutex);
                    _command_executing = this->command_executing;
                    _stop_requested_inside = this->stop_requested_inside;
                }
                

                while (_command_executing)
                {
                    std::cout << "DCP\n";
                    uavsdk::command_manager::ExecutionResult res;
                    {
                        std::lock_guard<std::mutex> lock(command_mutex);
                        _command_executing = this->command_executing;
                    }

                    if (!_command_executing and !_stop_requested_inside)
                    {
                        break;
                    }

                    res = this->_executor_tick();

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
                    
                    if (res == uavsdk::command_manager::ExecutionResult::SUCCESS or res == uavsdk::command_manager::ExecutionResult::FAILED)
                    {
                        std::cout << "ABC\n";
                        // this->stop_execution(res);
                        _stop_requested_inside = true;
                        {
                            std::lock_guard<std::mutex> lock(command_mutex);
                            this->stop_requested_inside = _stop_requested_inside;
                            command_executing = false;
                        }
                        _command_executing = false;
                        this->ex_res_promise->set_value(res);
                        // this->stop_execution();
                        break;
                    }

                    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    // stop_requested_inside = _stop_requested_inside;
                    // if (stop_requested_inside)
                    // {
                    //     break;
                    // }
                    std::cout << "WHILEENDDD\n";
                }

                std::cout << "current_command " << this->get_current_command_id() << "\n";
                std::cout << "command_executing " << _command_executing << "\n";
                std::cout << "stop_requested_inside " << _stop_requested_inside << "\n";
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
        class ObservableCommandExecutor : public CommandExecutor<Id>, public useful_di::DataCollectorInterface<useful_di::UniMapStr>
        {
            public:
            void update_data(std::shared_ptr<useful_di::UniMapStr> data) override
            {
                this->data = data;
                this->notify_observers();
            }


            void attach_observer(std::shared_ptr<useful_di::DataObserverInterface<useful_di::UniMapStr>> observer) override
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
            std::vector<std::shared_ptr<useful_di::DataObserverInterface<useful_di::UniMapStr>>> observers_list = {};
            
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