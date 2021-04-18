#pragma warning(disable: 4996) // wcsnicmp deprecated

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#pragma warning(push)
#pragma warning(disable: 4005) //macro redefinition
#include <ntstatus.h>
#pragma warning(pop)

#include "ntos.h"
#include <string>



namespace nt {
    DWORD get_path(HANDLE file, std::wstring& nspath);
    bool find_sym_link(std::wstring dir, std::wstring name, std::wstring& out);
    HANDLE open_directory(HANDLE root_handle, std::wstring& dir, ACCESS_MASK desired_access);
}