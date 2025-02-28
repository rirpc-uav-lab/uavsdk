
#pragma once
#include <memory>
#include <thread>
#include <mutex>

#include <uavsdk/command_manager/control_nodes.hpp>
#include <uavsdk/command_manager/command_interface.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/cmd_manager.hpp>

    class Success : public uavsdk::command_manager::SingleProccessCommandInterface
    {
        public:
        Success()
        {
            this->___set_type();
        }
        uavsdk::command_manager::ExecutionResult logic_tick() override
        {
                std::cout << "Success\n";
                return uavsdk::command_manager::ExecutionResult::SUCCESS;
        }
    };

    class Failed : public uavsdk::command_manager::SingleProccessCommandInterface
    {
        public:

        Failed()
        {
            this->___set_type();
        }
        uavsdk::command_manager::ExecutionResult logic_tick() override
        {
                std::cout << "Failed\n";
                return uavsdk::command_manager::ExecutionResult::FAILED;
        }
    };

    class SuccessAfterRunning : public uavsdk::command_manager::SingleProccessCommandInterface
    {
        public:

        SuccessAfterRunning()
        {
            this->___set_type();
        }

        uavsdk::command_manager::ExecutionResult logic_tick() override
        {
            if (i == 4)
            {
                std::cout << "Success!\n";
                return uavsdk::command_manager::ExecutionResult::SUCCESS;
                i = 0;
            }
            else
            {
                std::cout << "Running\n";
                i++;
                return uavsdk::command_manager::ExecutionResult::RUNNING;
            }
                
        }
        private:
        int i = 0;
    };

int main()
{
    uavsdk::agrilab::commands::Executor executor;
    std::shared_ptr<useful_di::UniMapStr> blackboard_init =  std::make_shared<useful_di::UniMapStr>();
    auto sequsss = std::make_shared<uavsdk::command_manager::ControlNodes::Sequence>(blackboard_init);
    auto fallback = std::make_shared<uavsdk::command_manager::ControlNodes::Fallback>(blackboard_init);

    sequsss->add_stage(std::make_shared<Success>());

    fallback->add_stage(std::make_shared<Failed>());
    fallback->add_stage(std::make_shared<SuccessAfterRunning>());

    sequsss->add_stage(fallback);

    executor.set_command(sequsss);

    
    uavsdk::command_manager::StartExecutionResult res = executor.start_execution();
    
    std::shared_future<uavsdk::command_manager::ExecutionResult> res_future = executor.get_execution_result_future();

    if (res == uavsdk::command_manager::StartExecutionResult::STARTED)
    {
        auto status = res_future.wait_for(std::chrono::milliseconds(10));
        while (!(status == std::future_status::ready))
        {
            status = res_future.wait_for(std::chrono::milliseconds(10));
        }
        auto exres = res_future.get();
        if (exres == uavsdk::command_manager::ExecutionResult::SUCCESS)
        {
            std::cout << "Success! Final \n";
        }
        else
        {
            std::cout << "Failed! Final\n";
        }
        status = res_future.wait_for(std::chrono::milliseconds(10));
    }

    executor.stop_execution();
}