#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <e_lib.h>

#ifndef _ASSERT_H
#ifndef assert
#define assert(...)
#endif
#endif

#define SECTION(x) __attribute__ ((section (x)))

extern uint8_t HuCard[0x00100000] SECTION ("shared_dram"); // 8 Megabits for HuCard

struct pixel_t
{
  uint32_t        : 8; // alpha/buffer
  uint32_t red    : 3;
  uint32_t        : 5;
  uint32_t green  : 3;
  uint32_t        : 5;
  uint32_t blue   : 3;
  uint32_t        : 5;
};

struct framebuffer_t
{
  uint16_t xres; // maximum: 565
  uint16_t yres; // maximum: 242
  pixel_t pixels[0x24000];
};

extern framebuffer_t FrameBuffer SECTION ("shared_dram"); // framebuffer


/* NOTE: These translator functions allow you to convert one type to another without the need for
 *       taking the address of the type, casting it, then take the value at the address.  They
 *       also let you translate absolute addresses (e.g. 0x00001000) into a type of your choosing.
 *
 *
 *       The ptr() function is best used for quickly translating an absolute address into a void*.
 *       This is most useful when creating a new class at an absolute address like the following:
 *
 *        MyClass* myc = new (ptr(0x00001000)) MyClass();
 *
 *
 *       The template function to() is very useful for absolute address assignment of struct and
 *       simple variables:
 *
 *        some_struct& var3 = to<some_struct>(0x00001000);
 *        uint16_t& var4 = to_u16(0x00001000);
 *
 *
 *       It's also possible to translate one type to another but /this can be very dangerous/
 *
 *        some_struct& var1 = to<some_struct>(ptr_to_variable_of_another_type);
 *        some_struct& var2 = to<some_struct>(variable_of_another_type);
 *        uint16_t& var5 = to_u16(variable_of_another_type);
 *        uint16_t& var6 = to_u16(ptr_to_variable_of_another_type);
 *
 *
 *       Of course you can just use them to /copy/ variables from an absolute address:
 *
 *        var7 = to_u16(0x00001000);
*/

// basic translators
template<typename A, typename B> constexpr A& to (B& b) { return *reinterpret_cast<A*>(&b); }
template<typename A, typename B> constexpr A& to (B* b) { return to<A, B>(*b); }

// special translators for converting numbers into pointers/objects
                     constexpr void*     ptr(int b) { return reinterpret_cast<void*>(b); }
template<typename A> constexpr A&        to (int b) { return *reinterpret_cast<A*>(b); }

// translator shortcuts
template<typename B> constexpr char&     to_s8 (B& b) { return to<char    >(b); }
template<typename B> constexpr uint8_t&  to_u8 (B& b) { return to<uint8_t >(b); }
template<typename B> constexpr uint16_t& to_u16(B& b) { return to<uint16_t>(b); }
template<typename B> constexpr uint32_t& to_u32(B& b) { return to<uint32_t>(b); }

// translator shortcuts for pointers
template<typename B> constexpr char&     to_s8 (B* b) { return to<char    >(b); }
template<typename B> constexpr uint8_t&  to_u8 (B* b) { return to<uint8_t >(b); }
template<typename B> constexpr uint16_t& to_u16(B* b) { return to<uint16_t>(b); }
template<typename B> constexpr uint32_t& to_u32(B* b) { return to<uint32_t>(b); }

// translator shortcuts for abolute addresses
constexpr char&     to_s8 (int b) { return to<char    >(b); }
constexpr uint8_t&  to_u8 (int b) { return to<uint8_t >(b); }
constexpr uint16_t& to_u16(int b) { return to<uint16_t>(b); }
constexpr uint32_t& to_u32(int b) { return to<uint32_t>(b); }


/* NOTE: The "align" attribute forces a variable/type/function to be aligned to a multiple of the
 *       size specified.  Alignment will override a parent object's "packed" attribute.
 *
 *       Aligned variables will take more space if they are not already aligned.  This will cause
 *       sizeof() to return different results based on how far it must be moved in order to be
 *       aligned.
 */

#define align   __attribute__((aligned))
#define align8  __attribute__((aligned(8)))
#define align16 __attribute__((aligned(16)))
#define align32 __attribute__((aligned(32)))
#define align64 __attribute__((aligned(64)))


/* NOTE: The "packed" attribute enables structures (and classes) to have their bytes in sequencial
 *       order regardless of system endianness.  This must be used if you are using a union to
 *       manipulate the same data as different types where byte order is important.
 *
 *       Bitfields are agressively packed, so unless you are explicit in reserving unused bits,
 *       they will be packed regardless of type boundries.
 */

#define pack __attribute__((packed))


/* NOTE: Combining "packed" and "aligned" attributes results in the struct/class being aligned
 *       but pack it's contents.  However, if a child value is specified as being aligned, it
 *       will override the packed attribute.
 */

#define packalign   __attribute__((packed, aligned))
#define packalign8  __attribute__((packed, aligned(8)))
#define packalign16 __attribute__((packed, aligned(16)))
#define packalign32 __attribute__((packed, aligned(32)))
#define packalign64 __attribute__((packed, aligned(64)))


/* NOTE: The "interrupt" attribute specifies that the funtion can be used as an ISR (Interupt
 *       Service Routine).  The function cannot take arguments or return any values and must be
 *       declared as such: void interrupt FunctionNameHere(void)
 *
 *       Connect the function using this synatax: e_irq_attach(E_some_INT, FuncNameHere);
 *
 *       Possible values for E_some_INT are:
 *
 *       E_SW_EXCEPTION
 *       E_MEM_FAULT
 *       E_TIMER0_INT
 *       E_TIMER1_INT
 *       E_MESSAGE_INT
 *       E_DMA0_INT
 *       E_DMA1_INT
 *       E_USER_INT
 *
 *       A single function can be connected to multiple interrupt signals.  The ISR can also be
 *       /replaced/ by running e_irq_attach() again.
 */

#define interrupt   __attribute__((interrupt(E_USER_INT)))

/******************************************************************************/


struct highlow_t
{
  uint8_t low;
  uint8_t high;
} pack;

const uint8_t _Ncores = 16;
const uint32_t EndOfRAM = 0x00007FFF;

typedef char e_barrier_t;
typedef int  e_mutex_t;
typedef int  e_mutexattr_t;
//typedef int  size_t;

extern volatile e_barrier_t      core_sync_barriers    [_Ncores]; // barriers array
extern          e_barrier_t*     tgt_core_sync_barriers[_Ncores]; // barriers array

//extern volatile e_mutex_t        mutex;                           // groupe lock mutex
//extern          e_dma_desc_t     dma_descriptor[3];               // descriptor structure for DMA

extern void EndOfClockCycle(void);     // wait for PCE clock tick (21 MHz) - DMA 0

template<typename T> constexpr T* fromend(size_t byte_offset = 0)
  { return to<T*>(EndOfRAM - sizeof(T) - byte_offset); }

//template<typename T> T* memzero(T* data);

#endif // GLOBAL_H
