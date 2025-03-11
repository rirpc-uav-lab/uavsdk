#pragma once
#include <memory>
#include <thread>
#include <mutex>

#include <uavsdk/useful_data_lib/useful_data_impl.hpp>

#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/command_manager/executors.hpp>



namespace uavsdk
{
    namespace command_manager
    {
        namespace control_nodes
        {
            class Sequence : public uavsdk::command_manager::StagedCommandInterface, public uavsdk::command_manager::IBlackboard
            {
                public:
                Sequence(std::shared_ptr<useful_di::UniMapStr> _blackboard, int id=-1) : IBlackboard(_blackboard)
                {
                    auto seq = std::make_shared<uavsdk::command_manager::executors::SequentialExecutionStrategy>();
                    this->set_execution_strategy(seq);
                    if (id != -1)
                    {
                        this->set_id("sequence" + std::to_string(id));
                    }
                    else 
                    {
                        this->set_id("sequence");
                    }
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    return this->execution_strategy->execute_stages(this->stages);
                }
            };


            class Fallback : public uavsdk::command_manager::StagedCommandInterface, public uavsdk::command_manager::IBlackboard
            {
                public:
                Fallback(std::shared_ptr<useful_di::UniMapStr> _blackboard, int id=-1) : IBlackboard(_blackboard)
                {
                    auto fallb = std::make_shared<uavsdk::command_manager::executors::FallbackExecutionStrategy>();
                    this->set_execution_strategy(fallb);
                    if (id != -1)
                    {
                        this->set_id("fallback" + std::to_string(id));
                    }
                    else 
                    {
                        this->set_id("fallback");
                    }
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    return this->execution_strategy->execute_stages(this->stages);
                }
            };


            class ParallelStrict : public uavsdk::command_manager::StagedCommandInterface, public uavsdk::command_manager::IBlackboard
            {
                public:
                ParallelStrict(std::shared_ptr<useful_di::UniMapStr> _blackboard, int id=-1) : IBlackboard(_blackboard)
                {
                    auto par_strict = std::make_shared<uavsdk::command_manager::executors::ParallelStrictExecutionStrategy>();
                    this->set_execution_strategy(par_strict);
                    if (id != -1)
                    {
                        this->set_id("parallel_strict" + std::to_string(id));
                    }
                    else 
                    {
                        this->set_id("parallel_strict");
                    }
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    return this->execution_strategy->execute_stages(this->stages);
                }
            };


            class ParallelHopeful : public uavsdk::command_manager::StagedCommandInterface, public uavsdk::command_manager::IBlackboard
            {
                public:
                ParallelHopeful(std::shared_ptr<useful_di::UniMapStr> _blackboard, int id=-1) : IBlackboard(_blackboard)
                {
                    auto par_hopeful = std::make_shared<uavsdk::command_manager::executors::ParallelHopefulExecutionStrategy>();
                    this->set_execution_strategy(par_hopeful);
                    if (id != -1)
                    {
                        this->set_id("parallel_hopeful" + std::to_string(id));
                    }
                    else 
                    {
                        this->set_id("parallel_hopeful");
                    }
                    this->___set_type();
                }

                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    return this->execution_strategy->execute_stages(this->stages);
                }
            };
        }; 
    };
};






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