#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0
#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif
	elf = (void*)buf;

	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);
	//set_bp();

	/* Load each program segment */
	int i=0;
	for(ph=(void *)(buf+elf->e_phoff); i<elf->e_phnum ; ++i,++ph) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {
			//Allocate user page for segment
			uint32_t hwaddr=mm_malloc(ph->p_vaddr, ph->p_memsz);
			//Physical memory and Virtual memory pointed to the same page
			//So (void *)((hwaddr)) or (void *)(pa_to_va(hwaddr)) all works in memcpy and memset.
#ifdef HAS_DEVICE
			//assert(ph->p_filesz<4096);
			ide_read((void *)((hwaddr)), ph->p_offset, ph->p_filesz);
#else
			memcpy((void *)((hwaddr)),(void *)(ph->p_offset),ph->p_filesz);
#endif
			memset((void *)((hwaddr)+ph->p_filesz),0,ph->p_memsz-ph->p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(brk < new_brk) { brk = new_brk; }
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif
	return entry;
}
