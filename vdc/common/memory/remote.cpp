#include "remote.h"

#include <global.h>
#include <memory.h>
#include <cassert>

std::map<uint32_t, DMAdata*> RemoteMem::s_memories;

template<uint32_t address, typename T, RemoteMem::MemoryAccess access>
RemoteMem::RemoteMem(void)
{
  static_assert(sizeof(RemoteMem) == sizeof(DMAdata), "bad size for LocalMem");
  s_memories[address] = this;
}


template<typename T>
void RemoteMem::operator =(const T& other)
{
  assert(sizeof(T) == size);
  *reinterpret_cast<T*>(data) = other;
  if(access & Write)
    mem_core_write(*this);
}

template<typename T>
RemoteMem::operator T&(void)
{
  if(access & Read)
    mem_core_read(*this);
  return *reinterpret_cast<T*>(data);
}


void RemoteMem::write(const uint8_t* dataptr)
{
  memcpy((void*)data, (const void*)dataptr, size);
}

void RemoteMem::read(uint8_t*& dataptr)
{
  dataptr = data;
}

DMAdata* RemoteMem::raw(uint32_t address)
{
  return s_memories[address];
}
