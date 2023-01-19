#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

namespace common
{
    //简单的本地日志系统
    class logrecord
    {
    private:
        FILE *fd;
        char *filename;

        char buffer[64];

        pthread_mutex_t mutex;

    private:
        char *get_localtime(time_t now)
        {
            struct tm time;
            localtime_r(&now, &time);

            bzero(this->buffer, 64 /*buf_size*/);
            sprintf(this->buffer, "%4.4d/%2.2d/%2.2d/%2.2d:%2.2d:%2.2d", time.tm_year + 1900, time.tm_mon + 1,
                    time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

            return (char *)this->buffer;
        }

    public:
        explicit logrecord(char *filename)
        {
            this->fd = fopen(filename, "a");
            this->filename = filename;
            if (pthread_mutex_init(&this->mutex, NULL))
            {
                fclose(this->fd);
            }
        }

        

        ~logrecord()
        {
            fclose(this->fd);
            this->filename = nullptr;
            pthread_mutex_destroy(&this->mutex);
        }

    public:
        // log record function
        int log_msg_record(const char *format, ...)
        {
            va_list ap;
            va_list apt;
            va_start(ap, format);
            va_copy(apt, ap);

            // print out
            vfprintf(stdout, format, ap);

            // write to rtmsg.
            if (access(this->filename, F_OK) < 0)
            {
                this->fd = fopen(this->filename, "w");
            }

            if (this->fd != NULL)
            {
                // lock
                pthread_mutex_lock(&this->mutex);

                //
                fprintf(this->fd, "%s:", get_localtime(time(NULL)));
                vfprintf(this->fd, format, apt);

                // do fflush, if for more efficient, do not fflush.
                fflush(this->fd);

                // unlock
                pthread_mutex_unlock(&this->mutex);
            }

            va_end(ap);
            va_end(apt);
        }
    };
}