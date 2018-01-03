#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/HTMLparser.h>
#include "foxpath.h"


int run(map* args){
	xmlInitParser();
	px(json(execute_xpath_expression(file_read(map_id(args,1),1,1),map_id(args,2)),1),1);
	xmlCleanupParser();
	return 0;
};

map* execute_xpath_expression(char* in, char* xpathExpr){
	xmlChar* nsList=NULL;

	xmlDocPtr doc=htmlReadDoc(in,"https://habibur.com/","utf8",HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
	if(!doc){ fox_error("error parsing HTML",0); };

	xmlXPathContextPtr xpathCtx=xmlXPathNewContext(doc);
	if(!xpathCtx){ xmlFreeDoc(doc); fox_error("XML parsing failed",0); };


	xmlXPathObjectPtr xpathObj=xmlXPathEvalExpression(xpathExpr,xpathCtx);
	if(!xpathObj){ xmlXPathFreeContext(xpathCtx); xmlFreeDoc(doc); fox_error(xstr("error in expression ", xpathExpr, End),0); };

	map* ret=print_xpath_nodes(xpathObj->nodesetval);

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(doc);
	return ret;
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
map* print_xpath_nodes(void* nodes1) {
	if(!nodes1){ return NULL; };
	map* ret=new_vec();
	xmlNodeSetPtr nodes=nodes1;
	for(int i = 0; i < nodes->nodeNr; ++i){
		xmlNodePtr cur = nodes->nodeTab[i];
		if(cur->type==XML_NAMESPACE_DECL){ continue; };
		char* name=cur->name;
		if(cur->ns){ name=xstr(cur->ns->href,name, End); };
		char* value=cur->children ? (cur->children->content ? str_dup(cur->children->content) : NULL) : NULL;
		vec_add(ret,value); };
	return ret;
};

