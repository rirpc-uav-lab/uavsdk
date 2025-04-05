#pragma once

#include <memory>
#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>


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
                    *data_p = data;
                }


                virtual T get_data()
                {
                    return *data_p; 
                }

                protected:
                std::shared_ptr<T> data_p;
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