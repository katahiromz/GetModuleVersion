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

// English (US) UTF-16
#define PRODUCT_VER_ENGLISH_US_UTF16    "\\StringFileInfo\\040904E4\\ProductVersion"
// German (Germany) UTF-16
#define PRODUCT_VER_GERMAN_UTF16        "\\StringFileInfo\\040704E4\\ProductVersion"
// English (US) Western European
#define PRODUCT_VER_ENGLISH_US_WE       "\\StringFileInfo\\040904B0\\ProductVersion"
// English (US) Neutral
#define PRODUCT_VER_ENGLISH_US_NEUTRAL  "\\StringFileInfo\\04090000\\ProductVersion"
// Swedish (Sweden) Western European
#define PRODUCT_VER_SWEDISH_WE          "\\StringFileInfo\\041D04B0\\ProductVersion"

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
    if ((VerQueryValueA(pbData, PRODUCT_VER_ENGLISH_US_UTF16,   &pvData, &size) ||
         VerQueryValueA(pbData, PRODUCT_VER_GERMAN_UTF16,       &pvData, &size) ||
         VerQueryValueA(pbData, PRODUCT_VER_ENGLISH_US_WE,      &pvData, &size) ||
         VerQueryValueA(pbData, PRODUCT_VER_ENGLISH_US_NEUTRAL, &pvData, &size) ||
         VerQueryValueA(pbData, PRODUCT_VER_SWEDISH_WE,         &pvData, &size)) && size)
    {
        // NOTE: *ppszDest must be freed using LocalFree later
        *ppszDest = StrDupA((PSTR)pvData);
        hr = *ppszDest ? S_OK : E_OUTOFMEMORY;
    }
    else if (VerQueryValueA(pbData, "\\VarFileInfo\\Translation", &pvData, &size))
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
                hr = *ppszDest ? S_OK : E_OUTOFMEMORY;
            }
        }
    }

    if (FAILED(hr))
        printf("hr: 0x%lX\n", hr);

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
