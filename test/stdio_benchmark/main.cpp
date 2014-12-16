/* mbed Microcontroller Library
 * Copyright (c) 2013-2014 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"

#define TEST_STDIO 0

int main() {
    printf("\r\n\r\n*** Start of memory write test (2K bytes) ***\r\n");

    // dummy data
    char buf[2048];
    int index = 0;
    for (index = 0; index < 2048; index++) {
        buf[index] = ~index & 0xFF;
    }

    // Run the timed write test
    float starttime, duration;
    Timer t;
    t.start();
    starttime = t.read();

#if TEST_STDIO
    LocalFileSystem local("local");
    FILE *fp = fopen("/local/test.dat", "w");
    fwrite(buf, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0]), fp);
    fclose(fp);
#else
    FILEHANDLE fh = local_file_open("test.dat", O_WRONLY);
    LocalFileHandle lfh(fh);
    lfh.write(buf, sizeof(buf));
    lfh.close();
#endif

    duration = t.read() - starttime;
    printf("Write completed in %.2f seconds. Average throughput = %.0f bytes/second.\r\n", duration, 2048/duration);
}
