
#include "bdlog.hpp"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "Common.h"

#define FILENAME_LEN 1024
#define MESSAGE_LEN 1024


FILE * fpLog = NULL;
static char logName[FILENAME_LEN]= {0};
static int logLevel = LOG_LEVEL_ALL;

void log_init(const char* filename, int level) {

  bdlog("log_init logname %s, level %d.\n", filename, level);
  if (!fpLog) {

    if(!filename) {
      return;
    }

    fpLog = fopen(filename, "r+");
    if(!fpLog) {
      fpLog = fopen(filename, "w+");
      if(!fpLog) {
        perror("fpLog.");
      }
    }

    memset(logName, 0, FILENAME_LEN);
    memcpy(logName, filename, FILENAME_LEN);

    logLevel = level;
  }

}

void log_close() {
  bdlog("log_close logname %s, level %d.\n", logName, logLevel);
  if (fpLog)
    fclose(fpLog);
}

void log_fprintf(int level, const char* fmt, ...) {
  do {

    if (level > logLevel) {
      return;
    }
    if (!fpLog && logName[0]) { 
      fpLog = fopen(logName, "rb+");
      if(!fpLog) {
        fpLog = fopen(logName, "wb+");
        if(!fpLog) {
          perror("fpLog. write");
        }
      }
    }

    char message[MESSAGE_LEN] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf (message, MESSAGE_LEN, fmt, args);
    va_end(args);

    // printf("%s", message);
    if (!fpLog) { // 直接输出到控制台
      fprintf(stderr, "%s", message );
    }
    else {
    setFilePos(fpLog, 0, SEEK_END);
      fwrite(message, 1, strlen(message), fpLog);
    }
  } while(0);
}