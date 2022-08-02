/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  parse_apn.h
 *    Description:  This file is the function to parse APN.
 *
 *        Version:  1.0.0(13/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "13/07/22 22:18:27"
 *
 ********************************************************************************/


#ifndef _PARSE_APN_H
#define _PARSE_APN_H


#define   APNS_XML  "apns-full-conf.xml"


extern int parse_apn(const char *file_name, char *qmcc, char *qmnc, char *apn);


#endif
