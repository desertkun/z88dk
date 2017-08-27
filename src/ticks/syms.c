#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ticks.h"

static symbol  *symbols = NULL;
static int      symbols_num = 0;




static int symbol_compare(const void *p1, const void *p2)
{
    const symbol *s1 = p1, *s2 = p2;

    return s2->address - s1->address;
}

void read_symbol_file(char *filename)
{
    char  buf[256];
    FILE *fp = fopen(filename,"r");

    if ( fp != NULL ) {
        while ( fgets(buf, sizeof(buf), fp) != NULL ) {
            int argc;
            char **argv = parse_words(buf,&argc);

            // Ignore
            if ( argc < 6 ) {
                free(argv);
                continue;
            }
            symbols = realloc(symbols, (symbols_num + 1) * sizeof(symbols[0]));
            symbols[symbols_num].name = strdup(argv[0]);
            symbols[symbols_num].file = strdup(argv[5]);
            symbols[symbols_num].address = strtol(argv[2] + 1, NULL, 16);
            symbols_num++;
            free(argv);
        }
    }
    qsort(symbols, symbols_num, sizeof(symbols[0]),symbol_compare);
}

static int bsearch_find(const void *key, const void *elem)
{
     int val = *(int *)key;
     const symbol *sym = elem;

     return sym->address - val;
}

symbol *find_symbol_byname(const char *name)
{
    int i;

    for ( i = 0; i < symbols_num; i++ ) {
        if ( strcmp(symbols[i].name,name) == 0 ) {
            return &symbols[i];
        }
    }
    return NULL;
}

const char *find_symbol(int addr)
{
    symbol *sym = bsearch(&addr, symbols, symbols_num, sizeof(symbols[0]), bsearch_find);
    return sym ? sym->name : NULL;
}

char **parse_words(char *line, int *argc)
{
    int                 i = 0, j = 0 , n = 0;
    int                 len = strlen(line);
    int                 in_single_quotes = 0, in_double_quotes = 0;
    char              **args;

    while ( isspace(line[i] ) ) {
        i++;
    }

    for ( ; i <= len; i++) {
        switch (line[i]) {
        case '"':
            if (in_single_quotes) {
                line[j++] = line[i];
                break;
            }
            in_double_quotes = !in_double_quotes;
            break;
        case '\'':
            if (in_double_quotes) {
                line[j++] = line[i];
                break;
            }
            in_single_quotes = !in_single_quotes;
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case 0:
            if (!in_single_quotes && !in_double_quotes) {
                line[j++] = 0;
                n++;
                i++;
                /* Try to find the start of the next word */
                while (i <= len && (line[i] == 0 || isspace(line[i])) ) {
                    i++;
                }
                i--;
                break;
            }
            line[j++] = line[i];
            break;
        case '\\':
            switch (line[i + 1]) {
            case '"':
            case '\'':
            case '\\':
                line[j++] = line[i + 1];
                break;
            case ' ':
                if (in_single_quotes || in_double_quotes) {
                    line[j++] = line[i];
                }
                line[j++] = line[i + 1];
                break;
            default:
                line[j++] = line[i];
                line[j++] = line[i + 1];
                break;
            }
            i++;
            break;
        default:
            line[j++] = line[i];
            break;
        }
    }

    args = (char **)malloc(sizeof(char *) * (n + 1) );
    n = 0;
    args[n++] = line;
    j--;

    for (i = 0; i < j; i++) {
        if (line[i] == 0) {
            args[n++] = line + i + 1;
        }
    }

    *argc = n;

    return args;
}
