#ifndef LOCAL_H
#define LOCAL_H

#include <stdint.h>
#include <map>
#include "dma.h"

class LocalMem : private DMAdata
{
public:
  template<uint32_t address, typename T, MemoryAccess access> LocalMem(void);

  template<typename T> void operator =(const T& other);
  template<typename T> operator T&(void);

  void write(const uint8_t* dataptr);
  void read(uint8_t*& dataptr);

  template<uint32_t Addr, typename T> static void write(T* data);
  template<uint32_t Addr, typename T> static T* read(void);

  static DMAdata* raw(uint32_t address);

private:
  static uint16_t s_mem_address;
  static std::map<uint32_t, DMAdata*> s_memories;
};

#endif // LOCAL_H
