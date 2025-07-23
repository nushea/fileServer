#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>



#define BUFFER 256

void sendFail(){
    puts("HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: close\r\n"
    "\r\n"
    "token or password are incorrect or sth idk");
}

int openFile(const char * filepath, const char * filename){
    FILE *f;
    if(!filepath)
        return -1;
    char fp[BUFFER+1];
    if(filepath[0] == '/'){
        strncpy(fp+1, filepath, BUFFER);
        fp[0] = '.';
        f = fopen(fp, "rb");
    }
    else
        f = fopen(filepath, "rb");
    
    if (!f) {
        puts("HTTP/1.1 404 Not Found");
        puts("Content-Type: text/plain\n");
        puts("file not found\n");
        printf("%s",filepath);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: application/octet-stream\r\n");
    printf("Content-Disposition: attachment; filename=\"%s\"\r\n", filename);
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

int query(char * key, char * pass){
    sqlite3 *db;

    int rc;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT password, filename, filepath FROM trans WHERE key = ?;";

    rc = sqlite3_open("trans.db", &db);
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -2;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "failed to prepare: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -3;
    }

    if (sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "failed to bind key\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -4;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_close(db);
        return -5;
    }
    const char *password = (const char*)sqlite3_column_text(stmt, 0);
    const char *filename = (const char*)sqlite3_column_text(stmt, 1);
    const char *filepath = (const char*)sqlite3_column_text(stmt, 2);
    sqlite3_close(db);

    //printf("password: %s\nfilename: %s\nfilepath: %s\n", password, filename, filepath);
    if(!filepath || !filename){
        return -6;
    }
    if(!password){
        openFile(filepath, filename);
        return 0;}
    if(!(*password)){
        openFile(filepath, filename);
        return 0;
    }
    if(!(*pass)){
        return -7;
    }
    int i = 0;
    while(pass[i] && password[i] && i < BUFFER){
        if(pass[i] != password[i])
            return -8;
        i+=1;
    }
    if(pass[i] || password[i])
        return -9;

    openFile(filepath, filename);
    return 0;
}

int main(){
    char * key, * pass;
    key = (char*) malloc(BUFFER);
    if(!key){
        sendFail();
        return 0;
    }
    pass = (char*) malloc(BUFFER);
    if(!pass){
        free(key);
        sendFail();
        return 0;
    }
    strncpy(key, getenv("REQUEST_URI"), BUFFER); 
    if(!strncmp(key, "/fileServer/", 12))
        memmove(key, key+12, BUFFER-12);
    char * passPos;
    passPos = strrchr(key, '&');
    if(passPos == NULL)
        pass[0] = 0;
    else{
        strncpy(pass, passPos + 1, strlen(passPos+1));
        pass[strlen(passPos+1)] = 0;
        *passPos=0;
    }
    for(int i = 0; i < strlen(key); i++)
        if(key[i] < 43 || key[i] > 122){
            free(key);
            free(pass);
            sendFail();
            return 0;
        }
    if(query(key, pass)){
        free(key);
        free(pass);
        sendFail();
        return 0;
    }

    //char *filepath = "./files/uwu.png";
    //if(openFile(filepath) != 0) return -1;
    free(key);
    free(pass);
    return 0;
}
