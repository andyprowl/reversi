#pragma once

#include <boost/signals2/signal.hpp>

namespace util
{

template<typename>
struct handler_type;

template<typename R, typename... Args>
struct handler_type<boost::signals2::signal<R(Args...)>>
{
    
public:
    
    using type = std::function<R(Args...)>;

};

template<typename T>
using handler_type_t = typename handler_type<T>::type;

template<typename>
struct signal_type;

template<typename R, typename... Args>
struct signal_type<std::function<R(Args...)>>
{
    
public:
    
    using type = boost::signals2::signal<R(Args...)>;

};

template<typename T>
using signal_type_t = typename signal_type<T>::type;

}