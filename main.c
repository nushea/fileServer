#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int openFile(char * filepath){
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        puts("HTTP/1.1 404 Not Found");
        puts("Content-Type: text/plain\n");
        puts("file not found");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: application/octet-stream\r\n");
    printf("Content-Disposition: attachment; filename=\"test.png\"\r\n");
    printf("Content-Length: %ld\r\n", size);
    printf("\r\n");

    // content
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        fwrite(buf, 1, n, stdout);
    }

    fclose(f);
    return 0;
}

int main(){
    char *filepath = "./files/uwu.png";

    if(openFile(filepath) != 0)
        return -1;

//    puts("Content-Type: text/plain\n\n");
//    puts("test\n");
    return 0;
}
