/*
 * Copyright (c) 2010 Toni Spets <toni.spets@iki.fi>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fh_old;
    FILE *fh_new;
    unsigned char b_old;
    unsigned char b_new;
    long offset = 0L;
    long size;

    if (argc < 3)
    {
        fprintf(stderr, "usage: %s <old> <new>\n", argv[0]);
        return 1;
    }

    fh_old = fopen(argv[1], "r");
    if (!fh_old)
    {
        perror(argv[1]);
        return 1;
    }

    fh_new = fopen(argv[2], "r");
    if (!fh_new)
    {
        perror(argv[2]);
        return 1;
    }

    fseek(fh_old, 0L, SEEK_END);
    fseek(fh_new, 0L, SEEK_END);

    if (ftell(fh_old) != ftell(fh_new))
    {
        fprintf(stderr, "error: old and new file differ in size\n");
        fclose(fh_old);
        fclose(fh_new);
        return 1;
    }

    size = ftell(fh_old);

    fseek(fh_old, 0L, SEEK_SET);
    fseek(fh_new, 0L, SEEK_SET);

    printf("#define PATCH_TITLE \"A patch for <program> by <name>\"\n");
    printf("#define PATCH_DESC  \"\"\n");
    printf("#define PATCH_EXE   \"%s\"\n\n", argv[1]);

    printf("typedef struct { unsigned int off; unsigned char a; unsigned char b; } patch;\n");
    printf("patch pdata[] =\n{\n");
    
    while(offset < size)
    {
        if (fread(&b_old, 1, 1, fh_old) != 1 || fread(&b_new, 1, 1, fh_new) != 1)
        {
            perror("read error");
            fclose(fh_old);
            fclose(fh_new);
            return 1;
        }

        if (b_old != b_new)
        {
            printf("    { 0x%08X, 0x%02X, 0x%02X },\n", (unsigned int)offset, b_old, b_new);
        }

        offset++;
    }

    printf("    { 0x00000000, 0x00, 0x00 }\n");
    printf("};\n");

    fclose(fh_old);
    fclose(fh_new);

    return 0;
}
