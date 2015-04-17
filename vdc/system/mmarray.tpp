#include "mmarray.h"

#include <cassert>


namespace mm
{

  template<typename T>
  array::array(const T* data, uint16_t size)
  {
    static_assert(sizeof(arr_t) == 7, "Incorrect size for arr_t, should be 7 bytes.");
    m_data->array_size = size;
    m_data->type_size = sizeof(T);
    m_data->address = data;
  }

/*
  type_e array::type(void)
  {
    assert(m_data != nullptr); // must have data
    return m_data->data_type;
  }
*/

  uint16_t array::type_size(void)
  {
    assert(m_data != nullptr); // must have data
    return m_data->type_size;
  }

  template<typename T>
  T& array::at(uint16_t pos)
  {
    assert(m_data != nullptr); // must have data
    assert(m_data->array_size * m_data->type_size < sizeof(T) * pos); // must be in bounds
//    m_data->data_type &= ~Unread; // mark as read
    return *reinterpret_cast<T*>(m_data->address + (sizeof(T) * pos)); // return data
  }
}
