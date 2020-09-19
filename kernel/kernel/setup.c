#include <kernel/setup.h>
#include <kernel/multiboot.h>
#include <i386/mm.h>
#include <stdio.h> 


void print_mem_map(multiboot_memory_map_t *a){
    printf("    address :%x %x   length:%x \n", (int) a->addr, (int) (a->addr + a->len), (int)a->len);
    switch(a->type){
        case MULTIBOOT_MEMORY_AVAILABLE:
        printf("    Avaliable memory\n");
        break;
        case MULTIBOOT_MEMORY_RESERVED:
        printf("    Reversed memory\n");
        break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
        printf("    Apic memory\n");
        break;
        case MULTIBOOT_MEMORY_NVS:
        printf("    NVS memory\n");
        break;
        case MULTIBOOT_MEMORY_BADRAM:
        printf("    Bad memory\n");
        break;
    }
}

extern void mm_page_init(multiboot_info_t *);

void print_mempry_map_with_grub2(multiboot_info_t* mdb){

    if(mdb->flags & MULTIBOOT_INFO_MEMORY ){
        printf("lower memory and uppper memory detected :%x  %x\n", mdb->mem_lower, mdb->mem_upper);
    }else{
        printf("there is no mempry detected !!!!\n");
    }

    if(mdb->flags & MULTIBOOT_INFO_BOOTDEV){
        printf("bootdev have been detected : %x\n", mdb->boot_device);
    }else{
        printf("no bootdev\n");
    }

    if(mdb->flags & MULTIBOOT_INFO_CMDLINE){
        printf("cmd_line option : %s\n", (char*)mdb->cmdline);
    }else{
        printf("no cmd_line option\n");
    }

    if(mdb->flags & MULTIBOOT_INFO_MODS){
        printf("to do:: show modinfo here\n");
    }else{
        printf("no mod info for now\n");
    }

    if(mdb->flags & MULTIBOOT_INFO_AOUT_SYMS){
        printf("symbol table has been founded\n");
    }else if(mdb->flags & MULTIBOOT_INFO_ELF_SHDR){
        printf("elf header tabel has been found\n");
    }else{
        printf("error in flags bit 4&5");
    }

    if(mdb->flags & MULTIBOOT_INFO_MEM_MAP){
        printf("full memory map detected  as follow\n    memory map address: %x length : %x\n", mdb->mmap_addr, mdb->mmap_length);
        size_t all_mem_map_length = mdb->mmap_length ;
        multiboot_memory_map_t * mmap_pointer= mdb->mmap_addr;

        while(all_mem_map_length){
            print_mem_map(mmap_pointer);
            all_mem_map_length-= mmap_pointer->size + 4;
            mmap_pointer=(multiboot_memory_map_t *)(((char *)mmap_pointer )+ mmap_pointer->size + 4);
            //mmap_pointer += mmap_pointer->size;
        }
    }


    if(mdb->flags & MULTIBOOT_INFO_DRIVE_INFO){
        printf("drive info detected \n");
    }else{
        printf("no drive info detected \n");
    }

    if(mdb->flags & MULTIBOOT_INFO_CONFIG_TABLE){
        printf("config table detected\n");
    }else{
        printf("no config table detected \n");
    }

    if(mdb->flags &MULTIBOOT_INFO_BOOT_LOADER_NAME){
        printf("boot loader name : %s\n",(char*)mdb->boot_loader_name);
    }else{
        printf("no boot loader name\n");
    }
}
void setup_arch(multiboot_info_t* mdb_info){
    
    print_mempry_map_with_grub2(mdb_info);

    mm_page_init(mdb_info);

    
}