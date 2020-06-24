#ifndef __STDARG_H__
#define __STDARG_H__

#define va_start(list,param1)   (list = (va_list)&param1+ sizeof(param1) )
#define va_arg(list,mode)   ((mode *) ( list += sizeof(mode) ) )[-1]
#define va_end(list) ( list = (va_list)0 )
typedef char *va_list;


#endif 