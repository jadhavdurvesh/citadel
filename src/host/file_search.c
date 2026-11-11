#define _POSIX_C_SOURCE 200809L
#include "citadel/host.h"
#include <string.h>
#include <stdio.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static int has(const char*s,const char*q){size_t i,n=strlen(q);if(!n)return 0;for(;*s;s++){for(i=0;i<n;i++){char a=s[i],b=q[i];if(!a)break;if(a>='A'&&a<='Z')a+=32;if(b>='A'&&b<='Z')b+=32;if(a!=b)break;}if(i==n)return 1;}return 0;}
static void walk(const char*d,const char*q,unsigned depth,CitadelFileSearchResult*r){
 WIN32_FIND_DATAA f;char pat[520],p[520];HANDLE h;
 if(depth>CITADEL_FILE_SEARCH_MAX_DEPTH||r->count>=CITADEL_FILE_SEARCH_MAX_RESULTS){r->truncated=1;return;}
 if(snprintf(pat,sizeof(pat),"%s\\*",d)>=(int)sizeof(pat))return;
 h=FindFirstFileA(pat,&f);if(h==INVALID_HANDLE_VALUE)return;
 do{if(!strcmp(f.cFileName,".")||!strcmp(f.cFileName,".."))continue;
 if(snprintf(p,sizeof(p),"%s\\%s",d,f.cFileName)>=(int)sizeof(p))continue;
 if(f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)walk(p,q,depth+1,r);
 else if(has(f.cFileName,q)){if(strlen(p)<sizeof(r->paths[0]))strcpy(r->paths[r->count++],p);else r->truncated=1;}
 }while(FindNextFileA(h,&f)&&r->count<CITADEL_FILE_SEARCH_MAX_RESULTS);
 if(r->count>=CITADEL_FILE_SEARCH_MAX_RESULTS)r->truncated=1;FindClose(h);
}
#else
#include <dirent.h>
#include <sys/stat.h>
static int has(const char*s,const char*q){size_t i,n=strlen(q);if(!n)return 0;for(;*s;s++){for(i=0;i<n;i++){unsigned char a=s[i],b=q[i];if(!a)break;if(a>='A'&&a<='Z')a+=32;if(b>='A'&&b<='Z')b+=32;if(a!=b)break;}if(i==n)return 1;}return 0;}
static void walk(const char*d,const char*q,unsigned depth,CitadelFileSearchResult*r){
 DIR*dir;struct dirent*e;if(depth>CITADEL_FILE_SEARCH_MAX_DEPTH||r->count>=CITADEL_FILE_SEARCH_MAX_RESULTS){r->truncated=1;return;}
 dir=opendir(d);if(!dir)return;
 while((e=readdir(dir))&&r->count<CITADEL_FILE_SEARCH_MAX_RESULTS){char p[520];struct stat st;
 if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
 if(snprintf(p,sizeof(p),"%s/%s",d,e->d_name)>=(int)sizeof(p)||lstat(p,&st))continue;
 if(S_ISDIR(st.st_mode))walk(p,q,depth+1,r);
 else if(S_ISREG(st.st_mode)&&has(e->d_name,q)){if(strlen(p)<sizeof(r->paths[0]))strcpy(r->paths[r->count++],p);else r->truncated=1;}
 }
 if(e)r->truncated=1;closedir(dir);
}
#endif
CitadelStatus citadel_host_search_files(const char*root,const char*fragment,CitadelFileSearchResult*out){
 if(!root||!fragment||!out||!root[0]||!fragment[0])return CITADEL_ERR_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));walk(root,fragment,0,out);return CITADEL_OK;
}
