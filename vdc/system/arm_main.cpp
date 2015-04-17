#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
//#include <e-hal.h>

#include <system/framebuffer.h>

/*
#define BUF_OFFSET 0x01000000
#define MAXCORES 16
#define FBDEV "/dev/fb0"
#define ROWS 4
#define COLS 4
#define FRAMES 2000000
*/

/*

int main(void)
{
  static msg_block_t msg;
  memset(&msg, 0, sizeof(msg));



  e_platform_t eplat;
  e_epiphany_t edev;
  e_mem_t emem;

  e_init(nullptr);
  e_reset_system();
  e_get_platform_info(&eplat);
  e_alloc(&emem, BUF_OFFSET, sizeof(msg_block_t));

  uint8_t core_id = 0;
  volatile unsigned int vepiphany[MAXCORES] = { 0 };
  volatile unsigned int vcoreid = 0;
  unsigned int vhost[MAXCORES] = { 0 };
  e_open(&edev, 0, 0, ROWS, COLS);
  e_write(&emem, 0, 0, 0, &msg, sizeof(msg));
  e_reset_group(&edev);
  e_load_group("epiphany.srec", &edev, 0, 0, ROWS, COLS, E_TRUE);


  nano_wait(0, 100000000);
  clock_gettime(CLOCK_REALTIME, &time);
  time0 = time.tv_sec + time.tv_nsec * 1.0e-9;

  for(uint frame = 0; frame < FRAMES; ++frame)
  {
    for (uint core = 0; core < MAXCORES; ++core)
    {
        do
        {
          e_read(&emem, 0, 0, (off_t)((char *)&msg.msg_d2h[core] - (char *)&msg), &msg.msg_d2h[core], sizeof(msg_dev2host_t));
          vepiphany[core] = msg.msg_d2h[core].value;
          nano_wait(0, 1000000);
        } while (vepiphany[core] >= 0);
      }
    }

    for (row = 0; row < ROWS; row++)
    {
      for (col = 0; col < COLS; col++)
      {
        e_resume(&edev, row, col);
      }
    }
  }

  clock_gettime(CLOCK_REALTIME, &time);
  time1 = time.tv_sec + time.tv_nsec * 1.0e-9;
  //printf("frames: %d\n", FRAMES);
  //printf("time: %f sec\n", time1 - time0);

  e_close(&edev);
  e_free(&emem);
  e_finalize();

  return 0;

}
*/
