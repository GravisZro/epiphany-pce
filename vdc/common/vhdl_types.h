#ifndef VHDL_TYPES_H
#define VHDL_TYPES_H

#include <cassert>
#include <list>
#include <vector>

#define downto ,

template<typename T>
class signal
{
public:
  signal(void);

  operator T(void) { return m_old_value; }
  void operator = (const T& value) { m_new_value = value; }

  bool operator ==(const T& value) { return value == m_old_value; }
  bool operator !=(const T& value) { return value != m_old_value; }

  bool operator > (const T& value) { return value >  m_old_value; }
  bool operator >=(const T& value) { return value >= m_old_value; }

  bool operator < (const T& value) { return value <  m_old_value; }
  bool operator <=(const T& value) { return value <= m_old_value; }

  void update(void) { m_old_value = m_new_value; }

private:
  T m_old_value;
  T m_new_value;
};

template<typename T>
class signal_pool
{
public:
  static void add (signal<T>* sig) { get_pool().push_back(sig); }
  static void update (void)
  {
    for(auto pos = get_pool().begin(); pos != get_pool().end(); ++pos)
      pos->update();
  }

private:
  static std::list<signal<T>*>& get_pool(void)
  {
    static std::list<signal<T>*> s_signal_pool;
    return s_signal_pool;
  }
};

template<typename T> signal<T>::signal(void) { signal_pool<T>::add(this); }




enum EDirection
{
  in,
  out,
  inout,
};

class std_logic_value
{
public:
  std_logic_value(void) : m_value { 'U', 0 } { }

  operator const char* (void) const { return m_value; }
  bool operator ==(const char* value) const { assert(!value[1]); return m_value[0] == value[0]; }
  bool operator !=(const char* value) const { assert(!value[1]); return m_value[0] != value[0]; }
  void operator = (const char* value)       { assert(!value[1]); m_value[0] = value[0]; }


  operator const char (void) const { return m_value[0]; }
  bool operator ==(const char& value) const { return m_value[0] == value; }
  bool operator !=(const char& value) const { return m_value[0] != value; }
  void operator = (const char& value)
  {
    switch(value)
    {
      default: assert(false);
      case 'U':  // uninitialized
      case 'X':  // strong drive, unknown logic value
      case '0':  // strong drive, logic zero
      case '1':  // strong drive, logic one
      case 'Z':  // high impedance
      case 'W':  // weak drive, unknown logic value
      case 'L':  // weak drive, logic zero
      case 'H':  // weak drive, logic one
      case '-':  // don't care
        m_value[0] = value;
    }
  }

private:
  char m_value[2];
};

// template<EDirection direction>
class std_logic
{
public:
  operator const char*(void) const { return m_value; }

  void operator = (const char* value)       {        m_value  = value; }
  bool operator ==(const char* value) const { return m_value == value; }
  bool operator !=(const char* value) const { return m_value != value; }

private:
  std_logic_value m_value;
};

struct others_t
{
public:
  void operator =(const char& value) { m_value = value; }
  operator std_logic_value(void) const { return m_value; }
private:
  std_logic_value m_value;
};


class std_logic_vector
{
public:
  std_logic_vector(const unsigned int upper_bound, const unsigned int lower_bound)
    : m_values(upper_bound - lower_bound), m_lower_bound(lower_bound) { }

  std_logic_value& operator [](const unsigned int index) { return m_values[index - m_lower_bound]; }

  void operator =(const std_logic_vector& ext) { m_values = ext.m_values; }

  void operator =(const char* value)
  {
    for(int i = 0; value[i]; ++i)
      m_values[i]  = value[i];
  }

  void operator =(const others_t& other)
  {
    for(auto pos = m_values.begin(); pos != m_values.end(); ++pos)
      *pos = other;
  }

private:
  std::vector<std_logic_value> m_values;
  const unsigned int m_lower_bound;
};

#endif // VHDL_TYPES_H
