#include <stddef.h>
#include <stdarg.h>
#include <i386/config.h> 
#include <i386/linkage.h>
#include <kernel/vsnprintf.h> 
#include <kernel/printk.h>

#define LOG_BUF_LEN (1 << CONFIG_LOG_BUG_SHIFT )
#define LOG_BUF_MASK (LOG_BUF_LEN - 1 )

static char __attribute__((used)) __log_buf_ [LOG_BUF_LEN];

#define LOG_BUF(idx) (__log_buf_[(idx)&LOG_BUF_MASK])
static unsigned long log_start = 0;
static unsigned long log_end = 0;


#define DEAULT_MESSAGE_LEVEL (4)

#define MIN_MESSAGE_LEVEL (1)
#define MAX_MESSAGE_LEVEL (7)

void emit_log_char(char c){
    LOG_BUF(log_end) = c ;
    log_end ++ ;
    if(log_end - log_start > LOG_BUF_LEN){
        log_start = log_end - LOG_BUF_LEN;
    }
}

int asmlinkage printk(const char* restrict fmt, ...){
    va_list args;
    va_start(args, fmt);

    static char log_buf[1024];
    static char *p;
    static int unknown_log_level = 1 ; 
    size_t printed_len;


    printed_len = vsnprintf(log_buf,1024,fmt,args);
    for (p = log_buf; *p; p++) {
    if (unknown_log_level) {
        if (p[0] != '<' || p[1] < '0' || p[1] > '7' || p[2] != '>') {
            emit_log_char('<');
            emit_log_char(DEAULT_MESSAGE_LEVEL + '0');
            emit_log_char('>');
        }
        unknown_log_level = 0;
    }
    emit_log_char(*p);
    if (*p == '\n')
        unknown_log_level = 1;
    }
    /*
        todo :
        call console to out put the log message 
    */
    va_end(args);
    return 1;
}



