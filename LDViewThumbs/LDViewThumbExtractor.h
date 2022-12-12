// LDViewThumbExtractor.h : Declaration of the CLDViewThumbExtractor

#ifndef __LDVIEWTHUMBEXTRACTOR_H_
#define __LDVIEWTHUMBEXTRACTOR_H_

#include <TCFoundation/TCStlIncludes.h>

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLDViewThumbExtractor
class ATL_NO_VTABLE CLDViewThumbExtractor : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLDViewThumbExtractor, &CLSID_LDViewThumbExtractor>,
	public IPersistFile,
	public IExtractImage2,
	public ILDViewThumbExtractor
{
public:
	CLDViewThumbExtractor(): m_size({0, 0})
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LDVIEWTHUMBEXTRACTOR)
DECLARE_NOT_AGGREGATABLE(CLDViewThumbExtractor)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CLDViewThumbExtractor)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IExtractImage)
	COM_INTERFACE_ENTRY(IExtractImage2)
	COM_INTERFACE_ENTRY(ILDViewThumbExtractor)
END_COM_MAP()


// ILDViewThumbExtractor
public:
	////////////////////////////////////////////////////////////////////////////
	// IPersistFile Begin
	////////////////////////////////////////////////////////////////////////////
	STDMETHOD(GetClassID)(CLSID * pClassID)
	{
		*pClassID = CLSID_LDViewThumbExtractor;
		return S_OK;
	}
	STDMETHOD(IsDirty)(void)
	{
		return S_FALSE;
	}
	STDMETHOD(Load)(/* [in] */ LPCOLESTR pszFileName, /* [in] */ DWORD dwMode);
	STDMETHOD(Save)(/* [unique][in] */ LPCOLESTR /*pszFileName*/,
		/* [in] */ BOOL /*fRemember*/)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SaveCompleted)(/* [unique][in] */ LPCOLESTR /*pszFileName*/)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetCurFile)(/* [out] */ LPOLESTR * /*ppszFileName*/)
	{
		return E_NOTIMPL;
	}
	////////////////////////////////////////////////////////////////////////////
	// IPersistFile End
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// IExtractImage2 Begin
	////////////////////////////////////////////////////////////////////////////
	STDMETHOD(GetLocation)( 
		/* [size_is][out] */ LPWSTR pszPathBuffer,
		/* [in] */ DWORD cch,
		/* [unique][out][in] */ DWORD *pdwPriority,
		/* [in] */ const SIZE *prgSize,
		/* [in] */ DWORD dwRecClrDepth,
		/* [out][in] */ DWORD *pdwFlags);
	STDMETHOD(Extract)(/* [out] */ HBITMAP *phBmpThumbnail);
	STDMETHOD(GetDateStamp)(/* [out] */ FILETIME *pDateStamp);
	////////////////////////////////////////////////////////////////////////////
	// IExtractImage2 End
	////////////////////////////////////////////////////////////////////////////

private:
	bool findLDView(void);
	bool isLDrawFile(void);
	bool processFile(const wchar_t *datFilename, const wchar_t *imageFilename);

	std::wstring m_path;
	std::wstring m_ldviewDir;
	std::wstring m_ldviewPath;
	SIZE m_size;
};

#endif //__LDVIEWTHUMBEXTRACTOR_H_
