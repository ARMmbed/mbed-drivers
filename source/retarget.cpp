/*
 * Copyright (c) 2006-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/platform.h"
#include "mbed-drivers/FileHandle.h"
#include "mbed-drivers/FileSystemLike.h"
#include "mbed-drivers/FilePath.h"
#include "serial_api.h"
#include "compiler-polyfill/attributes.h"
#include "cmsis.h"
#include <errno.h>
#include "minar/minar.h"
#include "mbed-hal/init_api.h"
#include "mbed-hal/serial_api.h"
#include "core_generic.h"

#if defined(__ARMCC_VERSION)
#   include <rt_sys.h>
#   define PREFIX(x)    _sys##x
#   ifdef __MICROLIB
#       pragma import(__use_full_stdio)
#   endif

#elif defined(__ICCARM__)
#   include <yfuns.h>
#   define PREFIX(x)        _##x

#   define STDIN_FILENO     0
#   define STDOUT_FILENO    1
#   define STDERR_FILENO    2

#else
#   include <sys/stat.h>
#   include <sys/unistd.h>
#   include <sys/syslimits.h>
#   define PREFIX(x)    x
#endif

#ifndef YOTTA_CFG_MBED_MAX_FILEHANDLES
#   define YOTTA_CFG_MBED_MAX_FILEHANDLES 16
#endif

#ifndef pid_t
 typedef int pid_t;
#endif
#ifndef caddr_t
 typedef char * caddr_t;
#endif

#ifndef YOTTA_CFG_MBED_OS_STDIO_DEFAULT_BAUD
#define YOTTA_CFG_MBED_OS_STDIO_DEFAULT_BAUD 115200
#endif

#define STDIO_DEFAULT_BAUD YOTTA_CFG_MBED_OS_STDIO_DEFAULT_BAUD


using namespace mbed;
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE

#ifdef YOTTA_GREENTEA_CLIENT_VERSION_STRING
#include "greentea-client/test_env.h"
#else
#include "mbed-drivers/test_env.h"
#endif

bool coverage_report = false;
const int gcov_fd = 'g' + ((int)'c' << 8);

// Retarget specific code coverage report start notification
static void retarget_notify_coverage_start(const char *path) {
#ifdef YOTTA_GREENTEA_CLIENT_VERSION_STRING
    greentea_notify_coverage_start(path);
#else
    notify_coverage_start(path);
#endif  // YOTTA_GREENTEA_CLIENT_VERSION_STRING
}

// Retarget specific code coverage report end notification
static void retarget_notify_coverage_end() {
#ifdef YOTTA_GREENTEA_CLIENT_VERSION_STRING
    greentea_notify_coverage_end();
#else
    notify_coverage_end();
#endif  // YOTTA_GREENTEA_CLIENT_VERSION_STRING
}

#endif  // YOTTA_CFG_DEBUG_OPTIONS_COVERAGE


#if defined(__MICROLIB) && (__ARMCC_VERSION>5030000)
// Before version 5.03, we were using a patched version of microlib with proper names
extern const char __stdin_name[]  = ":tt";
extern const char __stdout_name[] = ":tt";
extern const char __stderr_name[] = ":tt";

#else
extern const char __stdin_name[]  = "/stdin";
extern const char __stdout_name[] = "/stdout";
extern const char __stderr_name[] = "/stderr";
#endif

/* newlib has the filehandle field in the FILE struct as a short, so
 * we can't just return a Filehandle* from _open and instead have to
 * put it in a filehandles array and return the index into that array
 * (or rather index+3, as filehandles 0-2 are stdin/out/err).
 */
static FileHandle *filehandles[YOTTA_CFG_MBED_MAX_FILEHANDLES];

FileHandle::~FileHandle() {
    /* Remove all open filehandles for this */
    for (unsigned int fh_i = 0; fh_i < sizeof(filehandles)/sizeof(*filehandles); fh_i++) {
        if (filehandles[fh_i] == this) {
            filehandles[fh_i] = NULL;
        }
    }
}

#if DEVICE_SERIAL

#include "mbed-drivers/Serial.h"


namespace mbed {

Serial& get_stdio_serial()
{
    static bool stdio_uart_inited = false;
    static Serial stdio_serial(STDIO_UART_TX, STDIO_UART_RX);
    if (!stdio_uart_inited) {
        stdio_serial.baud(STDIO_DEFAULT_BAUD);
        stdio_uart_inited = true;
    }
    return stdio_serial;
}

} // namespace mbed
#endif

static void init_serial() {
#if DEVICE_SERIAL
    get_stdio_serial();
#endif
}

static inline int openmode_to_posix(int openmode) {
    int posix = openmode;
#ifdef __ARMCC_VERSION
    if (openmode & OPEN_PLUS) {
        posix = O_RDWR;
    } else if(openmode & OPEN_W) {
        posix = O_WRONLY;
    } else if(openmode & OPEN_A) {
        posix = O_WRONLY|O_APPEND;
    } else {
        posix = O_RDONLY;
    }
    /* a, w, a+, w+ all create if file does not already exist */
    if (openmode & (OPEN_A|OPEN_W)) {
        posix |= O_CREAT;
    }
    /* w and w+ truncate */
    if (openmode & OPEN_W) {
        posix |= O_TRUNC;
    }
#elif defined(__ICCARM__)
    switch (openmode & _LLIO_RDWRMASK) {
        case _LLIO_RDONLY: posix = O_RDONLY; break;
        case _LLIO_WRONLY: posix = O_WRONLY; break;
        case _LLIO_RDWR  : posix = O_RDWR  ; break;
    }
    if (openmode & _LLIO_CREAT ) posix |= O_CREAT;
    if (openmode & _LLIO_APPEND) posix |= O_APPEND;
    if (openmode & _LLIO_TRUNC ) posix |= O_TRUNC;
#endif
    return posix;
}

extern "C" FILEHANDLE PREFIX(_open)(const char* name, int openmode) {
#if defined(__MICROLIB) && (__ARMCC_VERSION>5030000)
    // Before version 5.03, we were using a patched version of microlib with proper names
    // This is the workaround that the microlib author suggested us
    static int n = 0;
    if (!std::strcmp(name, ":tt")) return n++;

#else
    /* Use the posix convention that stdin,out,err are filehandles 0,1,2.
     */
    if (std::strcmp(name, __stdin_name) == 0) {
        init_serial();
        return 0;
    } else if (std::strcmp(name, __stdout_name) == 0) {
        init_serial();
        return 1;
    } else if (std::strcmp(name, __stderr_name) == 0) {
        init_serial();
        return 2;
    }
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    else if(coverage_report) {
        retarget_notify_coverage_start(name);
        return gcov_fd;
    }
#endif

#endif

    // find the first empty slot in filehandles
    unsigned int fh_i;
    for (fh_i = 0; fh_i < sizeof(filehandles)/sizeof(*filehandles); fh_i++) {
        if (filehandles[fh_i] == NULL) break;
    }
    if (fh_i >= sizeof(filehandles)/sizeof(*filehandles)) {
        return -1;
    }

    FileHandle *res;

    /* FILENAME: ":0x12345678" describes a FileLike* */
    if (name[0] == ':') {
        void *p;
        sscanf(name, ":%p", &p);
        res = (FileHandle*)p;

    /* FILENAME: "/file_system/file_name" */
    } else {
        FilePath path(name);

        if (!path.exists())
            return -1;
        else if (path.isFile()) {
            res = path.file();
        } else {
            FileSystemLike *fs = path.fileSystem();
            if (fs == NULL) return -1;
            int posix_mode = openmode_to_posix(openmode);
            res = fs->open(path.fileName(), posix_mode); /* NULL if fails */
        }
    }

    if (res == NULL) return -1;
    filehandles[fh_i] = res;

    return fh_i + 3; // +3 as filehandles 0-2 are stdin/out/err
}

extern "C" int PREFIX(_close)(FILEHANDLE fh) {
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    if(coverage_report && fh == gcov_fd) {
        retarget_notify_coverage_end();
        return 0;
    }
#endif

    if (fh < 3) return 0;

    FileHandle* fhc = filehandles[fh-3];
    filehandles[fh-3] = NULL;
    if (fhc == NULL) return -1;

    return fhc->close();
}

#if defined(__ICCARM__)
extern "C" size_t    __write (int        fh, const unsigned char *buffer, size_t length) {
#else
extern "C" int PREFIX(_write)(FILEHANDLE fh, const unsigned char *buffer, unsigned int length, int mode) {
#endif
    (void) mode;
    int n; // n is the number of bytes written
    if (fh < 3) {
#if DEVICE_SERIAL
        for (unsigned int i = 0; i < length; i++) {
            get_stdio_serial().putc(buffer[i]);
        }
#endif
        n = length;
    }
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    else if(coverage_report && fh == gcov_fd) {
        for (unsigned int i = 0; i < length; i++) {
            if (0x00 == buffer[i]) {
                // Very simple coverage stream compression
                // Observation: About 30-35% of coverage stream are bytes with value 0x00
                // Greentea will pick up '.' and replace with "00"
                printf(".");
            } else {
                printf("%02x", buffer[i]);
            }
        }
        n = length;
    }
#endif
    else {
        FileHandle* fhc = filehandles[fh-3];
        if (fhc == NULL) return -1;

        n = fhc->write(buffer, length);
    }
#ifdef __ARMCC_VERSION
    return length-n;
#else
    return n;
#endif
}

#if defined(__ICCARM__)
extern "C" size_t    __read (int        fh, unsigned char *buffer, size_t       length) {
#else
extern "C" int PREFIX(_read)(FILEHANDLE fh, unsigned char *buffer, unsigned int length, int mode) {
#endif
    (void) mode;
    int n; // n is the number of bytes read
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    if(coverage_report && fh == gcov_fd) {
        return 0;
    }
#endif
    if (fh < 3) {
        // only read a character at a time from stdin
#if DEVICE_SERIAL
        *buffer = get_stdio_serial().getc();
#endif
        n = 1;
    } else {
        FileHandle* fhc = filehandles[fh-3];
        if (fhc == NULL) return -1;

        n = fhc->read(buffer, length);
    }
#ifdef __ARMCC_VERSION
    return length-n;
#else
    return n;
#endif
}

#ifdef __ARMCC_VERSION
extern "C" int PREFIX(_istty)(FILEHANDLE fh)
#else
extern "C" int _isatty(FILEHANDLE fh)
#endif
{
    /* stdin, stdout and stderr should be tty */
    if (fh < 3) return 1;

    FileHandle* fhc = filehandles[fh-3];
    if (fhc == NULL) return -1;

    return fhc->isatty();
}

extern "C"
#if defined(__ARMCC_VERSION)
int _sys_seek(FILEHANDLE fh, long position)
#elif defined(__ICCARM__)
long __lseek(int fh, long offset, int whence)
#else
int _lseek(FILEHANDLE fh, int offset, int whence)
#endif
{
    if (fh < 3) return 0;

#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    if(coverage_report && fh == gcov_fd) {
        return 0;
    }
#endif
    FileHandle* fhc = filehandles[fh-3];
    if (fhc == NULL) return -1;

#if defined(__ARMCC_VERSION)
    return fhc->lseek(position, SEEK_SET);
#else
    return fhc->lseek(offset, whence);
#endif
}

#ifdef __ARMCC_VERSION
extern "C" int PREFIX(_ensure)(FILEHANDLE fh) {
    if (fh < 3) return 0;

    FileHandle* fhc = filehandles[fh-3];
    if (fhc == NULL) return -1;

    return fhc->fsync();
}

extern "C" long PREFIX(_flen)(FILEHANDLE fh) {
    if (fh < 3) return 0;

    FileHandle* fhc = filehandles[fh-3];
    if (fhc == NULL) return -1;

    return fhc->flen();
}
#endif


#if !defined(__ARMCC_VERSION) && !defined(__ICCARM__)
extern "C" int _fstat(int fd, struct stat *st) {
    if ((STDOUT_FILENO == fd) || (STDERR_FILENO == fd) || (STDIN_FILENO == fd)) {
        st->st_mode = S_IFCHR;
        return  0;
    }
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    if (coverage_report && fd == gcov_fd) {
        st->st_size = 0;
        return 0;
    }
#endif

    errno = EBADF;
    return -1;
}
#endif

namespace std {
extern "C" int remove(const char *path) {
    FilePath fp(path);
    FileSystemLike *fs = fp.fileSystem();
    if (fs == NULL) return -1;

    return fs->remove(fp.fileName());
}

extern "C" int rename(const char *oldname, const char *newname) {
    FilePath fpOld(oldname);
    FilePath fpNew(newname);
    FileSystemLike *fsOld = fpOld.fileSystem();
    FileSystemLike *fsNew = fpNew.fileSystem();

    /* rename only if both files are on the same FS */
    if (fsOld != fsNew || fsOld == NULL) return -1;

    return fsOld->rename(fpOld.fileName(), fpNew.fileName());
}

extern "C" char *tmpnam(char *s) {
    (void) s;
    return NULL;
}

extern "C" FILE *tmpfile() {
    return NULL;
}
} // namespace std

#ifdef __ARMCC_VERSION
extern "C" char *_sys_command_string(char *cmd, int len) {
    return NULL;
}
#endif

extern "C" DIR *opendir(const char *path) {
    /* root dir is FileSystemLike */
    if (path[0] == '/' && path[1] == 0) {
        return FileSystemLike::opendir();
    }

    FilePath fp(path);
    FileSystemLike* fs = fp.fileSystem();
    if (fs == NULL) return NULL;

    return fs->opendir(fp.fileName());
}

extern "C" struct dirent *readdir(DIR *dir) {
    return dir->readdir();
}

extern "C" int closedir(DIR *dir) {
    return dir->closedir();
}

extern "C" void rewinddir(DIR *dir) {
    dir->rewinddir();
}

extern "C" off_t telldir(DIR *dir) {
    return dir->telldir();
}

extern "C" void seekdir(DIR *dir, off_t off) {
    dir->seekdir(off);
}

extern "C" int mkdir(const char *path, mode_t mode) {
    FilePath fp(path);
    FileSystemLike *fs = fp.fileSystem();
    if (fs == NULL) return -1;

    return fs->mkdir(fp.fileName(), mode);
}

#if defined(TOOLCHAIN_GCC) || defined(TARGET_LIKE_CLANG)
/* prevents the exception handling name demangling code getting pulled in */
#include "mbed-drivers/mbed_error.h"
namespace __gnu_cxx {
    void __verbose_terminate_handler() {
        error("Exception");
    }
}
extern "C" __weak void __cxa_pure_virtual(void);
extern "C" __weak void __cxa_pure_virtual(void) {
    exit(1);
}

#endif

// ****************************************************************************
// mbed_hal_init() is a function that is called before starting the scheduler,
// it is not meant for user code, but for HAL ports to perform initialization
// before the scheduler is started

#if defined(TOOLCHAIN_ARM)
extern "C" int $Super$$main(void);

extern "C" int $Sub$$main(void) {
    mbed_hal_init();
    return $Super$$main();
}
#elif defined(TOOLCHAIN_GCC)  || defined(TARGET_LIKE_CLANG)
extern "C" int __real_main(void);

extern "C" int __wrap_main(void) {
    mbed_hal_init();
    return __real_main();
}
#elif defined(TOOLCHAIN_IAR)
// IAR doesn't have the $Super/$Sub mechanism of armcc, nor something equivalent
// to ld's --wrap. It does have a --redirect, but that doesn't help, since redirecting
// 'main' to another symbol looses the original 'main' symbol. However, its startup
// code will call a function to setup argc and argv (__iar_argc_argv) if it is defined.
// Since mbed doesn't use argc/argv, we use this function to call mbed_hal_init.
extern "C" void __iar_argc_argv() {
    mbed_hal_init();
}
#endif

// the user should set up their application in app_start
extern void app_start(int, char**);
extern "C" int main(void) {
    minar::Scheduler::postCallback(
        mbed::util::FunctionPointer2<void, int, char**>(&app_start).bind(0, NULL)
    );
    return minar::Scheduler::start();
}



extern "C" void _exit(int status)
{
    (void) status;
    while(1) {
        __BKPT(0);
    }
}
extern "C" int _kill(pid_t pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}
extern "C" pid_t _getpid(void)
{
    return 0;
}
