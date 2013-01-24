#include <efi.h>
#include <efilib.h>

typedef struct {
  UINT32                Type;
  UINT32				Pad1;
  EFI_PHYSICAL_ADDRESS  PhysicalStart;
  EFI_VIRTUAL_ADDRESS   VirtualStart;
  UINT64                NumberOfPages;
  UINT64                Attribute;
  UINT64				Pad2;
} EFI_MEMORY_DESCRIPTOR1;

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(image, systab);
	UINTN mms, mk, ds, i;
	UINT32 dv;
	UINT64 s, as;
	EFI_MEMORY_DESCRIPTOR *mm;
	EFI_MEMORY_DESCRIPTOR1 *mm1;
	EFI_STATUS er;
	
	mms = 0;
	er = uefi_call_wrapper(systab->BootServices->GetMemoryMap, 5, &mms, mm, &mk, &ds, &dv);
		if (er != EFI_BUFFER_TOO_SMALL){Print (L"Something is wrong!\r\n"); return er;}
	er = uefi_call_wrapper(systab->BootServices->AllocatePool, 3, EfiLoaderData, mms, ((void*)&mm));
		if (er != EFI_SUCCESS){Print(L"Cannot allocate pool!\r\n");return er;}
	er = uefi_call_wrapper(systab->BootServices->GetMemoryMap, 5, &mms, mm, &mk, &ds, &dv);
		if (er != EFI_SUCCESS){Print(L"Something is wrong!\r\n");return er;}

	mms /= sizeof(EFI_MEMORY_DESCRIPTOR1);
	mm1 = (EFI_MEMORY_DESCRIPTOR1*)mm;
	s = 0; as = 0;
	for (i = 0; i < mms; i++)
		if (mm1[i].Type == EfiBootServicesCode || mm1[i].Type == EfiBootServicesData || mm1[i].Type == EfiConventionalMemory)
			s += mm1[i].NumberOfPages;
		else
			as += mm1[i].NumberOfPages;
	Print(L"%lld bytes (%lld pages) of memory are avaliable.\r\n", s * 4096, s);
	Print(L"%lld bytes (%lld pages) of memory are NOT avaliable for applications.\r\n", as * 4096, as);
	
	er = uefi_call_wrapper(systab->BootServices->FreePool, 1, mm);
		if (er != EFI_SUCCESS){Print(L"Cannot free pool!\r\n");return er;}
	return EFI_SUCCESS;
}
