#include <Windows.h>
#include <Commctrl.h>
#include <vector>
#include "resource.h"
#include "HTTPDownloader.h"

#define WM_DOWNLOADSIZE WM_USER+1000
#define WM_DOWNLOADFAIL WM_USER+1001
#define WM_DOWNLOADFINISH WM_USER+1002

struct DOWNLOAD_PARAM
{
	char url[300];
	char local[300];
	HWND hwnd;
	volatile bool Stop;
};

DWORD WINAPI DownLoadProc(LPVOID Param)
{
	DOWNLOAD_PARAM * p=(DOWNLOAD_PARAM*)Param;

	std::vector<char> buf(1024*1024);

	FILE * f=NULL;
	void * h=NULL;
	f=fopen(p->local,"wb");
	if(NULL==f)
	{
		SendMessage(p->hwnd,WM_DOWNLOADFAIL,(WPARAM)p,(LPARAM)"本地文件打不开");
		goto lbl_exit;
	}
	h=StartHttpDownload(p->url);
	if(NULL==h)
	{
		SendMessage(p->hwnd,WM_DOWNLOADFAIL,(WPARAM)p,(LPARAM)"无法下载");
		goto lbl_exit;
	}

	unsigned Total=0;

	for(;;)
	{
		if(p->Stop)
		{
			SendMessage(p->hwnd,WM_DOWNLOADFAIL,(WPARAM)p,(LPARAM)"下载取消");
			break;
		}
		unsigned a;
		int r=ReadHttpData(h,&buf[0],buf.size(),&a);
		if(1==r)
		{
			SendMessage(p->hwnd,WM_DOWNLOADFINISH,(WPARAM)p,(LPARAM)"下载完成");
			break;
		}
		else if(2==r)
		{
			SendMessage(p->hwnd,WM_DOWNLOADFAIL,(WPARAM)p,(LPARAM)"下载中断");
			break;
		}
		if(a!=0)
		{
			fwrite(&buf[0],a,1,f);
			Total+=a;
			SendMessage(p->hwnd,WM_DOWNLOADSIZE,(WPARAM)p,Total);
		}
		Sleep(100);
	}

lbl_exit:
	if(f!=NULL)fclose(f);
	if(h!=NULL)StopHttpDownload(h);
	delete p;
	return 0;
}

BOOL WINAPI DialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			// 添加第一个列表框
			HWND h=GetDlgItem(hWnd,IDC_LIST1);
			LVCOLUMN column;
			column.mask = LVCF_TEXT|LVCF_WIDTH;
			column.cx = 300;
			column.pszText = "URL";
			SendMessage(h,LVM_INSERTCOLUMN,0,(LPARAM)&column);
			column.pszText = "本地路径";
			SendMessage(h,LVM_INSERTCOLUMN,1,(LPARAM)&column);
			column.cx = 100;
			column.pszText = "已下载";
			SendMessage(h,LVM_INSERTCOLUMN,2,(LPARAM)&column);

			// 添加第二个列表框
			HWND h1 = GetDlgItem(hWnd, IDC_LIST2);
			column.cx = 300;
			column.pszText = "URL";
			SendMessage(h1,LVM_INSERTCOLUMN,0,(LPARAM)&column);
			column.pszText = "本地路径";
			SendMessage(h1,LVM_INSERTCOLUMN,1,(LPARAM)&column);
			column.cx = 100;
			column.pszText = "状态";
			SendMessage(h1, LVM_INSERTCOLUMN, 2, (LPARAM)&column);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			char url[300];
			char local[300];
			if(LOWORD(wParam)==IDC_BTN_NEW)
			{
				// 添加项
				HWND h=GetDlgItem(hWnd,IDC_EDIT_URL);
				GetWindowText(h,url,sizeof url);
				h=GetDlgItem(hWnd,IDC_EDIT_LOCAL);
				GetWindowText(h,local,sizeof local);
				LVITEM it;
				it.mask=LVIF_TEXT;
				it.pszText=url;
				it.iItem=0;
				it.iSubItem=0;
				h=GetDlgItem(hWnd,IDC_LIST1);
				SendMessage(h,LVM_INSERTITEM,0,(LPARAM)&it);
				it.iSubItem=1;
				it.pszText=local;
				SendMessage(h,LVM_SETITEMTEXT,0,(LPARAM)&it);
				it.iSubItem=2;
				it.pszText="0";
				SendMessage(h,LVM_SETITEMTEXT,0,(LPARAM)&it);
				// 启动此项的下载
				DOWNLOAD_PARAM * dp=new DOWNLOAD_PARAM;
				dp->hwnd=hWnd;
				dp->Stop=false;
				strcpy(dp->url,url);
				strcpy(dp->local,local);
				memset(&it, 0, sizeof(LVITEM));
				it.mask = LVIF_PARAM;
				it.iItem = 0;
				it.lParam = (LPARAM)dp;
				SendMessage(h, LVM_SETITEM, 0, (LPARAM)&it);
				CreateThread(NULL,0,DownLoadProc,dp,0,NULL);
			}
			else if(LOWORD(wParam)==IDC_BTN_CANCEL)
			{
				HWND h=GetDlgItem(hWnd,IDC_LIST1);
				int n=SendMessage(h,LVM_GETNEXTITEM,-1,LVIS_SELECTED);
				if(-1==n)
				{
					MessageBox(hWnd,"没有选","选",MB_OK|MB_ICONEXCLAMATION);
				}
				else
				{
					LVITEM lvi;
					memset(&lvi, 0, sizeof(LVITEM));
					lvi.mask=LVIF_PARAM;
					lvi.iItem=n;
					SendMessage(h, LVM_GETITEM, 0, (LPARAM)&lvi);
					DOWNLOAD_PARAM * dp=(DOWNLOAD_PARAM*)lvi.lParam;
					dp->Stop=true;
				}
			}
		}
		break;
	case WM_DOWNLOADSIZE:
		{
			HWND h=GetDlgItem(hWnd,IDC_LIST1);
			LVITEM it;
			it.mask=LVIF_TEXT;
			char s[16];
			sprintf(s, "%u", (unsigned)lParam);
			it.pszText=s;
			it.iSubItem=2;
			int n = SendMessage(h, LVM_GETITEMCOUNT, 0, 0);			
			int i;
			for (i = 0; i < n; ++i) {
				LVITEM lvi;
				memset(&lvi, 0, sizeof(LVITEM));
				lvi.mask=LVIF_PARAM;
				lvi.iItem=i;
				SendMessage(h, LVM_GETITEM, i, (LPARAM)&lvi);				
				if ((DOWNLOAD_PARAM *)wParam == (DOWNLOAD_PARAM *)lvi.lParam) {
					SendMessage(h,LVM_SETITEMTEXT,i,(LPARAM)&it);
				}
			}
		}
		break;
	case WM_DOWNLOADFAIL: // fall through
	case WM_DOWNLOADFINISH:
		{

			HWND h1=GetDlgItem(hWnd,IDC_LIST1);
			char url[300];
			char local[300];
			int n=SendMessage(h1,LVM_GETITEMCOUNT,0,0);
			int i;
			for(i=0;i<n;i++)
			{
				LVITEM lvi;
				memset(&lvi, 0, sizeof(LVITEM));
				lvi.mask=LVIF_PARAM;
				lvi.iItem=i;
				SendMessage(h1, LVM_GETITEM, i, (LPARAM)&lvi);
				if ((DOWNLOAD_PARAM *)wParam == (DOWNLOAD_PARAM *)lvi.lParam) {
					lvi.iSubItem = 0;
					lvi.pszText = url;
					lvi.cchTextMax = sizeof url;
					SendMessage(h1,LVM_GETITEMTEXT,i,(LPARAM)&lvi);
					lvi.iSubItem = 1;
					lvi.pszText = local;
					lvi.cchTextMax = sizeof local;
					SendMessage(h1,LVM_GETITEMTEXT,i,(LPARAM)&lvi);
					DOWNLOAD_PARAM * dp=new DOWNLOAD_PARAM;
					dp->hwnd=hWnd;
					dp->Stop=false;
					strcpy(dp->url,url);
					strcpy(dp->local,local);
					memset(&lvi, 0, sizeof(LVITEM));
					lvi.mask = LVIF_PARAM;
					lvi.iItem = i;
					lvi.lParam = (LPARAM)dp;
					SendMessage(h1, LVM_DELETEITEM, i, NULL);

					HWND h2 = GetDlgItem(hWnd, IDC_LIST2);
					LVITEM it;
					it.mask=LVIF_TEXT;
					it.pszText=url;
					it.iItem=0;
					it.iSubItem=0;
					SendMessage(h2,LVM_INSERTITEM,0,(LPARAM)&it);
					it.iSubItem=1;
					it.pszText=local;
					SendMessage(h2,LVM_SETITEMTEXT,0,(LPARAM)&it);
					it.iSubItem=2;
					it.pszText=(LPSTR)lParam;
					SendMessage(h2,LVM_SETITEMTEXT,0,(LPARAM)&it);
					break;
				}
			}
		}
		break;
		/*
	case WM_DOWNLOADFINISH:
		{
			HWND h=GetDlgItem(hWnd,IDC_LIST1);
			LVITEM it;
			it.mask=LVIF_TEXT;
			it.pszText="下载完毕";
			it.iSubItem=2;
			SendMessage(h,LVM_SETITEMTEXT,wParam,(LPARAM)&it);
		}
		break;
		*/
	case WM_CLOSE:
		EndDialog(hWnd,0);
		break;
	}
	return FALSE;
}

void main()
{
	InitHttpDownload();
	DialogBoxParam(GetModuleHandle(NULL),"xunlei",NULL,DialogProc,0);
	UninitHttpDownload();
}
