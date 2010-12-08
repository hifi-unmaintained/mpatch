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
#include <string.h>
#include "pdata.h"

#define LINE "Press ENTER to continue or Ctrl-C to cancel."

void chr_repeat(char c, int cnt)
{
    int i = 0;
    for (; i < cnt; i++)
    {
        putc(c, stdout);
    }
}

int main(int argc, char **argv)
{
    FILE *fh;
    unsigned char byte;
    patch *p;
    enum { UNK, VAL, REV } status = UNK;

    printf("%s\n", PATCH_TITLE);
    chr_repeat('-', strlen(PATCH_TITLE));
    printf("\n");

    printf(PATCH_DESC);
    printf("\n\n");

    printf("%s\n", LINE);
    chr_repeat('-', strlen(LINE));
    printf("\n");
    getchar();

    printf("Opening file...\n");
    if ( !(fh = fopen(PATCH_EXE, "r+")) )
    {
        perror(PATCH_EXE);
        getchar();
        return 1;
    }

    printf("Validating...\n");

    p = pdata;
    while (p->off)
    {
        if (fseek(fh, p->off, SEEK_SET) != 0)
        {
            perror("seek");
            fclose(fh);
            getchar();
            return 1;
        }

        if (fread(&byte, 1, 1, fh) != 1)
        {
            perror("read");
            fclose(fh);
            getchar();
            return 1;
        }

        if (byte == p->a)
        {
            if (status == UNK)
            {
                status = VAL;
            }
            else if (status == REV)
            {
                printf("Error: Invalid file\n");
                fclose(fh);
                getchar();
                return 1;
            }
        }
        else if (byte == p->b)
        {
            if (status == UNK)
            {
                status = REV;
            }
            else if (status == VAL)
            {
                printf("Error: Invalid file\n");
                fclose(fh);
                getchar();
                return 1;
            }
        }
        else
        {
            printf("Error: Invalid file\n");
            fclose(fh);
            getchar();
            return 1;
        }

        p++;
    }

    if (status == UNK)
    {
        printf("Empty patch? Nothing to do.\n");
        fclose(fh);
        getchar();
        return 1;
    }

    if (status == REV)
    {
        printf("File already patched. Press ENTER to revert the patch or Ctrl-C to cancel.\n");
        getchar();

        p = pdata;
        while (p->off)
        {
            if (fseek(fh, p->off, SEEK_SET) != 0)
            {
                perror("seek");
                fclose(fh);
                getchar();
                return 1;
            }

            if (fwrite(&p->a, 1, 1, fh) != 1)
            {
                perror("write");
                fclose(fh);
                getchar();
                return 1;
            }

            p++;
        }

        fclose(fh);

        printf("Patch reverted.\n");
        printf("Press ENTER to exit.\n");
        getchar();

        return 0;
    }

    printf("Patching...\n");

    p = pdata;
    while (p->off)
    {
        if (fseek(fh, p->off, SEEK_SET) != 0)
        {
            perror("seek");
            fclose(fh);
            getchar();
            return 1;
        }

        if (fwrite(&p->b, 1, 1, fh) != 1)
        {
            perror("write");
            fclose(fh);
            getchar();
            return 1;
        }

        p++;
    }

    fclose(fh);

    printf("All done. Enjoy!\n");
    printf("Press ENTER to exit.\n");
    getchar();

    return 0;
}
