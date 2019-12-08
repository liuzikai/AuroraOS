//
// Created by liuzikai on 11/2/19.
//

#ifndef _TASK_H
#define _TASK_H

#include "types.h"
#include "x86_desc.h"
#include "file_system.h"

#define PROCESS_INITIAL     1U  // initial process

// Process control block (PCB) stores info for every task
typedef struct process_t process_t;
struct process_t {
    uint8_t valid;
    uint8_t* executable_name;
    uint8_t* args;
    uint32_t flags;
    process_t* parent;
    uint32_t kesp;  // kernel stack esp
    int32_t page_id;
    file_array_t file_array;
};


// Process kernel memory (PKM) consists of task control block and the kernel stack for a task
#define PKM_SIZE_IN_BYTES    8192
typedef union process_kernel_memory_t process_kernel_memory_t;
union process_kernel_memory_t {
    process_t tcb;
    uint8_t kstack[PKM_SIZE_IN_BYTES];  // stack for kernel status
};

// TODO: change comment
#define PKM_STARTING_ADDR    KERNEL_ESP_START  // PKM starts at 8MB (bottom of kernel image), going to low address
#define PKM_ALIGN_MASK       0xFFFFE000  // PKM is 8k-aligned, when in kernel_stack, mask ESP with this is current PCB

// Address of idx-th process control block
#define ptr_process(idx) ((process_t *) (PKM_STARTING_ADDR - (idx + 1) * PKM_SIZE_IN_BYTES))
#define PROCESS_MAX_CNT    2  // maximum number of processes running at the same time
extern uint32_t process_cnt;

/**
 * Get current process based on ESP. Only for usage in kernel state.
 * @return Pointer to current process
 */
static inline process_t* cur_process() {
    process_t* ret;
    asm volatile ("movl %%esp, %0  \n\
                   andl $0xFFFFE000, %0    /* PKM_ALIGN_MASK */" \
                   : "=r" (ret) \
                   : \
                   : "cc", "memory" \
                   );
    return ret;
}

void task_init();
void task_run_initial_process();

int32_t system_execute(uint8_t *command);
int32_t system_halt(int32_t status);
int32_t system_getargs(uint8_t *buf, int32_t nbytes);

#endif // _TASK_H
