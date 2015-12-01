#ifndef _HTTPDOWNLOADER_H_FILE_INCLUDED_

#define _HTTPDOWNLOADER_H_FILE_INCLUDED_

void InitHttpDownload();
void UninitHttpDownload();
void * StartHttpDownload(const char * URL);
int ReadHttpData(void * h,void * Buf,unsigned Size,unsigned * RdSize);
void StopHttpDownload(void * h);

#endif
