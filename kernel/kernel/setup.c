#include <kernel/setup.h>
#include <kernel/multiboot.h>
#include <stdio.h> 
// /*
// struct multiboot_info
// {
//   /* Multiboot info version number */
//   multiboot_uint32_t flags;

//   /* Available memory from BIOS */
//   multiboot_uint32_t mem_lower;
//   multiboot_uint32_t mem_upper;

//   /* "root" partition */
//   multiboot_uint32_t boot_device;

//   /* Kernel command line */
//   multiboot_uint32_t cmdline;

//   /* Boot-Module list */
//   multiboot_uint32_t mods_count;
//   multiboot_uint32_t mods_addr;

//   union
//   {
//     multiboot_aout_symbol_table_t aout_sym;
//     multiboot_elf_section_header_table_t elf_sec;
//   } u;

//   /* Memory Mapping buffer */
//   multiboot_uint32_t mmap_length;
//   multiboot_uint32_t mmap_addr;

//   /* Drive Info buffer */
//   multiboot_uint32_t drives_length;
//   multiboot_uint32_t drives_addr;

//   /* ROM configuration table */
//   multiboot_uint32_t config_table;

//   /* Boot Loader Name */
//   multiboot_uint32_t boot_loader_name;

//   /* APM table */
//   multiboot_uint32_t apm_table;

//   /* Video */
//   multiboot_uint32_t vbe_control_info;
//   multiboot_uint32_t vbe_mode_info;
//   multiboot_uint16_t vbe_mode;
//   multiboot_uint16_t vbe_interface_seg;
//   multiboot_uint16_t vbe_interface_off;
//   multiboot_uint16_t vbe_interface_len;

//   multiboot_uint64_t framebuffer_addr;
//   multiboot_uint32_t framebuffer_pitch;
//   multiboot_uint32_t framebuffer_width;
//   multiboot_uint32_t framebuffer_height;
//   multiboot_uint8_t framebuffer_bpp;
// #define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
// #define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
// #define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
//   multiboot_uint8_t framebuffer_type;
//   union
//   {
//     struct
//     {
//       multiboot_uint32_t framebuffer_palette_addr;
//       multiboot_uint16_t framebuffer_palette_num_colors;
//     };
//     struct
//     {
//       multiboot_uint8_t framebuffer_red_field_position;
//       multiboot_uint8_t framebuffer_red_mask_size;
//       multiboot_uint8_t framebuffer_green_field_position;
//       multiboot_uint8_t framebuffer_green_mask_size;
//       multiboot_uint8_t framebuffer_blue_field_position;
//       multiboot_uint8_t framebuffer_blue_mask_size;
//     };
//   };
// };
// 

void print_mem_map(multiboot_memory_map_t *a){
    printf("    address :%x %x   length:%x \n", (int) a->addr, (int)a->addr + a->len, a->len);
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


}