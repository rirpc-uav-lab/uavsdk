#include <iostream>
#include <memory>
#include <thread>

#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>

int main()
{
    using std::dynamic_pointer_cast;
    using uavsdk::data_adapters::cxx::BasicDataAdapter;
    
    useful_di::ObserverCollectorExpectations expectations;
    expectations.set_data_type(utils::cppext::get_type<BasicDataAdapter<uint32_t>>());

    auto f = [](std::shared_ptr<useful_di::TypeInterface> p) 
    {
        std::cout << "" << p->___get_type() << "\n";
        std::cout << "value: " << dynamic_pointer_cast<BasicDataAdapter<uint32_t>>(p)->get_data() << "\n";
    };

    useful_di::StrictBindableDataObserver observer(expectations, f);

    auto collector = std::make_shared<useful_di::StrictDataCollector>(expectations);

    observer.bind(collector);

    for (uint32_t i = 0; i < 100; i++)
    {
        // std::cout << "B\n";
        auto int_p = std::make_shared<BasicDataAdapter<uint32_t>>(i);
        collector->update_data(int_p);
        std::this_thread::sleep_for(std::chrono::milliseconds(333));
    }

    return 0;
}