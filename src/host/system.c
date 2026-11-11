#include "citadel/host.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif
CitadelStatus citadel_host_system_snapshot(CitadelSystemSnapshot *o) {
 if(!o)return CITADEL_ERR_INVALID_ARGUMENT;
 o->total_memory_bytes=0;o->available_memory_bytes=0;o->logical_cpu_count=0;
#if defined(_WIN32)
 { MEMORYSTATUSEX m; SYSTEM_INFO s; m.dwLength=sizeof(m);
   if(!GlobalMemoryStatusEx(&m))return CITADEL_ERR_UNAVAILABLE; GetSystemInfo(&s);
   o->total_memory_bytes=m.ullTotalPhys;o->available_memory_bytes=m.ullAvailPhys;
   o->logical_cpu_count=s.dwNumberOfProcessors; }
#elif defined(__linux__)
 { struct sysinfo s; long c=sysconf(_SC_NPROCESSORS_ONLN); if(sysinfo(&s))return CITADEL_ERR_UNAVAILABLE;
   o->total_memory_bytes=(uint64_t)s.totalram*s.mem_unit;o->available_memory_bytes=(uint64_t)s.freeram*s.mem_unit;
   o->logical_cpu_count=c>0?(uint32_t)c:1u; }
#elif defined(__APPLE__)
 { int mib[2]={CTL_HW,HW_MEMSIZE}; uint64_t total=0;size_t len=sizeof(total);
   mach_port_t h=mach_host_self();vm_size_t page=0;vm_statistics64_data_t vm;mach_msg_type_number_t count=HOST_VM_INFO64_COUNT;
   if(sysctl(mib,2,&total,&len,NULL,0)||host_page_size(h,&page)!=KERN_SUCCESS||
      host_statistics64(h,HOST_VM_INFO64,(host_info64_t)&vm,&count)!=KERN_SUCCESS)return CITADEL_ERR_UNAVAILABLE;
   o->total_memory_bytes=total;o->available_memory_bytes=(uint64_t)(vm.free_count+vm.inactive_count)*page;
   {long c=sysconf(_SC_NPROCESSORS_ONLN);o->logical_cpu_count=c>0?(uint32_t)c:1u;} }
#else
 {long c=sysconf(_SC_NPROCESSORS_ONLN);o->logical_cpu_count=c>0?(uint32_t)c:1u;}
#endif
 return CITADEL_OK;
}
