#include "nt.h"
#include <iostream>

DWORD nt::get_path(HANDLE h_File, std::wstring& ps_NTPath)
{
    if (h_File == 0 || h_File == INVALID_HANDLE_VALUE)
        return ERROR_INVALID_HANDLE;

    BYTE  u8_Buffer[2000];
    DWORD u32_ReqLength = 0;

    UNICODE_STRING* pk_Info = &((OBJECT_NAME_INFORMATION*)u8_Buffer)->Name;
    pk_Info->Buffer = 0;
    pk_Info->Length = 0;

    // IMPORTANT: The return value from NtQueryObject is bullshit! (driver bug?)
    // - The function may return STATUS_NOT_SUPPORTED although it has successfully written to the buffer.
    // - The function returns STATUS_SUCCESS although h_File == 0xFFFFFFFF
    NtQueryObject(h_File, (OBJECT_INFORMATION_CLASS)1, u8_Buffer, sizeof(u8_Buffer), &u32_ReqLength);

    // On error pk_Info->Buffer is NULL
    if (!pk_Info->Buffer || !pk_Info->Length)
        return ERROR_FILE_NOT_FOUND;

    pk_Info->Buffer[pk_Info->Length / 2] = 0; // Length in Bytes!

    ps_NTPath = pk_Info->Buffer;
    return 0;
}

bool nt::find_sym_link(std::wstring dir, std::wstring name, std::wstring& out)
{
    HANDLE dir_handle = open_directory(NULL, dir, DIRECTORY_QUERY);
    if (!dir_handle)
        return false;

    NTSTATUS status;
    ULONG query_context = 0, length;
    POBJECT_DIRECTORY_INFORMATION objinf;
    bool found = false;

    do {

        //
        // Wine implementation of NtQueryDirectoryObject interface is very basic and incomplete.
        // It doesn't work if no input buffer specified and does not return required buffer size.
        //

        length = 0;
        status = NtQueryDirectoryObject(dir_handle, NULL, 0, TRUE, FALSE, &query_context, &length);
        if (status != STATUS_BUFFER_TOO_SMALL)
            break;

        objinf = (POBJECT_DIRECTORY_INFORMATION)malloc((SIZE_T)length);
        if (objinf == NULL)
            break;

        status = NtQueryDirectoryObject(dir_handle, objinf, length, TRUE, FALSE, &query_context, &length);
        if (!NT_SUCCESS(status)) {
            free(objinf);
            break;
        }

        std::wstring _name(objinf->Name.Buffer);

        if (_name.find(name) != std::wstring::npos)
        {
            found = true;
            out = _name;
            free(objinf);
            break;
        }

        free(objinf);

    } while (TRUE);

    NtClose(dir_handle);

    return found;
}

HANDLE nt::open_directory(HANDLE root_handle, std::wstring& dir, ACCESS_MASK desired_access)
{
    NTSTATUS status;
    HANDLE dir_handle = NULL;
    UNICODE_STRING us_dir;
    OBJECT_ATTRIBUTES obj_attr;

    RtlInitUnicodeString(&us_dir, dir.c_str());
    InitializeObjectAttributes(&obj_attr, &us_dir, OBJ_CASE_INSENSITIVE, root_handle, NULL);
    
    status = NtOpenDirectoryObject(&dir_handle, desired_access, &obj_attr);
   
    if (!NT_SUCCESS(status))
        return NULL;

    return dir_handle;
}
