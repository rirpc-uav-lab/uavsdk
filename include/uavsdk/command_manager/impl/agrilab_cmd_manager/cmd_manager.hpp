#include <uavsdk/command_manager/manager.hpp>


namespace uavsdk
{
    namespace agrilab
    {
        namespace commands
        {
            class Executor : public uavsdk::command_manager::CommandExecutor<std::string>
            {
                void _set_current_command_id_to_idle() override
                {
                    this->current_command_id = "idle";
                }
            };
        } // namespace commands
    } // namespace agrilab
} // namespace uavsdk
