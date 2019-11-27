
/*
 * Deal with the paging turn on and load for task 
 * Note: 
 *  kernel page is done by hard code in x86_desc.S
 *  init_paging is done in boot.S 
 */

#ifndef _TASK_PAHING_H
#define _TASK_PAHING_H

#include "types.h"

/*
 * Version 3.0 Tingkai Liu 2019.11.3
 * First written 
 * 
 * Version 4.0 Tingkai Liu 2019.11.17
 * Support system call: vidmap 
 *
 * Version 5.0 Tingkai Liu 2019.11.27
 * Support multi-terminal and scheduling
 */

#define     MAX_RUNNING_TASK    2           // for cp3
#define     MAX_NUM_TERMINAL    3
#define     PID_USED            666         // good number!
#define     PID_FREE            0

#define     TASK_VRAM_MAPPED        88          // interesting number!
#define     TASK_VRAM_NOT_MAPPED    0

#define     TASK_START_MEM      0x08000000  // 128MB
#define     TASK_END_MEM        0x08400000  // 132MB
#define     TASK_IMG_LOAD_ADDR  0x08048000
#define     TASK_PAGE_FLAG      0x00000087  // flags for a user level task
#define     TASK_VIR_MEM_ENTRY  32          // 128MB / 4MB
#define     TASK_VIR_VIDEO_MEM_ENTRY    0xB8

#define     TERMINAL_VID_OPENED         777     // funny number!
#define     TERMINAL_VID_NOT_OPENED     0
#define     MAGIC_NO_TERMINAL_OPENED    0xECE666    // used for ter_id indicating no opened terminal

int task_set_up_memory(const uint8_t* task_name, uint32_t* eip, const int ter_id);  // called by system call execute
int task_reset_paging(const int cur_id, const int pre_id);  // called by system call halt

int system_vidmap(uint8_t ** screen_start);

// active terminal switching
int terminal_vid_open(const int ter_id);
int terminal_vid_close(const int ter_id);
int terminal_active_vid_switch(const int new_ter_id, const int pre_ter_id);
int terminal_vid_set(const int ter_id);


#endif /*_TASK_PAHING_H*/

