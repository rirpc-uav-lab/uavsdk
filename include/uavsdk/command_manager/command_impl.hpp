#pragma once

#include <thread>
#include <memory>
#include <future>
#include <iostream>
#include <string>
#include <functional>

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/command_manager/executors.hpp>

namespace uavsdk
{
    namespace command_manager
    {
        template <typename Id>
        class CommandInterfaceWithBlackboard : public SingleProccessCommandInterface, public IIdentification<Id>
        {
            public:
            CommandInterfaceWithBlackboard(std::shared_ptr<useful_di::UniMapStr> _blackboard)
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