

#ifndef _PARSE_APN_H
#define _PARSE_APN_H


#define   APNS_XML  "apns-full-conf.xml"


extern int parse_apn(const char *file_name, char *qmcc, char *qmnc, char *apn);


#endif
