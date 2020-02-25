// TestThreadManager.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "JCThreadManager.h"
using namespace std;

int loop = 0;
int loop1 = 0;

DWORD WINAPI TestThreadProc0(JCThreadManager* pThreadManager)
{
	char buf[256]={0};
	sprintf(buf,(const char*)pThreadManager->GetParam());
	while(true)
	{
		if (pThreadManager->IsClose())
			break;
		printf("%s:%d\n",buf,loop++);
	}
	return 0;
}

DWORD WINAPI TestThreadProc1(JCThreadManager* pThreadManager)
{
	char buf[256]={0};
	sprintf(buf,(const char*)pThreadManager->GetParam());
	while(true)
	{
		if (pThreadManager->IsClose())
			break;
		printf("%s:%d\n",buf,loop++);
	}
	return 0;
}

DWORD WINAPI TestThreadProc2(JCThreadManager* pThreadManager)
{
	char buf[256]={0};
	sprintf(buf,(const char*)pThreadManager->GetParam());
	while(true)
	{
		if (pThreadManager->IsClose())
			break;
		pThreadManager->Lock();
		printf("%s:%d\n",buf,loop1++);
		pThreadManager->UnLock();
	}
	return 0;
}

DWORD WINAPI TestThreadProc3(JCThreadManager* pThreadManager)
{
	char buf[256]={0};
	sprintf(buf,(const char*)pThreadManager->GetParam());
	while(true)
	{
		if (pThreadManager->IsClose())
			break;
		pThreadManager->Lock();
		printf("%s:%d\n",buf,loop1++);
		pThreadManager->UnLock();
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	JCThreadManager jc0,jc1,jc2,jc3;
	//jc0 jc1输出顺序混乱
	jc0.InitManager(TestThreadProc0,"test0");
	jc1.InitManager(TestThreadProc1,"test1",false);
	jc1.Start();//手动启动
	Sleep(2000);
	jc0.End();//结束线程，IsClose()返回true
	jc1.End();
	jc1.WaitForClose(2000);
	jc0.WaitForClose();//等待结束，默认是INFINITE


	getchar();
	//jc2 jc3输出顺序正常
	jc2.InitManager(TestThreadProc2,"test2",true,"testlock");
	jc3.InitManager(TestThreadProc3,"test3",true,"testlock");
	Sleep(2000);
	//jc2 jc3程序退出自动关闭
	return 0;
}

