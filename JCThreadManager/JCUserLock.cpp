#include "StdAfx.h"
#include "JCUserLock.h"


JCUserLock::JCUserLock(void)
	:m_hEvent(NULL)
{
}


JCUserLock::~JCUserLock(void)
{
	if (m_hEvent)
	{
		
	}
}

void JCUserLock::Init(string name)
{
	string lockname = "";
	if(name != "")
		lockname = "jc_"+name;
	m_hEvent = CreateEventA(NULL, FALSE, TRUE, lockname.c_str());
	if(!m_hEvent)
		m_hEvent = OpenEventA(EVENT_ALL_ACCESS,true,lockname.c_str());
}

void JCUserLock::Lock(DWORD dwMilliseconds)
{
	if (m_hEvent)
	{
		WaitForSingleObject(m_hEvent, dwMilliseconds);
	}
}

void JCUserLock::UnLock()
{
	if (m_hEvent)
	{
		SetEvent(m_hEvent);
	}
}