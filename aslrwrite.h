//**************************************************//
//**This Header File is used in combination********//
//**with a dynamic Library and must be rewritten**//
//**if you want to use it for another purpose****//
//**********************************************//

//******************************************//
//**Credits: HackJack & Razzile(Kamizoom)**//
//****************************************//

//********************************************//
//**Usage: writeData(0xOFFSET, 0xDATA,SIZE)**//
//******************************************//

//importing and including files

#import <Foundation/Foundation.h>
#import <mach/mach.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#import <mach/mach_traps.h>

/*
This is the main Function. This is where the writing takes place.
It declares the port as mach_task_self, calculates the offset.
Then it changes the Protections at the offset to be able to write to it.
After that it sets the Protections back so the Apps runs as before
Then it writes either 4 Byte or 2 to the address.
Parameters: the address, the data to be written, the size 2 or 4
Return: True = Success or False = Failed
*/

__attribute__((visibility("hidden"))) bool writeData(vm_address_t offset,  unsigned long long data, int size) {

    //declaring variables

    kern_return_t err;
    mach_port_t port = mach_task_self();
    vm_address_t address = (offset + _dyld_get_image_vmaddr_slide(0));

    //set memory protections to allow us writing code there

    err = vm_protect(port, (vm_address_t) address, size, NO,VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);

    //check if the protection fails

    if (err != KERN_SUCCESS) {

       return FALSE;

    }
    
    //write code to memory
    
    err = vm_write(port, (vm_address_t)address, (vm_address_t)&data, size);
    
    if (err != KERN_SUCCESS) {
        
        return FALSE;
        
    }
    
    //set the protections back to normal so the app can access this address as usual

    err = vm_protect(port, (vm_address_t)address, size, NO,VM_PROT_READ | VM_PROT_EXECUTE);
    
    if (err != KERN_SUCCESS) {
        
        return FALSE;
        
    }

    return TRUE;
	
}