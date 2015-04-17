#ifndef HCOMMUNICATION_H
#define HCOMMUNICATION_H

#include <stdint.h>

#include "epiphany_info.h"
//#include "framebuffer.h"
#include "mmarray.h"

// host to core communication class

// note: the header describes the size of each shared data element being shared

class HCommunication
{
public:
  static void set_base_address(uint32_t address);
  static void set_default_buffer_size(uint32_t size);

  template<typename T> static void write(const uint16_t core, const T& data);
  template<typename T> static void read (const uint16_t core, const T& data);

  template<typename T> static void write(const uint16_t core, const T*  data, const uint32_t  size);
  template<typename T> static void read (const uint16_t core,       T*& data,       uint32_t& size);

private:
  static uint32_t header_address(uint32_t core);
  static uint32_t data_address  (uint32_t core);
private:
  static mm::arr_t* m_to_host;
  static mm::arr_t* m_to_core;
  static uint32_t m_base_address; // address where the headers start
  static uint32_t m_default_buffer_size; // default buffer size in bytes
};

#include "hcommunication.tpp"

#endif // HCOMMUNICATION_H
