//
// linenoise example server
//
// simple network socket server serving a single telnet client
//

#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "linenoise.h"

//
//
//

#define LISTEN_PORT 6510

//
//
//
static void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc,"hello");
        linenoiseAddCompletion(lc,"hello there");
    } else {
        linenoiseAddCompletion(lc,"_nothing to see here");
        linenoiseAddCompletion(lc,"_nowhere to go from here");
    }
}

//
//
//
int main()
{
    int listen_fd;
    int comm_fd;
    int sockopt;
    struct sockaddr_in servaddr;

    printf("Start\n");

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockopt = 1;
    setsockopt(listen_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&sockopt, sizeof(sockopt));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);

    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    printf("Listen on port %d\n", LISTEN_PORT);
    listen(listen_fd, 10);

    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
    printf("Connected\n");

    /* Set the completion callback.
     * This will be called every time the user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);

    /* Load history from file.
     * The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); // Load the history at startup

    // telnet terminal setup (hack'ish)
    write(comm_fd, "\xFF\xFB\x01", 3); // IAC WILL ECHO
    write(comm_fd, "\xFF\xFB\x03", 3); // IAC WILL SUPPRESS GO-AHEAD
    write(comm_fd, "\xFF\xFC\x22", 3); // IAC WONT LINEMODE
    write(comm_fd, "\xFF\xFE\x2D", 3); // IAC DONT LOCAL ECHO

    while (1)
    {
        char *s = linenoiseMain(comm_fd, comm_fd, "prompt>");
        if (s) {
            printf("%s:%d: (%d) '%s'\n", __func__, __LINE__, strlen(s), s);
            if (s[0] != '\0') {
                linenoiseHistoryAdd(s); // Add to the history.
            }
        } else {
            printf("%s:%d: BREAK\n", __func__, __LINE__);
            break;
        }
    }

    printf("Save history\n");
    linenoiseHistorySave("history.txt"); // Save the history to disk.

    printf("Stop\n");

    return 0;
}

//
// EOF
//
