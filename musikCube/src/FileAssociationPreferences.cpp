///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCubeApp.h"
#include "FileAssociationPreferences.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// FileAssociationPreferences
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(FileAssociationPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(FileAssociationPreferences, PreferencesPropertyPage)
END_MESSAGE_MAP()

const std::wstring FileAssociationPreferences::s_CubeFileClass = L"musikCube.audio";

/*ctor*/ FileAssociationPreferences::FileAssociationPreferences()
: PreferencesPropertyPage(FileAssociationPreferences::IDD)
{
}

FileAssociationPreferences::~FileAssociationPreferences()
{
    while (this->m_AssocationMap.size() > 0)
    {
        delete this->m_AssocationMap.begin()->second;
        this->m_AssocationMap.erase(this->m_AssocationMap.begin());
    }
}

void FileAssociationPreferences::DoDataExchange(CDataExchange* pDX)
{
    PreferencesPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXTENSION_LIST, m_ExtensionListCtrl);
}

void FileAssociationPreferences::LoadPrefs()
{
    this->m_ExtensionListCtrl.SetExtendedStyle(
        this->m_ExtensionListCtrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
    //
    this->m_ExtensionListCtrl.InsertColumn(0, L"Extension", LVCFMT_LEFT, 100);

    musik::Core::StringArray formats = musik::Core::GetPlayerFormats();

    this->m_AssocationMap.clear();
    for (size_t i = 0; i < formats.size(); i++)
    {
        this->m_ExtensionListCtrl.InsertItem(i, formats[i]);
        this->m_AssocationMap[i] = new Association(formats[i]);
        this->m_ExtensionListCtrl.SetCheck(i, this->m_AssocationMap[i]->IsAssociated());
    }
}

void FileAssociationPreferences::CommitChanges()
{
    this->CreateOrUpdateFileClass();

    for (int i = 0; i < this->m_ExtensionListCtrl.GetItemCount(); i++)
    {
        Association* assoc = this->m_AssocationMap[i];

        (this->m_ExtensionListCtrl.GetCheck(i) == TRUE) ? 
            assoc->SetAssociation() : assoc->UnsetAssociation();
    }

    ::SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
}

BOOL FileAssociationPreferences::OnInitDialog()
{
    this->m_Modified = true;
    return PreferencesPropertyPage::OnInitDialog();
}

void FileAssociationPreferences::CreateOrUpdateFileClass()
{
    wchar_t exePath[MAX_PATH];
    ::GetModuleFileName(NULL, exePath, MAX_PATH);

    HKEY fileClassKey;
    // RegCreateKey will create the key if it doesn't exist, or open it if it does.
    if (::RegCreateKey(HKEY_CLASSES_ROOT, s_CubeFileClass.c_str(), &fileClassKey) == ERROR_SUCCESS)
    {
        // musikCube.audio = "musikCube audio file"
        wchar_t* valueString = L"musikCube audio file";
        int valueSize = ::wcslen(valueString) * sizeof(wchar_t);
        if (::RegSetValue(fileClassKey, NULL, REG_SZ, valueString, valueSize) == ERROR_SUCCESS)
        {
            // musikCube.Audio -> DefaultIcon = "...\musikCube.exe",1
            HKEY defaultIconKey;
            ::RegCreateKey(fileClassKey, L"DefaultIcon", &defaultIconKey);
            wchar_t buffer[512];
            ::wsprintf(buffer, L"\"%ls\",1", exePath);
            valueSize = ::wcslen(buffer) * sizeof(wchar_t);
            ::RegSetValue(defaultIconKey, NULL, REG_SZ, buffer, valueSize);
            ::RegCloseKey(defaultIconKey);

            // musikCube.Audio -> Shell = "Open"
            HKEY shellKey;
            valueString = L"Open";
            valueSize = ::wcslen(valueString) * sizeof(wchar_t);
            ::RegCreateKey(fileClassKey, L"shell", &shellKey);
            ::RegSetValue(shellKey, NULL, REG_SZ, valueString, valueSize);
            {
                // musikCube.Audio -> Shell -> Open
                HKEY shellOpenKey;
                ::RegCreateKey(shellKey, L"Open", &shellOpenKey);
                {
                    // musikCube.Audio -> Shell -> Open -> command = "...\musikCube.exe" "%1"
                    HKEY commandKey;
                    ::wsprintf(buffer, L"\"%ls\" \"%%1\"", exePath);
                    valueSize = ::wcslen(buffer) * sizeof(wchar_t);
                    ::RegCreateKey(shellOpenKey, L"command", &commandKey);
                    ::RegSetValue(commandKey, NULL, REG_SZ, buffer, valueSize);
                    ::RegCloseKey(commandKey);
                }
                ::RegCloseKey(shellOpenKey);
            }
            ::RegCloseKey(shellKey);
        }

        ::RegCloseKey(fileClassKey);
    }
}

///////////////////////////////////////////////////
// FileAssociationPreferences::Assocation
///////////////////////////////////////////////////

const std::wstring FileAssociationPreferences::Association::s_OldKeyName = L"musikCube.backup";

/*ctor*/ FileAssociationPreferences::Association::Association(const std::wstring& extension)
{
    this->m_Extension = L"." + extension;
    this->LoadFileClass();
}

void FileAssociationPreferences::Association::SetAssociation()
{
    if ( ! this->IsAssociated())
    {
        if (this->SetFileClass(FileAssociationPreferences::s_CubeFileClass, this->m_CurrentFileClass))
        {
            this->m_CurrentFileClass = FileAssociationPreferences::s_CubeFileClass;
        }
    }
}

void FileAssociationPreferences::Association::UnsetAssociation()
{
    if (this->IsAssociated())
    {
        if (this->SetFileClass(this->m_OldFileClass, this->m_CurrentFileClass))
        {
            this->m_CurrentFileClass = m_OldFileClass;
        }
    }
}

bool FileAssociationPreferences::Association::IsAssociated()
{
    bool result = (::wcscmp(this->m_CurrentFileClass.c_str(), FileAssociationPreferences::s_CubeFileClass.c_str()) == 0);
    return result;
}

bool FileAssociationPreferences::Association::SetFileClass(const std::wstring& newFileClass, const std::wstring& oldFileClass)
{
    bool result = false;

    HKEY registryKey;
    // RegCreateKey will create the key if it doesn't exist, or open it if it does.
    if (::RegCreateKey(HKEY_CLASSES_ROOT, this->m_Extension.c_str(), &registryKey) == ERROR_SUCCESS)
    {
        // NEW ASSOCIATION
        //
        BYTE* buffer = (BYTE*) newFileClass.c_str();
        int bufferSize = (::wcslen(newFileClass.c_str()) + 1) * sizeof(wchar_t);
        //
        if (::RegSetValueEx(registryKey, NULL, NULL, REG_SZ, buffer, bufferSize) == ERROR_SUCCESS)
        {
            result = true;
        }

        // OLD ASSOCIATION
        //
        buffer = (BYTE*) oldFileClass.c_str();
        bufferSize = (::wcslen(oldFileClass.c_str()) + 1) * sizeof(wchar_t);
        //
        if (::RegSetValueEx(registryKey, s_OldKeyName.c_str(), NULL, REG_SZ, buffer, bufferSize) == ERROR_SUCCESS)
        {
            result &= true;
        }

        if (::RegCloseKey(registryKey) != ERROR_SUCCESS)
        {
            throw L"Failed to close registry key";
        }
    }

    return result;
}

void FileAssociationPreferences::Association::LoadFileClass()
{
    bool result = false;

    HKEY registryKey;
    // RegCreateKey will create the key if it doesn't exist, or open it if it does.
    if (::RegCreateKey(HKEY_CLASSES_ROOT, this->m_Extension.c_str(), &registryKey) == ERROR_SUCCESS)
    {
        DWORD bufferSize = 0;

        // DEFAULT (CURRENT)
        if (::RegQueryValueEx(registryKey, NULL, NULL, NULL, NULL, &bufferSize) == ERROR_SUCCESS)
        {
            BYTE* buffer = new BYTE[bufferSize];

            if (::RegQueryValueEx(registryKey, NULL, NULL, NULL, buffer, &bufferSize) == ERROR_SUCCESS)
            {
                this->m_CurrentFileClass = (wchar_t*)buffer;
            }

            delete buffer;
        }

        // PREVIOUS VALUE
        if (::RegQueryValueEx(registryKey, s_OldKeyName.c_str(), NULL, NULL, NULL, &bufferSize) == ERROR_SUCCESS)
        {
            BYTE* buffer = new BYTE[bufferSize];

            if (::RegQueryValueEx(registryKey, s_OldKeyName.c_str(), NULL, NULL, buffer, &bufferSize) == ERROR_SUCCESS)
            {
                this->m_OldFileClass = (wchar_t*)buffer;

                if (this->m_OldFileClass == FileAssociationPreferences::s_CubeFileClass)
                {
                    this->m_OldFileClass = L"";
                }
            }

            delete buffer;
        }

        ::RegCloseKey(registryKey);
    }
}

///////////////////////////////////////////////////

} } // namespace musik::Cube