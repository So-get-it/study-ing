/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.h
 *    Description:  This head file is socket_client_init.h
 *
 *        Version:  1.0.0(04/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "04/05/22 19:11:52"
 *                 
 ********************************************************************************/

#ifndef _S_CLIENT_H_
#define _S_CLIENT_H_

/*
 * =====================================================================================
 *         Name:  socket_client_init
 *  Description:  To get socket client fd
 *   Input args:
 *                              domain_ip: the hostname for client
 *                                       port: connect server port for client
 *
 * return value:  <0: failure   >0:ok
 * =====================================================================================
 */
extern int socket_client_init(char *domain_ip, int port);

/*
 * =====================================================================================
 *         Name:  SocketConnected
 *  Description:  To judge socket disconnected or not.
 *   Input args:  socket file description
 * return value:  1 (connected) or 0 (disconnected)
 * =====================================================================================
 */
extern int SocketConnected(int sock);

extern int socket_write(int sockfd, char *buf);

extern void socket_term(int *sockfd);

#endif
