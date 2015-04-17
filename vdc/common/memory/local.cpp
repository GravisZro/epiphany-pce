#include "local.h"

std::map<uint32_t, DMAdata*> LocalMem::s_memories;
uint16_t s_mem_address = 0x7800;

template<uint32_t address, typename T, DMAdata::MemoryAccess access>
LocalMem::LocalMem(void)
{
  static_assert(sizeof(LocalMem) == sizeof(DMAdata), "bad size for LocalMem");


  s_memories.emplace(address, s_mem_address);
  DMAdata* d = s_mem_address;
  s_mem_address += sizeof(T);
  //s_memories.end()
}

template<typename T>
void LocalMem::operator =(const T& other)
{

}

template<typename T>
LocalMem::operator T&(void)
{

}

void LocalMem::write(const uint8_t* dataptr)
{

}

void LocalMem::read(uint8_t*& dataptr)
{

}

template<uint32_t Addr, typename T>
void LocalMem::write(T* data)
{

}

template<uint32_t Addr, typename T>
T* LocalMem::read(void)
{

}

DMAdata* LocalMem::raw(uint32_t address)
{
  return nullptr;
}

#if 0
namespace LocalMem
{



  static DMAdata* raw(uint32_t address)
  {
    assert(s_memories.find(address) != s_memories.end());
    DMAdata* mem = s_memories[address];

    assert(mapped.size = sizeof(T));

    return &mapped.data;
  }

  template<uint32_t Addr, typename T>
  static T* Allocate(void)
  {
    static uint16_t mem_address = 0x7800;
    lookup_addresses.emplace(Addr, mem_address);
    mapped_memory<T>& mapped = *reinterpret_cast<mapped_memory<T>*>(mem_address);
    mapped.share = false;
    mapped.size = sizeof(T);
    mem_address = mapped.size + 2;
    return mem_address - mapped.size;
  }

};
#endif
