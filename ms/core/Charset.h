#pragma once
#include <tchar.h>
#include <stringapiset.h>
#include <stdlib.h>
#include <string>

namespace ms {

class Charset
{
public:
    static std::wstring ANSIToUnicode(const char* ansi)
    {
        int textlen = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
        wchar_t * pBuffer = new wchar_t[textlen + 1];
        memset(pBuffer, 0, (textlen + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, ansi, -1, pBuffer, textlen);
        std::wstring unicode(pBuffer);
        delete[] pBuffer;
        return unicode;
    }

    static BOOL ANSIToUnicode(wchar_t* dst, const int size, const char* ansi)
    {
        BOOL bResult{ FALSE };
        int textlen = 0;
        textlen = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
        if (size > textlen)
        {
            // memset(dst, 0, (textlen + 1) * sizeof(wchar_t));
            if (0 != MultiByteToWideChar(CP_ACP, 0, ansi, -1, dst, textlen))
            {
                bResult = TRUE;
            }
            dst[textlen] = 0;
        }
        return bResult;
    }

    static std::string UnicodeToANSI(const wchar_t *unicode)
    {
        int textlen = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
        char * pBuffer = new char[textlen + 1];
        memset(pBuffer, 0, sizeof(char) * (textlen + 1));
        WideCharToMultiByte(CP_ACP, 0, unicode, -1, pBuffer, textlen, NULL, NULL);
        std::string ansi{ pBuffer };
        delete[] pBuffer;
        return ansi;
    }

    static BOOL UnicodeToANSI(char* dst, const int size, const wchar_t *unicode)
    {
        BOOL bResult{ FALSE };
        int textlen = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
        if (size > textlen)
        {
            //memset(dst, 0, sizeof(char) * (textlen + 1));
            if (0 != WideCharToMultiByte(CP_ACP, 0, unicode, -1, dst, textlen, NULL, NULL))
            {
                bResult = TRUE;
            }
            dst[textlen] = 0;
        }
        return bResult;
    }

    static std::wstring UTF8ToUnicode(const char* utf8)
    {
        int textlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        wchar_t * pBuffer = new wchar_t[textlen + 1];
        memset(pBuffer, 0, (textlen + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)pBuffer, textlen);
        std::wstring unicode{ pBuffer };
        delete[]pBuffer;
        return unicode;
    }

    static std::wstring UTF8ToUnicode(const wchar_t* utf8)
    {
        std::string u = UnicodeToANSI(utf8);
        int textlen = MultiByteToWideChar(CP_UTF8, 0, u.c_str(), -1, NULL, 0);
        wchar_t * pBuffer = new wchar_t[textlen + 1];
        memset(pBuffer, 0, (textlen + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, u.c_str(), -1, (LPWSTR)pBuffer, textlen);
        std::wstring unicode{ pBuffer };
        delete[]pBuffer;
        return unicode;
    }

    static BOOL UTF8ToUnicode(wchar_t*dst, const int size, const char* utf8)
    {
        BOOL bResult{ FALSE };
        int textlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (size > textlen)
        {
            // memset(dst, 0, (textlen + 1) * sizeof(wchar_t));
            if (0 != MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)dst, textlen))
            {
                bResult = TRUE;
            }
            dst[textlen] = 0;
        }
        return bResult;
    }

    static std::string UnicodeToUTF8(const wchar_t *unicode)
    {
        int textlen = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
        char * pBuffer = new char[textlen + 1];
        memset(pBuffer, 0, sizeof(char) * (textlen + 1));
        WideCharToMultiByte(CP_UTF8, 0, unicode, -1, pBuffer, textlen, NULL, NULL);
        std::string utf8{ pBuffer };
        delete[] pBuffer;
        return utf8;
    }

    static BOOL UnicodeToUTF8(char* dst, const int size, const wchar_t *unicode)
    {
        BOOL bResult{ FALSE };
        int textlen = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
        if (size > textlen)
        {
            // memset(dst, 0, sizeof(char) * (textlen + 1));
            if (0 != WideCharToMultiByte(CP_UTF8, 0, unicode, -1, dst, textlen, NULL, NULL))
            {
                bResult = TRUE;
            }
            dst[textlen] = 0;
        }
        return bResult;
    }
    // char utf8 -> unicode -> ansi
    static std::string UTF8ToANSI(const char* utf8)
    {
        std::wstring unicode = UTF8ToUnicode(utf8);
        std::string ansi = UnicodeToANSI(unicode.c_str());
        return ansi;
    }

#ifdef _UNICODE
    // wchar utf8 -> char utf8 -> unicode
    static std::wstring UTF8ToLocal(const wchar_t* utf8)
    {
        std::wstring unicode = UTF8ToUnicode(utf8);
        return unicode;
    }
#else
    // char utf8 -> unicode -> ansi
    static std::string UTF8ToLocal(const char* utf8)
    {
        std::string ansi = UTF8ToANSI(utf8);
        return ansi;
    }
#endif
};


};// ms
