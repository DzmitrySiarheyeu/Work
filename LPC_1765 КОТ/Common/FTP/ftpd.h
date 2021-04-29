/*
 *  Copyright (c) 2004, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @file    ftpd.h
 * @author  Dennis Kuschel
 * @brief   FTP demon for embedded devices
 *
 * This software is from http://mycpu.mikrocontroller.net.
 * Please send questions and bug reports to dennis_k@freenet.de.
 */

#ifndef _FTPD_H_
#define _FTPD_H_

#include "lwip/sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define FS_SUBDIRECTORIES 1
/* note: already defined in cc.h from lwip
typedef unsigned char   u8_t;
typedef signed   char   s8_t;
typedef unsigned short  u16_t;
typedef signed   short  s16_t;
typedef unsigned int    u32_t;
typedef signed   int    s32_t;
*/
typedef unsigned int    uint_t;
typedef signed   int    sint_t;

//#define FS_SUBDIRECTORIES 1

/* macros */
#if POSCFG_ENABLE_NANO != 0

#define sysPrintErr(x)          nosPrint(x)
#define sysSleep(ms)            nosTaskSleep(ms)
#define sysMemAlloc(s)          nosMemAlloc(s)
#define sysMemFree(x)           nosMemFree(x)
#define sysMemCopy(d,s,l)       nosMemCopy(d,s,l)
#define sysMemSet(d,v,s)        nosMemSet(d,v,s)

#else

//#define sysPrintErr(x)          printf("%s",x)
#define sysSleep(ms)            posTaskSleep(ms)
#define sysMemAlloc(s)          mem_malloc(s)
#define sysMemFree(x)           mem_free(x)
#define sysMemCopy(d,s,l)       memcpy(d,s,l)
#define sysMemSet(d,v,s)        memset(d,v,s)

#endif

#define sysStrcpy(d,s)          strcpy(d,s)
#define sysStrncpy(d,s,n)       strncpy(d,s,n)
#define sysStrcat(d,s)          strcat(d,s)
#define sysStrlen(s)            strlen(s)
#define sysStrcmp(s1,s2)        strcmp(s1,s2)
#define sysStricmp(s1,s2)       stricmp(s1,s2)
#define sysStrncmp(s1,s2,n)     strncmp(s1,s2,n)
#define sysStrnicmp(s1,s2,n)    strncmp(s1,s2,n)

//#define closesocket(s)          lwip_close(s)



/* socket error numbers */
#define sysGetErrno()       errno




void vFTPServer(void *Param);



/* Declaration of a counter variable.
   The variable is provided and exported by the operating system.
   It is incremented by the system timer. */
//#define COUNTERVAR      xTaskGetTickCount()  /* external variable that is incremented n times per second */
#define COUNTERTYPE     u32_t   /* type of external counter variable */
#define COUNTERSTYPE    s32_t   /* signed type of external counter variable */
//#define COUNTERTPS      1000       /* number of ticks the counter is incremented per second */



/** @defgroup ftpd Embedded FTP Demon *//** @{ */


/**
 * This function starts the FTP demon. The function
 * call does not return; to terminate the demon again
 * you can do a call to ::ftpd_stop.
 * @param username  Username. The FTPD supports only one
 *                  user name and has no rights management.
 *                  Anonymous login is not supported.
 * @param password  Password for the user with username.
 * @return zero on success.
 */
sint_t ftpd_start(char *username, char *password);


/**
 * This function stoppes the FTP demon.
 * Note that it takes a while (up to 1 second) until
 * this function returns.
 * @return zero on success.
 */
sint_t ftpd_stop(void);


/**
 * This function can be used to test if the
 * FTP demon task is still running.
 * @return  nonzero when the ftpd is running.
 */
sint_t ftpd_running(void);


/*@}*/


sint_t ft_buildPathName(char *outbuf, char *outmybuf, const char *pathname, const char *filename);



xTaskHandle CreateMyFTP( pdTASK_CODE pvTaskCode, char *name, unsigned portSHORT usStackDepth, void *pvParameters, unsigned portBASE_TYPE uxPriority );
#endif /* _FTPD_H_ */
