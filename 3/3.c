#include <efi.h>
#include <efilib.h>
EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(image, systab);
	UINTN mms = 0, mk, ds, dv;
	int i, s;
	EFI_MEMORY_DESCRIPTOR *mm;
	
	uefi_call_wrapper(systab->BootServices->GetMemoryMap, 5, &mms, mm, &mk, &ds, &dv);
	if (EFI_SUCCESS != uefi_call_wrapper(systab->BootServices->AllocatePool, 3, EfiLoaderData, mms, ((void*)&mm)))
		Print(L"Cannot allocate pool!\r\n");
	if (EFI_SUCCESS != uefi_call_wrapper(systab->BootServices->GetMemoryMap, 5, &mms, mm, &mk, &ds, &dv))
		Print(L"Error!\r\n");
	
	for (i = s = 0; i < mms; i++)
		if (mm[i].Type==EfiBootServicesCode || mm[i].Type==EfiBootServicesData || mm[i].Type==EfiConventionalMemory)
			s += mm[i].NumberOfPages;
	Print(L"Used %d bytes of memory.\r\n",s*4096);
	
	return uefi_call_wrapper(systab->BootServices->FreePool, 1, mm);
}
