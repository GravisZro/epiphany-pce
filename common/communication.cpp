#include "communication.h"
#ifdef COMMUNICATION_H
template<typename T> T* memzero(T* data)
{
  for(int i = 0; i < sizeof(T); ++i)
    reinterpret_cast<char*>(data)[i] = 0;
  return data;
}

template<typename T, typename U> T* memcopy(T*& data, const U*& new_data)
{
  for(int i = 0; i < sizeof(T); ++i)
    reinterpret_cast<char*>(data)[i] = reinterpret_cast<char*>(new_data)[i];
  return data;
}

template<typename T> constexpr T* fromend(size_t byte_offset = 0) { return to<T*>(0x8000 - sizeof(T) - byte_offset); }

struct entry_id_t
{
  char id[4];

  inline entry_id_t& operator =(const char* new_id)
    { *reinterpret_cast<uint32_t*>(id) = *reinterpret_cast<uint32_t*>(const_cast<char*>(new_id)); return *this; }

  inline bool operator ==(const char* new_id)
    { return *reinterpret_cast<uint32_t*>(id) == *reinterpret_cast<uint32_t*>(const_cast<char*>(new_id)); }

} pack;

struct header_entry_t
{
  entry_id_t id;
  uint16_t size; // size in bytes
  uint16_t data_addr;

  header_entry_t* next(void) { return this - 1; }
  header_entry_t* prev(void) { return this + 1; }

  template<typename T> void set_data(const T& data)
  {
    size = sizeof(data);
    data_addr = prev()->data_addr - size;
    *reinterpret_cast<T*>(m_header - data_addr) = data;
  }

  template<typename T> T& get_data(void)
  {
    assert(sizeof(T) == size);
    assert(data_addr);
    return *reinterpret_cast<T*>(m_header - data_addr);
  }
} pack;

constexpr header_entry_t* entry_addr(uint16_t entry_count);

struct Communication::header_t
{
  uint16_t entry_count;
  header_entry_t base_entry; // empty entry
  Communication* core_ptr;
  char core_id[8]; // id of this core
} pack;

constexpr header_entry_t* entry_addr(uint16_t entry_count) { return fromend<header_entry_t>(sizeof(header_t) + (sizeof(header_entry_t) * entry_count)); }


//***********************************************************************************

Communication::Communication(const char* local_core_label, uint16_t entry_count)
{
  memcopy(m_header->core_id, local_core_label);
  m_header->entry_count = entry_count;
  m_header->base_entry.size = 0;
  m_header->base_entry.data_addr = 0;
}

header_entry_t* get_entry(const char* id)
{
  header_entry_t* entry = entry_addr(0);
  for(uint16_t i = 0; i < entry_count; ++i, entry = entry->next())
    if(entry->id == id)
      break;

  if(!(entry->id == id))
  {
    entry = memzero(entry_addr(entry_count));
    ++entry_count;
    entry->id = id;
    entry->data_addr = 0;
    entry->size = 0;
  }

  return entry;
}


template<typename T> T& Communication::data    (const char* value_id)  { return *dataptr(value_id); }
template<typename T> T* Communication::dataptr (const char* value_id)
{

}

template<typename T> uint16_t Communication::size (const char* value_id)
{

}
#endif
