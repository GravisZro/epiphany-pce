#ifndef REMOTE_MEMORY_H
#define REMOTE_MEMORY_H

#include "map.h"


template<uint32_t begin_address, uint32_t end_address = begin_address>
class CMappedMem
{
public:
  operator uint8_t(void)
  {
    GetMem();
    m_mem_read->Read(begin_address, m_value);
    return m_value;
  }

  CMappedMem& operator =(const uint8_t value)
  {
    GetMem();
    m_mem_write->Write(begin_address, value);
    return *this;
  }

protected:
  inline void GetMem(void)
  {
    if(m_mem_read  == nullptr ||
       m_mem_write == nullptr)
    {
      m_mem_read  = CMemoryMap::GetRead(begin_address);
      m_mem_write = CMemoryMap::GetWrite(begin_address);
    }
  }

  uint8_t m_value;
  MappedIO* m_mem_read;
  MappedIO* m_mem_write;
};

template<typename Type, uint32_t begin_address, uint32_t end_address = begin_address>
class CMappedMemType : public Type,
                       protected CMappedMem<begin_address, end_address>
{
public:
  operator Type(void) { return *static_cast<Type*>(this); }
  CMappedMemType& operator =(const Type& value) { return *this = value; }
};

/*
template<typename T, uint32_t Addr>
class CRemoteMemory
{
public:
  const T& read(void) const { return to<const T>(CMemoryMap::SystemRead(Addr)); }
  void write(const T& value) const { CMemoryMap::SystemWrite(Addr, to<const uint8_t>(value)); }

  CRemoteMemory& operator =(const T& value) { write(value); return *this; }
  T* operator ->(void) { static T tmp; tmp = read(); return &tmp; }

  operator T(void) { return read(); }
};
*/

template <uint32_t Addr> using mmuint8_t = CMappedMem<Addr, Addr>;

#endif // REMOTE_MEMORY_H
