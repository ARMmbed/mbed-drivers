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
#include "test_env.h"
#include "semihost_api.h"

Serial pc(USBTX, USBRX);

#define FILENAME      "/local/out.txt"
#define TEST_STRING   "Hello World!"

FILE *test_open(const char *mode)
{
    FILE *f = fopen(FILENAME, mode);
    if (f == NULL) {
        printf("Error opening file"NL);
        notify_completion(false);
    }
    return f;
}

void test_write(FILE *f, char *str, int str_len)
{
    int n = fprintf(f, str);

    if (n != str_len) {
        printf("Error writing file"NL);
        notify_completion(false);
    }
}

void test_read(FILE *f, char *str, int str_len)
{
    int n = fread(str, sizeof(unsigned char), str_len, f);

    if (n != str_len) {
        printf("Error reading file"NL);
        notify_completion(false);
    }
}

void test_close(FILE *f)
{
    int rc = fclose(f);

    if (rc != 0) {
        printf("Error closing file"NL);
        notify_completion(false);
    }
}

int main()
{
    pc.printf("Test the Stream class\n");

    printf("connected: %s\n", (semihost_connected()) ? ("Yes") : ("No"));

    char mac[16];
    mbed_mac_address(mac);
    printf("mac address: %02x,%02x,%02x,%02x,%02x,%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    LocalFileSystem local("local");

    FILE *f;
    char *str = TEST_STRING;
    char *buffer = (char *)malloc(sizeof(unsigned char) * strlen(TEST_STRING));
    int str_len = strlen(TEST_STRING);

    // Write
    f = test_open("w");
    test_write(f, str, str_len);
    test_close(f);

    // Read
    f = test_open("r");
    test_read(f, buffer, str_len);
    test_close(f);

    // Check the two strings are equal
    notify_completion((strncmp(buffer, str, str_len) == 0));
}
