#pragma once

#include <memory>
#include <mutex>
#include <uavsdk/useful_data_lib/base_interfaces.hpp>


namespace uavsdk
{
    namespace data_adapters
    {
        namespace cxx
        {
            template <typename T>
            class BasicDataAdapter : public virtual useful_di::TypeInterface
            {
            public:
                BasicDataAdapter(T initial_data)
                {
                    this->___set_type();
                    this->data_p = std::make_shared<T>(initial_data);
                }

                
                virtual void set_data(T data)
                {
                    *this->data_p = data;
                }


                virtual T get_data()
                {
                    return *this->data_p; 
                }

            protected:
                std::shared_ptr<T> data_p;
            };


            template <typename T>
            class MutexDefendedDataAdapter : public BasicDataAdapter<T>
            {
            public:
                MutexDefendedDataAdapter(T initial_data) : BasicDataAdapter<T>(initial_data)
                {
                    this->___set_type();
                }


                virtual void set_data(T data) override
                {
                    std::lock_guard<std::mutex> lock(mx);
                    *this->data_p = data;
                }


                virtual T get_data() override
                {
                    std::lock_guard<std::mutex> lock(mx);
                    return *this->data_p; 
                }

            protected:
                std::mutex mx;
            };


            template <typename T>
            class PointerAccessDataAdapter : public BasicDataAdapter<T>
            {
                public:
                PointerAccessDataAdapter(T initial_data) : BasicDataAdapter<T>(initial_data)
                {
                    this->___set_type();
                }


                virtual std::shared_ptr<T> get_shared_ptr()
                {
                    return this->data_p;
                }
            };
        }
    }
}
