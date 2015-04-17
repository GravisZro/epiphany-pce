#ifndef HCOMMUNICATION_H
#include "hcommunication.h"
#endif

#include <string.h>
#include <cassert>

mm::arr_t* HCommunication::m_to_host;
mm::arr_t* HCommunication::m_to_core;
uint32_t HCommunication::m_base_address = 0;
uint32_t HCommunication::m_default_buffer_size = 32;

inline uint32_t HCommunication::header_address(uint32_t core)
{
  return m_base_address + (core * sizeof(mm::arr_t));
}

inline uint32_t HCommunication::data_address(uint32_t core)
{
  return header_address(core_count) + (core * m_default_buffer_size);
}

void HCommunication::set_base_address(uint32_t address)
{
  m_base_address = address;
  for(uint16_t i = 0; i < core_count; ++i)
    m_to_host[i] = { 0, 0, header_address(i) };
}

inline void HCommunication::set_default_buffer_size(uint32_t size)
{
  m_default_buffer_size = size;
}



template<typename T>
void HCommunication::write(const uint16_t core, const T* data, const uint32_t size)
{
  assert(m_base_address);

  m_to_core[core].address = data_address(core);
  m_to_core[core].array_size = size;
  m_to_core[core].type_size = sizeof(T);

  if(m_to_core[core].size() > m_default_buffer_size) // if large data
  {
    m_to_core[core].address = data_address(core_count);
    for(mm::arr_t* pos = m_to_core; pos < m_to_core + core_count; ++pos)
    {
      if(pos != m_to_core + core &&
         pos->size() > m_default_buffer_size &&
         m_to_core[core].end() >= pos->address)
        m_to_core[core].address = pos->end() + 1;
    }
  }

  memcpy(reinterpret_cast<void*>(m_to_core[core].address), data, m_to_core[core].size());
}


template<typename T>
void HCommunication::read(const uint16_t core, T*& data, uint32_t& size)
{
  assert(m_base_address);
  memcpy(&data, reinterpret_cast<void*>(m_to_core[core].address), size);
}




template<typename T>
void HCommunication::write(const uint16_t core, const T& data)
{
  write(core, &data, 1);
}

template<typename T>
void HCommunication::read(const uint16_t core, const T& data)
{
  read(core, &data, 1);
}
