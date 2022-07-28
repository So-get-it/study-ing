
//#define LIBXML_STATIC

#include <stdio.h>
#include <string.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml/xmlversion.h>
#include <libxml/xmlstring.h>
 
#define   APNS_XML  "apns-full-conf.xml"

 
int parse_apn(const char *file_name, char *qmcc, char *qmnc, char *qapn)
{
    xmlDocPtr 		doc;        /* xml document tree */
    xmlNodePtr 		cur;       /* xml node */
    xmlChar 		*mcc;         /* Operator MCC */
    xmlChar 		*mnc;         /* Operator MNC */
    xmlChar 		*apn;         /* pppd APN */
    //char    		*qmcc = "460";
    //char    		*qmnc = "03";
    int      		found = 0;
 
    if( !file_name )
    {
        printf("Invalid input arguments\n");
    }
 
    doc = xmlReadFile(file_name, "UTF_8", XML_PARSE_RECOVER);  //先读入需要解析的xml文件
    if (doc == NULL)
    {
        fprintf(stderr, "cleanup to parse xml file:%s\n", file_name);
        goto cleanup;
    }
    
    cur = xmlDocGetRootElement(doc);  //获取根节点
    if (cur == NULL)
    {
        fprintf(stderr, "Root is empty.\n");
        goto cleanup;
    }
 
    if ( xmlStrcmp(cur->name, (const xmlChar *)"apns") )
    {
        fprintf(stderr, "The root is not apns.\n");
        goto cleanup;
    }
/*   
    if (xmlHasProp(cur, "version"))
    {
        version = xmlGetProp(cur, "version");
        printf("version: %s\n", version);
 
    }
*/

    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
        if ( !xmlStrcmp(cur->name, (const xmlChar *)"apn"))
        {
            mcc = xmlGetProp(cur, "mcc");
            mnc = xmlGetProp(cur, "mnc");
 
            if( !xmlStrcmp(mcc, (const xmlChar *)qmcc) && !xmlStrcmp(mnc, (const xmlChar *)qmnc))
            {
                apn = xmlGetProp(cur, "apn");
                //uid = xmlGetProp(cur, "user");
                //pwd = xmlGetProp(cur, "password");
				
				if(!strstr(apn, "wap") && apn)		//wap上网是早期的，排除不要
				{
                	//printf("mcc:%s mnc:%s apn:%s\n",mcc, mnc, apn);

					strncpy(qapn, apn, strlen(apn));
					found = 1;
				}
                
				/*
				if( !xmlStrstr(apn, "wap") )
                {
                    found = 1;
                }
				*/
            }
        }
        cur = cur->next;
    }


cleanup:
    if (doc)
    {
        xmlFreeDoc(doc);
    }
 
    return found;
}

