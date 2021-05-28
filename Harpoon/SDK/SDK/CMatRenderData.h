#pragma once
#include <cstdint>

#include "ModelRenderSystem.h"
//class IMatRenderContext {

//};
#if 0
class CMatRenderDataReference
{
public:
	CMatRenderDataReference();
	CMatRenderDataReference(IMatRenderContext* pRenderContext);
	~CMatRenderDataReference();
	void Lock(IMatRenderContext* pRenderContext);
	void Release();

private:
	IMatRenderContext* m_pRenderContext;
};


inline CMatRenderDataReference::CMatRenderDataReference()
{
	m_pRenderContext = NULL;
}

inline CMatRenderDataReference::CMatRenderDataReference(IMatRenderContext* pRenderContext)
{
	m_pRenderContext = NULL;
	Lock(pRenderContext);
}

inline CMatRenderDataReference::~CMatRenderDataReference()
{
	Release();
}

inline void CMatRenderDataReference::Lock(IMatRenderContext* pRenderContext)
{
	if (!m_pRenderContext)
	{
		m_pRenderContext = pRenderContext;
		//m_pRenderContext->AddRefRenderData();
	}
}

inline void CMatRenderDataReference::Release()
{
	if (m_pRenderContext)
	{
		//m_pRenderContext->ReleaseRenderData();
		m_pRenderContext = NULL;
	}
}


//-----------------------------------------------------------------------------
// Utility class for locking/unlocking render data
//-----------------------------------------------------------------------------
template< typename E >
class CMatRenderData
{
public:
	CMatRenderData(IMatRenderContext* pRenderContext);
	CMatRenderData(IMatRenderContext* pRenderContext, int nCount, const E* pSrcData = NULL);
	~CMatRenderData();
	E* Lock(int nCount, const E* pSrcData = NULL);
	void Release();
	bool IsValid() const;
	const E* Base() const;
	E* Base();
	const E& operator[](int i) const;
	E& operator[](int i);

private:
	IMatRenderContext* m_pRenderContext;
	E* m_pRenderData;
	int m_nCount;
	bool m_bNeedsUnlock;
};

template< typename E >
inline CMatRenderData<E>::CMatRenderData(IMatRenderContext* pRenderContext)
{
	m_pRenderContext = pRenderContext;
	m_nCount = 0;
	m_pRenderData = 0;
	m_bNeedsUnlock = false;
}

template< typename E >
inline CMatRenderData<E>::CMatRenderData(IMatRenderContext* pRenderContext, int nCount, const E* pSrcData)
{
	m_pRenderContext = pRenderContext;
	m_nCount = 0;
	m_pRenderData = 0;
	m_bNeedsUnlock = false;
	Lock(nCount, pSrcData);
}

template< typename E >
inline CMatRenderData<E>::~CMatRenderData()
{
	Release();
}

template< typename E >
inline bool CMatRenderData<E>::IsValid() const
{
	return m_pRenderData != NULL;
}

template< typename E >
inline E* CMatRenderData<E>::Lock(int nCount, const E* pSrcData)
{
	m_nCount = nCount;
	if (pSrcData)
	{
		// Yes, we're const-casting away, but that should be ok since 
		// the src data is render data
		m_pRenderData = const_cast<E*>(pSrcData);
		//m_pRenderContext->AddRefRenderData();
		m_bNeedsUnlock = false;
		return m_pRenderData;
	}
	//m_pRenderData = m_pRenderContext->LockRenderDataTyped<E>(nCount, pSrcData);
	m_bNeedsUnlock = true;
	return m_pRenderData;
}

template< typename E >
inline void CMatRenderData<E>::Release()
{
	if (m_pRenderContext && m_pRenderData)
	{
		if (m_bNeedsUnlock)
		{
			//m_pRenderContext->UnlockRenderData(m_pRenderData);
		}
		else
		{
			//m_pRenderContext->ReleaseRenderData();
		}
	}
	m_pRenderData = NULL;
	m_nCount = 0;
	m_bNeedsUnlock = false;
}

template< typename E >
inline E* CMatRenderData<E>::Base()
{
	return m_pRenderData;
}

template< typename E >
inline const E* CMatRenderData<E>::Base() const
{
	return m_pRenderData;
}

template< typename E >
inline E& CMatRenderData<E>::operator[](int i)
{
	//Assert((i >= 0) && (i < m_nCount));
	return m_pRenderData[i];
}

template< typename E >
inline const E& CMatRenderData<E>::operator[](int i) const
{
	//Assert((i >= 0) && (i < m_nCount));
	return m_pRenderData[i];
}
#endif