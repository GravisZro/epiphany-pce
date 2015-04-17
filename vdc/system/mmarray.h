#ifndef MMARRAY_H
#define MMARRAY_H

#include <stdint.h>

namespace mm
{
/*
  enum type_e: uint8_t
  {
    Data    = 0x00,
    String  = 0x01,
    Action  = 0x02,

    Warning = 0x04,
    Error   = 0x08,

    Unread  = 0x40,
    ForCore = 0x80,
  };
*/
  struct arr_t
  {
//    type_e   data_type; // for IPC/ICC
    uint8_t  type_size;
    uint16_t array_size;
    uint32_t address;

    inline uint32_t end (void) const { return address   + size();     }
    inline uint32_t size(void) const { return type_size * array_size; }
  } __attribute__((packed));

  class array
  {
  public:
    template<typename T>
    array(const T* data = nullptr, uint16_t size = 0);

//    type_e type(void);

    uint16_t type_size(void);

    template<typename T>
    T& at(uint16_t pos);

  private:
    arr_t* m_data;
  };
}

#include "mmarray.tpp"
#endif // MMARRAY_H
