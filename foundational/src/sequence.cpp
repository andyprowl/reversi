#include "stdafx.hpp"

#include "util/sequence.hpp"
#include <algorithm>

namespace util { namespace detail
{

sequence_iterator::sequence_iterator(int const n, int const upper) 
    : n{n}, upper{upper}
{ 
}
    
int sequence_iterator::operator * () const
{ 
    return this->n; 
}    
    
sequence_iterator& sequence_iterator::operator ++ ()
{ 
    this->n = std::min(this->upper, this->n + 1); 

    return *this; 
}

bool sequence_iterator::operator != (value_ref<sequence_iterator> rhs) const 
{ 
    return (this->n != rhs.n);
}

sequence::sequence(int const from, int const to) 
    : from{from}
    , to{to}
{ 
}    
    
sequence_iterator sequence::begin() const
{ 
    return {this->from, this->to}; 
}
    
sequence_iterator sequence::end() const
{ 
    return {this->to, this->to}; 
}

} }

namespace util
{

detail::sequence sequence(int const from, int const to)
{
    if (to < from) 
    { 
        throw bad_interval_exception{};
    }
    
    return {from, to};
}

}