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
            uavsdk::command_manager::StartExecutionResult start_execution() override
            {
                bool _command_executing = false;
                {
                    std::scoped_lock lock(command_mutex);
                    _command_executing = this->command_executing;
                }
        
                if (not _command_executing)
                {
                    // std::cout << "start_execution::not _command_executing\n";
                    {
                        std::scoped_lock lock(command_mutex);
                        command_executing = true;
                    }
        
                    this->_set_current_command_id(std::dynamic_pointer_cast<uavsdk::command_manager::CommandInterfaceWithId<Id>>(this->current_command)->get_id());
        
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


            std::shared_future<uavsdk::command_manager::ExecutionResult> get_result_future()
            {
                return this->current_command->get_result_future();
            } 
        
        
            void stop_execution() override
            {
                bool _command_executing;
        
                {
                    std::scoped_lock lock(command_mutex);
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
                    std::scoped_lock lock(command_mutex);
                    this->command_executing = false;
                }
                this->executor_thread = nullptr;
                this->_set_current_command_id_to_idle();
            }
        
        
            protected:
            void _set_current_command_id(Id id)
            {
                this->current_command_id = id;
            }
        
        
            virtual void _set_current_command_id_to_idle() = 0;
        
        
            private:
            std::mutex command_mutex;
            bool command_executing = false;
            std::shared_ptr<std::thread> executor_thread; 
        
            void _loop()
            {
                bool _command_executing;
        
                {
                    std::scoped_lock lock(command_mutex);
                    _command_executing = this->command_executing;
                }
                
                while (_command_executing)
                {
                    this->_executor_tick();
        
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    std::scoped_lock lock(command_mutex);
                    _command_executing = this->command_executing;
                }
            }
        
            void _executor_tick()
            {
                std::dynamic_pointer_cast<uavsdk::command_manager::SingleProccessCommandInterface>(this->current_command)->tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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