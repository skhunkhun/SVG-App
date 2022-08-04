/*Name: Sunveer Khunkhun 
* Student No: 1064512
*/

#include "SVGParser.h"
#include "math.h"

Path *parsePath(xmlNode *node);
Rectangle *parseRect(xmlNode *node);
Circle *parseCir(xmlNode *node);
Group *parseGroup(xmlNode *node);

int getRect(List *list, float area);
int getGroupRect(List *list, float area);
int getCir(List *list, float area);
int getGroupCir(List *list, float area);
int getPath(List *list, const char *data);
int getGroupPath(List *list, const char *data);
int getGroup(List *list, int len);

int pathAtt(List *list);
int rectAtt(List *list);
int cirAtt(List *list);
int groupAtt(List *list);

void circleList(List *list, List *circles);
void groupToCircleList(List *list, List *group);

void pathList(List *list, List *path);
void groupToPathList(List *list, List *group);

void rectList(List *list, List *rectangles);
void groupToRectList(List *list, List *group);

void groupList(List *list, List *group);

void dummyFree();

void getUnits(char *cont, char units[50]);

//A2 Helpers

//Module 1
xmlDocPtr SVGtoXMLTree(const SVG *img);
bool SVGtoXMLTreeAttributes(xmlNodePtr node, List *otherAttributes);
bool SVGtoXMLTreeRects(xmlNodePtr root_node, List *rectangles);
bool SVGtoXMLTreeCirc(xmlNodePtr root_node, List *circles);
bool SVGtoXMLTreePath(xmlNodePtr root_node, List *path);
bool SVGtoXMLTreeGroup(xmlNodePtr root_node, List *group);

bool validXMLDoc(xmlDocPtr doc, const char *schemaFile);

//Module 2

bool getOtherAttributes(List *otherAttributes, Attribute*newAttribute);

//Module 3

void numLists(List *groups, int *rNum, int *cNum, int *pNum, int *gNum);

//A3 javascript glue functions

char *fileToJSON(char *file);

