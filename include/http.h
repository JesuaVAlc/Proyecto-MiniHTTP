#ifndef HTTP_H
#define HTTP_H

typedef struct {
    char method[16];
    char uri[2048];
    char user_agent[512];
    int  keep_alive;
} HttpRequest;

int  parseHTTP(const char *raw, HttpRequest *req);
void httpRes(int clientfd, const HttpRequest *req);

#endif