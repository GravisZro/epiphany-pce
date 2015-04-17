#if 0
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <global.h>

/*/
 * notes:
 *   - "core id"s can be a maximum of eight chars (everything after the first eight chars is ignored)
 *   - data identifier is a four char string (everything after the first four chars is ignored)
 */

class Communication
{
public:
  Communication(const char* local_core_label, uint16_t entry_count);

  template<typename T> static Communication* findcore (const char* remote_core_label); // find the core with this label

  template<typename T> T&       data    (const char* value_id); // referenced data can be read from and written to
  template<typename T> T*       dataptr (const char* value_id); // data pointed to can be read from and written to
  template<typename T> uint16_t size    (const char* value_id); // size of entry on own core

private:
  struct header_t;
  struct header_entry_t;
  header_t* m_header;
  header_entry_t* get_entry(const char* id); // creates entry if it doesnt exist
};

#endif // COMMUNICATION_H

#endif
