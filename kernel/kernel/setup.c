#include <kernel/setup.h>
#include <kernel/multiboot.h>
#include <stdio.h> 


void print_mempry_map_with_grub(multiboot_info_t* mdb){
    if(mdb->flags & MULTIBOOT_INFO_MEMORY ){
        printf("lower memory and uppper memory detected : %x %x \n", mdb->mem_lower, mdb->mem_upper);
    }



}
void setup_arch(multiboot_info_t* mdb_info){
    print_mempry_map_with_grub(mdb_info);


}