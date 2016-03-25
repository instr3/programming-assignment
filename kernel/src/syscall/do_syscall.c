#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		#define op1 tf->eax
		#define op2 tf->ecx
		#define op3 tf->edx
		#define op4 tf->ebx
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;

		case SYS_open:
			extern int fs_open(const char *pathname, int flags);
			tf->eax=fs_open(op1,op2);
			break;
		case SYS_read:
			extern int fs_read(int fd, void *buf, int len);
			tf->eax=fs_read(op1,op2,op3);
			break;
		case SYS_write:
			if(tf->ebx==1||tf->ebx==2)
			{
				extern void serial_printc(char);
				int i;char *c=(void *)tf->ecx;
				for(i=0;i<tf->edx;++i)
					serial_printc(*(c+i));
				//This is an old method
				//asm volatile (".byte 0xd6" : : "a"(2), "c"(tf->ecx), "d"(tf->edx));
				tf->eax=tf->edx;
				break;
			}
			else
			{
				extern int fs_write(int fd, void *buf, int len);
				tf->eax=fs_write(op1,op2,op3);
				break;
			}
		case SYS_lseek:
			extern int fs_lseek(int fd, int offset, int whence);
			tf->eax=fs_lseek(op1,op2,op3);
			break;
		case SYS_close:
			extern int fs_close(int fd);
			tf->eax=fs_close(op1);
			break;
		/* TODO: Add more system calls. */

		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

