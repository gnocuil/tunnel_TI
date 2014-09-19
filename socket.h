#pragma once

#define BUF_LEN 4096
extern char buf[BUF_LEN];
extern char buf4[BUF_LEN];

int socket_init();
int socket_init_tun();
int handle_socket();
int socket_send(char *buf, int len);
int socket_send4(char *buf, int len);
