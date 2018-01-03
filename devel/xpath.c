#include "xpath.h"
int run(map* args){
	if(map_len(args)!=3){ usage(map_id(args,0)); };
	px(json(xpath(file_read(map_id(args,1),1,1),map_id(args,2)),1),1);
	return 0;
};
void usage(char* name){
	px(xstr("", 
	"Usage: ", name, " <filename> <xpath>\n", 
	"	returns the node value from xpath search.\n", 
	"Example:\n", 
	"\t", name, " index.html '//a/@href'\n", 
	"\t", name, " index.html '//meta[@property=\"og:title\"]/@content'\n", 
	"\t", name, " index.html '//title'", 
	"", End),1);
	exit(0);
};
//struct _xmlNode {
//    xmlElementType   type;	/* type number, must be second ! */
//    const xmlChar   *name;      /* the name of the node, or the entity */
//    struct _xmlNode *children;	/* parent->childs link */
//    struct _xmlNode *last;	/* last child link */
//    struct _xmlNode *parent;	/* child->parent link */
//    struct _xmlNode *next;	/* next sibling link  */
//    struct _xmlNode *prev;	/* previous sibling link  */
//    struct _xmlDoc  *doc;	/* the containing document */
//    xmlNs           *ns;        /* pointer to the associated namespace */
//    xmlChar         *content;   /* the content */
//    struct _xmlAttr *properties;/* properties list */
//    xmlNs           *nsDef;     /* namespace definitions on this node */
//    void            *psvi;	/* for type/PSVI informations */
//    unsigned short   line;	/* line number */
//    unsigned short   extra;	/* extra data for XPath/XSLT */
//};
//struct _xmlAttr {
//    void           *_private;	/* application data */
//    xmlElementType   type;      /* XML_ATTRIBUTE_NODE, must be second ! */
//    const xmlChar   *name;      /* the name of the property */
//    struct _xmlNode *children;	/* the value of the property */
//    struct _xmlNode *last;	/* NULL */
//    struct _xmlNode *parent;	/* child->parent link */
//    struct _xmlAttr *next;	/* next sibling link  */
//    struct _xmlAttr *prev;	/* previous sibling link  */
//    struct _xmlDoc  *doc;	/* the containing document */
//    xmlNs           *ns;        /* pointer to the associated namespace */
//    xmlAttributeType atype;     /* the attribute type if validating */
//    void            *psvi;	/* for type/PSVI informations */
//};
