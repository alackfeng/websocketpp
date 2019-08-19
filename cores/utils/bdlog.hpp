//
//  bdlog.h
//  Jerasure
//
//  Created by 王俊 on 2019/3/6.
//  Copyright © 2019年 王俊. All rights reserved.
//
#ifndef bdlog_h
#define bdlog_h

#include <stdio.h>
#include <stdlib.h>
#ifdef ANDROID_JNI_LOG
#include <jni.h>
#include <android/log.h>
#define TAG "android-jni-chunkcc"
#endif

// #define DEBUG_BD
extern FILE * fpLog;
extern void log_init(const char* filename, int level);
extern void log_close();
extern void log_fprintf(int level, const char* fmt, ...);
enum LOG_LEVEL {
  LOG_LEVEL_0,
  LOG_LEVEL_FATAL,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WRAN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_TRACE,
  LOG_LEVEL_ALL
};

#ifndef bdlog
# ifdef DEBUG_BD
# ifdef ANDROID_JNI_LOG
#  define bdlog_fatal(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#  define bdlog_error(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#  define bdlog_warn(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#  define bdlog_info(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#  define bdlog_debug(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#  define bdlog(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
# else
#  define bdlog_fatal(...) log_fprintf(LOG_LEVEL_FATAL, __VA_ARGS__)
#  define bdlog_error(...) log_fprintf(LOG_LEVEL_ERROR, __VA_ARGS__)
#  define bdlog_warn(...) log_fprintf(LOG_LEVEL_WRAN, __VA_ARGS__)
#  define bdlog_info(...) log_fprintf(LOG_LEVEL_INFO, __VA_ARGS__)
#  define bdlog_debug(...) log_fprintf(LOG_LEVEL_DEBUG, __VA_ARGS__)
#  define bdlog(...) log_fprintf(LOG_LEVEL_DEBUG, __VA_ARGS__)
# endif
# else
#  define bdlog_fatal(...)
#  define bdlog_error(...)
#  define bdlog_warn(...)
#  define bdlog_info(...)
#  define bdlog_debug(...)
#  define bdlog(...)
# endif
#endif


#endif /* bdlog_h */
