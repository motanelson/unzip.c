
/* unzip_console.c — Aplicação de consola em C para descompactar um ZIP no diretório atual.
 * Requer: libzip
 * Compilar com: gcc -o unzip_console unzip_console.c -lzip
 */
//sudo apt-get install libzip-dev
//gcc -o unzip_console unzip_console.c -lzip
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_PATH 1024
#define ZIP_ERROR_BUFFER_SIZE 1024
int main(void) {
    char zipfile[MAX_PATH];
      
    printf("\033c\033[43;30m\nIntroduza o nome do ficheiro ZIP a descompactar: ");
    if (!fgets(zipfile, sizeof(zipfile), stdin)) {
        fprintf(stderr, "Erro ao ler entrada.\n");
        return 1;
    }

    // Remover newline '\n'
    zipfile[strcspn(zipfile, "\r\n")] = 0;

    int err = 0;
    zip_t *za = zip_open(zipfile, 0, &err);
    if (!za) {
        char errbuf[ZIP_ERROR_BUFFER_SIZE];
        zip_error_to_str(errbuf, sizeof(errbuf), err, errno);
        fprintf(stderr, "Erro ao abrir ZIP '%s': %s\n", zipfile, errbuf);
        return 1;
    }

    zip_int64_t num_entries = zip_get_num_entries(za, 0);
    printf("A descompactar %lld ficheiros:\n", (long long)num_entries);

    for (zip_uint64_t i = 0; i < num_entries; ++i) {
        const char *name = zip_get_name(za, i, 0);
        if (!name) {
            fprintf(stderr, "Erro ao obter nome de ficheiro no índice %llu\n",(long long) i);
            continue;
        }

        zip_file_t *zf = zip_fopen_index(za, i, 0);
        if (!zf) {
            fprintf(stderr, "Erro ao abrir ficheiro '%s' dentro do ZIP.\n", name);
            continue;
        }

        FILE *out = fopen(name, "wb");
        if (!out) {
            fprintf(stderr, "Erro ao criar ficheiro de saída '%s'.\n", name);
            zip_fclose(zf);
            continue;
        }

        char buffer[4096];
        zip_int64_t n;
        while ((n = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, n, out);
        }

        fclose(out);
        zip_fclose(zf);
        printf(" - %s\n", name);
    }

    zip_close(za);
    printf("Descompactação concluída.\n");
    return 0;
}
