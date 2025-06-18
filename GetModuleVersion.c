// GetModuleVersion.cpp
// License: MIT
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

typedef struct tagLANGANDCODEPAGE
{
    WORD wLanguage;
    WORD wCodePage;
} LANGANDCODEPAGE, *PLANGANDCODEPAGE;

HRESULT GetModuleVersion(PCSTR pszFileName, PSTR *ppszDest)
{
    DWORD dwHandle;
    PBYTE pbData;
    PVOID pvData;

    *ppszDest = NULL;

    UINT size = GetFileVersionInfoSizeA(pszFileName, &dwHandle);
    if (!size)
    {
        printf("No version info\n");
        return E_FAIL;
    }
    pbData = (PBYTE)LocalAlloc(LPTR, size);
    if (!pbData)
    {
        printf("E_OUTOFMEMORY\n");
        return E_OUTOFMEMORY;
    }
    GetFileVersionInfoA(pszFileName, dwHandle, size, pbData);

    HRESULT hr = E_FAIL;
    if (VerQueryValueA(pbData, "\\VarFileInfo\\Translation", &pvData, &size))
    {
        PVOID pDataSaved = pvData;
        PLANGANDCODEPAGE pEntry = (PLANGANDCODEPAGE)pvData;
        for (; (PBYTE)pEntry + sizeof(LANGANDCODEPAGE) <= (PBYTE)pDataSaved + size; ++pEntry)
        {
            CHAR szPath[MAX_PATH];
            wnsprintfA(szPath, _countof(szPath), "\\StringFileInfo\\%04X%04X\\ProductVersion",
                       pEntry->wLanguage, pEntry->wCodePage);
            if (VerQueryValueA(pbData, szPath, &pvData, &size) && size)
            {
                // NOTE: *ppszDest must be freed using LocalFree later
                *ppszDest = StrDupA((PSTR)pvData);
                hr = S_OK;
            }
        }
    }

    if (hr != S_OK)
        printf("No ProductVersion\n");
    LocalFree(pbData);
    return hr;
}

int main(void)
{
    PSTR pszDest;
    HRESULT hr = GetModuleVersion("..\\mspaint.exe", &pszDest);
    printf("%s\n", pszDest);
    LocalFree(pszDest);
}
