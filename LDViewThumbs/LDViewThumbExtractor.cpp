// LDViewThumbExtractor.cpp : Implementation of CLDViewThumbExtractor
#include "stdafx.h"
#include "LDViewThumbs.h"
#include "LDViewThumbExtractor.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCImage.h>

#define INSTALL_PATH_KEY "InstallPath"

/////////////////////////////////////////////////////////////////////////////
// CLDViewThumbExtractor

bool CLDViewThumbExtractor::findLDView(void)
{
	TCUserDefaults::setAppName("Travis Cobbs/LDView");
	char *lDViewDir = TCUserDefaults::stringForKey(INSTALL_PATH_KEY);

	if (lDViewDir)
	{
		FILE *lDView;

		m_ldviewPath = lDViewDir;
		m_ldviewPath += "\\LDView.exe";
		delete lDViewDir;
		lDView = fopen(m_ldviewPath.c_str(), "rb");
		if (lDView)
		{
			fclose(lDView);
			return true;
		}
		else
		{
			m_ldviewPath = "";
		}
	}
	return false;
}

bool CLDViewThumbExtractor::processFile(
	const wchar_t *datPath,
	const char *imageFilename)
{
	wchar_t commandLine[1024];
	STARTUPINFOW startupInfo;
	PROCESS_INFORMATION processInfo;
	int windowWidth = m_size.cx;
	int windowHeight = m_size.cy;
	DWORD priority = NORMAL_PRIORITY_CLASS;

	if (windowWidth < 320)
	{
		windowWidth = 320;
	}
	if (windowHeight < 240)
	{
		windowHeight = 240;
	}
	wsprintfW(commandLine, L"\"%S\" \"%s\" -SaveSnapshot=%S -SaveActualSize=0 "
		L"-SaveWidth=%d -SaveHeight=%d "
		L"-WindowWidth=%d -WindowHeight=%d -SaveZoomToFit=1 "
		L"-PreferenceSet=Thumbnails -SnapshotSuffix=.bmp",
		m_ldviewPath.c_str(), datPath, imageFilename, m_size.cx, m_size.cy,
		windowWidth, windowHeight);
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USEPOSITION;
	startupInfo.dwX = 0;
	startupInfo.dwY = 0;
	USES_CONVERSION;
	if (CreateProcessW(A2W(m_ldviewPath.c_str()), commandLine, NULL, NULL, FALSE,
		DETACHED_PROCESS | priority, NULL, NULL, &startupInfo, &processInfo))
	{
		while (1)
		{
			DWORD exitCode;

			GetExitCodeProcess(processInfo.hProcess, &exitCode);
			if (exitCode != STILL_ACTIVE)
			{
				printf("Done.\n");
				return true;
			}
			Sleep(50);
		}
	}
	else
	{
		DWORD error = GetLastError();
		char *buf;

		printf("Failed!\n");
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf,
			0, NULL);
		printf("%s\n", buf);
		LocalFree(buf);
		return false;
	}
}



////////////////////////////////////////////////////////////////////////////
// IPersistFile Begin
////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CLDViewThumbExtractor::Load(
	/* [in] */ LPCOLESTR pszFileName,
	/* [in] */ DWORD /*dwMode*/)
{
	if (!findLDView())
	{
		return E_FAIL;
	}
	m_path = pszFileName;
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// IPersistFile End
////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
// IExtractImage2 Begin
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CLDViewThumbExtractor::GetLocation( 
	/* [size_is][out] */ LPWSTR pszPathBuffer,
	/* [in] */ DWORD cch,
	/* [unique][out][in] */ DWORD * /*pdwPriority*/,
	/* [in] */ const SIZE *prgSize,
	/* [in] */ DWORD /*dwRecClrDepth*/,
	/* [out][in] */ DWORD *pdwFlags)
{
	if (cch > 0)
	{
		pszPathBuffer[0] = 0;
	}
	m_size = *prgSize;
	*pdwFlags |= IEIFLAG_CACHE;
	if (*pdwFlags & IEIFLAG_ASYNC)
	{
		return E_PENDING;
	}
	else
	{
		return S_OK;
	}
}

STDMETHODIMP CLDViewThumbExtractor::Extract(/* [out] */ HBITMAP *phBmpThumbnail)
{
	HRESULT hr = E_FAIL;

	if (findLDView())
	{
		TCHAR szTempPath[1024];
		TCHAR szTempFilename[MAX_PATH + 16];

		if (GetTempPath(sizeof(szTempPath) / sizeof(szTempPath[0]), szTempPath) == 0)
		{
			strcpy(szTempPath, "C:\\Temp");
		}
		if (GetTempFileName(szTempPath, "LDVThumb", 0, szTempFilename))
		{
			if (processFile(m_path.c_str(), szTempFilename))
			{
				TCImage *image = new TCImage;

				image->setFlipped(true);
				image->setLineAlignment(4);
				if (image->loadFile(szTempFilename))
				{
					BITMAPINFO bmInfo;
					TCByte *bmData;
					TCByte *imageData;
					int imageWidth;
					int imageHeight;
					int rowSize;
					int row;
					int col;
					HBITMAP hbm;

					SetLastError(0);
					bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);
					bmInfo.bmiHeader.biWidth = image->getWidth();
					bmInfo.bmiHeader.biHeight = image->getHeight();
					bmInfo.bmiHeader.biPlanes = 1;
					bmInfo.bmiHeader.biBitCount = 24;
					bmInfo.bmiHeader.biCompression = BI_RGB;
					bmInfo.bmiHeader.biSizeImage = 0;
					bmInfo.bmiHeader.biXPelsPerMeter = 1;
					bmInfo.bmiHeader.biYPelsPerMeter = 1;
					bmInfo.bmiHeader.biClrUsed = 0;
					bmInfo.bmiHeader.biClrImportant = 0;
					HDC hMEMDC = CreateCompatibleDC(NULL);
					hbm = CreateDIBSection(hMEMDC, &bmInfo, DIB_RGB_COLORS,
						(void**)&bmData, NULL, 0);
					imageData = image->getImageData();
					imageWidth = image->getWidth();
					imageHeight = image->getHeight();
					rowSize = image->getRowSize();
					for (row = 0; row < imageHeight; row++)
					{
						for (col = 0; col < imageWidth; col++)
						{
							int offset = row * rowSize + col * 3;

							bmData[offset] = imageData[offset + 2];
							bmData[offset + 1] = imageData[offset + 1];
							bmData[offset + 2] = imageData[offset];
						}
					}
					*phBmpThumbnail = hbm;
					hr = S_OK;
				}
				image->release();
			}
			DeleteFile(szTempFilename);
		}
	}
	else
	{
		*phBmpThumbnail = NULL;
	}
	return hr;
}

STDMETHODIMP CLDViewThumbExtractor::GetDateStamp(/* [out] */ FILETIME *pDateStamp)
{
    FILETIME ftCreate, ftAccess, ftWrite;
	HANDLE hFile = CreateFileW(m_path.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hFile)
	{
		return E_ACCESSDENIED;
	}
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
	{
		CloseHandle(hFile);
		return E_ACCESSDENIED;
	}
	*pDateStamp = ftWrite;
	CloseHandle(hFile);
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// IExtractImage2 End
////////////////////////////////////////////////////////////////////////////
