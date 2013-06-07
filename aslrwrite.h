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
This Function checks if the Application has ASLR enabled.
It gets the mach_header of the Image at Index 0.
It then checks for the MH_PIE flag. If it is there, it returns TRUE.
Parameters: nil
Return: Wether it has ASLR or not
*/

__attribute__((visibility("hidden"))) bool hasASLR() {
    
    const struct mach_header *mach;
    
    mach = _dyld_get_image_header(0);
    
    if (mach->flags & MH_PIE) {
	
	//has aslr enabled
	return TRUE;
	
    } else {
	
	//has aslr disabled
	return FALSE;
    }
    
    
}

/*
This Function gets the vmaddr slide of the Image at Index 0.
Parameters: nil
Return: the vmaddr slide
*/

__attribute__((always_inline))	__attribute__((visibility("hidden"))) vm_address_t vm_get_slide()
{
    return _dyld_get_image_vmaddr_slide(0);
}

/*
This Function calculates the Address if ASLR is enabled or returns the normal offset.
Parameters: The Original Offset
Return: Either the Offset or the New calculated Offset if ASLR is enabled
*/

__attribute__((visibility("hidden"))) vm_address_t calculateAddress(vm_address_t offset) {
	
    if (hasASLR()) {
	
	vm_address_t addr = vm_get_slide();
	
	return (addr + offset);
	
    } else {
	
	return offset;
	
    }
    
}

/*
This is the main Function. This is where the writing takes place.
It declares the port as mach_task_self, calculates the offset.
Then it changes the Protections at the offset to be able to write to it.
After that it sets the Protections back so the Apps runs as before
Then it writes either 4 Byte or 2 to the address.
Parameters: the address, the data to be written, the size 2 or 4
Return: True = Success or False = Failed
*/

__attribute__((visibility("hidden"))) bool writeData(vm_address_t offset,  unsigned int data, int size) {

    //declaring variables

    kern_return_t err;
    mach_port_t port = mach_task_self();
    vm_address_t address = calculateAddress(offset);

    //set memory protections to allow us writing code there

    err = vm_protect(port, (vm_address_t) address, sizeof(data), NO,VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);

    //check if the protection fails

    if (err != KERN_SUCCESS) {

       return FALSE;

    }
    
    //write code to memory

    if (size == 4) {
  
    *(uint32_t*)address = CFSwapInt32(data);

    } else if (size == 2) {

    *(uint16_t*)address = CFSwapInt16(data);

    }
    
    //set the protections back to normal so the app can access this address as usual

    err = vm_protect(port, (vm_address_t)address, sizeof(data), NO,VM_PROT_READ | VM_PROT_EXECUTE);

      return TRUE;
	
}