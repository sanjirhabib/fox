#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/HTMLparser.h>
#include <fox.h>
map* xpath(char* in, char* x_path){
	xmlInitParser();
	xmlChar* nsList=NULL;

	xmlDocPtr doc=htmlReadDoc(in,"https://habibur.com/","utf8",HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
	if(!doc){ xmlCleanupParser(); fox_error("error parsing HTML",0); };

	xmlXPathContextPtr xpathCtx=xmlXPathNewContext(doc);
	if(!xpathCtx){ xmlFreeDoc(doc); xmlCleanupParser(); fox_error("XML parsing failed",0); };


	xmlXPathObjectPtr xpathObj=xmlXPathEvalExpression(x_path,xpathCtx);
	if(!xpathObj){ xmlXPathFreeContext(xpathCtx); xmlFreeDoc(doc); xmlCleanupParser(); fox_error(xstr("error in expression ", x_path, End),0); };

	xmlNodeSetPtr nodes=xpathObj->nodesetval;
	if(!nodes){ xmlXPathFreeContext(xpathCtx); xmlFreeDoc(doc); xmlCleanupParser(); return NULL; };
	map* ret=new_vec();
	for(int i = 0; i < nodes->nodeNr; ++i){
		xmlNodePtr cur = nodes->nodeTab[i];
		if(cur->type==XML_NAMESPACE_DECL){ continue; };
		char* name=cur->name;
		if(cur->ns){ name=xstr(cur->ns->href,name, End); };
		char* value=cur->children ? (cur->children->content ? str_dup(cur->children->content) : NULL) : NULL;
		vec_add(ret,value);
	};
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return ret;
};
