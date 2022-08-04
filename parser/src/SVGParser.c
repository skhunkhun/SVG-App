/*Name: Sunveer Khunkhun 
* Student No: 1064512
*/

#include "SVGParser.h"
#include "SVGHelper.h"

//function to create an SVG struct
SVG* createSVG(const char* fileName){

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(fileName, NULL, 0); //create an xmldoc

    if (doc == NULL) { //if xmldoc is null then return null

        printf("error: could not parse file %s\n", fileName);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    } 

    root_element = xmlDocGetRootElement(doc); //get the root node of the doc

    SVG *newSVG = (SVG*)malloc(sizeof(SVG)); //allocate memory for the svg struct

    //initialize all of the lists of the svg struct
    newSVG->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    newSVG->rectangles = initializeList(&rectangleToString,&deleteRectangle, &compareRectangles);
    newSVG->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    newSVG->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    newSVG->paths = initializeList(&pathToString, &deletePath, &comparePaths);

    xmlNode *node = NULL;

    xmlAttr *attr;
    xmlNode *cur_node = root_element;

    strcpy(newSVG->namespace, "");
    strcpy(newSVG->title, "");
    strcpy(newSVG->description, "");

    strcpy(newSVG->namespace, (const char*)root_element->ns->href);

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) { //loop through the root node attributes and create a list of attributes

        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);

        //create a new Attribute structure and insert into the otherAttributes list for the SVG structure
        Attribute *svgAtt = malloc(sizeof(Attribute) + sizeof(char) * strlen(cont) + 1);
        svgAtt->name = malloc(strlen((char *)attr->name) + 1);
        strcpy(svgAtt->name, (char *)attr->name);
        strcpy(svgAtt->value, cont);

        insertBack(newSVG->otherAttributes, (void*)svgAtt);
    }

    for(node = root_element->children; node != NULL; node = node->next){ //loop through the children of the root node

        if(node->type == XML_ELEMENT_NODE){

            if(strcmp("title", (const char*)node->name) == 0){ 

                char *cont = (char*)xmlNodeGetContent(node);
                strcpy(newSVG->title, cont);
                free(cont);
            }
        
            if(strcmp("desc", (const char*)node->name) == 0){

                char *cont = (char*)xmlNodeGetContent(node);
                strcpy(newSVG->description, cont);
                free(cont);

            }

            if(strcmp("rect", (const char*)node->name) == 0){ //if child is rect, then call parse rectangle

                Rectangle *rec = parseRect(node);
                insertBack(newSVG->rectangles, (void*)rec);

            }

            if(strcmp("path", (const char*)node->name) == 0){ //if child is path, then call parse path

                Path *path = parsePath(node);
                insertBack(newSVG->paths, (void*)path);

            }

            if(strcmp("circle", (const char*)node->name) == 0){ //if child is circle, then call parse circle and insert into list

                Circle *circle = parseCir(node);
                insertBack(newSVG->circles, (void*)circle);
            }

            if(strcmp("g", (const char*)node->name) == 0){ //if child is group, then call parse group and insert into list

                Group *group = parseGroup(node);
                insertBack(newSVG->groups, (void*)group);
            }

        }
    }   

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return newSVG;
}

//function to create a valid SVG struct
SVG* createValidSVG(const char* fileName, const char* schemaFile) {

    xmlDoc *doc = NULL;

    doc = xmlReadFile(fileName, NULL, 0); //create an xmldoc

    if(validXMLDoc(doc, schemaFile) == false){ //check if doc is valid

        return NULL;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    SVG *newSVG = createSVG(fileName); //create a new SVG struct

    return newSVG;

}

//function to write an SVG struct to a file
bool writeSVG(const SVG* img, const char* fileName){

    //if parameters are NULL, then return false
    if(img == NULL || fileName == NULL || strcmp(fileName, "") == 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return false;
    }

    char *validFile = strstr(fileName, ".svg"); //check if filename is a valid svg file

    if(!validFile){

        return false;
    }

    xmlDocPtr doc = NULL;

    doc = SVGtoXMLTree(img); //create a doc given the SVG struct

    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1); //save the doc to a file

    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}

//function to validate an SVG doc
bool validateSVG(const SVG* img, const char* schemaFile){

    if(img == NULL || schemaFile == NULL || strcmp(schemaFile, "") == 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return false;
    }

    char *validFile = strstr(schemaFile, ".xsd"); //check if schema file is valid

    if(!validFile){

        return false;
    }

    xmlDocPtr doc = NULL;

    doc = SVGtoXMLTree(img); //create a doc given the SVG struct

    if(validXMLDoc(doc, schemaFile) == false){ //check if doc is valid

        return false;
    }

    if(doc == NULL){ //check if doc passes all of the parameters (non negative values, etc.)
        return false;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}

//function to print out the SVG struct into a string
char* SVGToString(const SVG* img){

    char* tmpStr;
	int len = 0;
	
	if (img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){
		return NULL;
	}

    //turn the list contents into a string
    char *tmp = toString(img->otherAttributes);
    char *rect = toString(img->rectangles);
    char *path = toString(img->paths);
    char *cir = toString(img->circles);
    char *group = toString(img->groups);

    //allocate memory for the string contents
    len = strlen(img->namespace) + strlen(img->title) + strlen(img->description) + strlen(tmp) + strlen(rect) + strlen(path) + strlen(cir) + strlen(group) + 100;
	tmpStr = malloc(sizeof(char)*len);
	
	sprintf(tmpStr, "\nNamespace: %s \nTitle: %s \nDescription: %s\nAttributes: %s\n\nRectangle: %s\n\nPath: %s\n\nCircle: %s\n\nGroup:\n%s\n\n", img->namespace, img->title, img->description, tmp, rect, path, cir, group);

    free(tmp);
    free(rect);
    free(path);
    free(cir);
    free(group);
	
	return tmpStr;
}

//function to free all of the memory allocated to the SVG struct and delete the struct
void deleteSVG(SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return;
    }

    freeList(img->rectangles);
    freeList(img->groups);
    freeList(img->circles);
    freeList(img->otherAttributes);
    freeList(img->paths);
    free(img);

}

//function to delete an attribute
void deleteAttribute( void* data){

    Attribute* tmpAttr = NULL;
	
	if (data == NULL){
		return;
	}
	
	tmpAttr = (Attribute*)data;

    free(tmpAttr->name);
	free(tmpAttr);

}

//function to turn the attribute contents into a string
char* attributeToString(void* data){

    char* tmpStr;
	Attribute* tmpAtt = NULL;
	int len = 0;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpAtt = (Attribute*)data; //cast into an attribute struct
	len = strlen(tmpAtt->name) + strlen(tmpAtt->value) + 10; //allocate memory for string
	tmpStr = (char*)malloc(sizeof(char) * len);
	
	sprintf(tmpStr, "%s %s ", (char*)tmpAtt->name, (char*)tmpAtt->value);
	
	return tmpStr;
}

//function to compare the attributes
int compareAttributes(const void *first, const void *second){

    return 0;
}

//function to delete group from the SVG struct
void deleteGroup(void* data){

    Group* tmpG = NULL;
	
	if (data == NULL){
		return;
	}
	
	tmpG = (Group*)data; //cast data to a Group struct

    //free all contents
    freeList(tmpG->circles);
    freeList(tmpG->paths);
    freeList(tmpG->rectangles);
    freeList(tmpG->groups);
    freeList(tmpG->otherAttributes);
	free(tmpG);

}

//function to turn group contents into a string
char* groupToString(void* data){

    char* tmpStr;
	Group *tmpG = NULL;
	int len = 0;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpG = (Group*)data;

    //turn group contents to a string
    char *att = toString(tmpG->otherAttributes);
    char *cir = toString(tmpG->circles);
    char *path = toString(tmpG->paths);
    char *rect = toString(tmpG->rectangles);
    char *group = toString(tmpG->groups);

    len = strlen(att) + strlen(cir) + strlen(path) + strlen(rect) + strlen(group) + 500;
	tmpStr = (char*)malloc(sizeof(char) * len);
	
    sprintf(tmpStr, "Attributes: %s\n\nCircle: %s\nPath: %s\nRectangle: %s\n\nGroup of group: %s\n", att, cir, path, rect, group);

    free(att);
    free(cir);
    free(path);
    free(rect);
    free(group);
	
	return tmpStr;
}
int compareGroups(const void *first, const void *second){

    return 0;
}

//function to delete a rectangle from the SVG struct
void deleteRectangle(void* data){

    Rectangle* tmpRect = NULL;
	
	if (data == NULL){
		return;
	}
	
	tmpRect = (Rectangle*)data;

    freeList(tmpRect->otherAttributes);
	free(tmpRect);

}

//function to turn a rectangle into a string
char* rectangleToString(void* data){

    char* tmpStr;
	Rectangle *tmpRect = NULL;
	int len = 0;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpRect = (Rectangle*)data;
    char *att = toString(tmpRect->otherAttributes);

    len = strlen(att) + 100;
	tmpStr = (char*)malloc(sizeof(char) * len);
	
    sprintf(tmpStr, "\nx = %f y = %f width = %f height = %f units = %s\nAttributes: %s\n", tmpRect->x, tmpRect->y, tmpRect->width, tmpRect->height, tmpRect->units, att);

    free(att);
	
	return tmpStr;

}
int compareRectangles(const void *first, const void *second){

    return 0;
}

//function to delete a circle from the SVG struct
void deleteCircle(void* data){

    Circle* tmpCir = NULL;
	
	if (data == NULL){
		return;
	}
	
	tmpCir = (Circle*)data;

    freeList(tmpCir->otherAttributes);
	free(tmpCir);

}

//function to turn a circle into string contents
char* circleToString(void* data){

    char* tmpStr;
	Circle *tmpCir = NULL;
	int len = 0;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpCir = (Circle*)data;
    char *att = toString(tmpCir->otherAttributes);
    
    len = strlen(att) + 100;
	tmpStr = malloc(sizeof(char) * len);
	
    sprintf(tmpStr, "\ncx = %f cy = %f r = %f units = %s\nAttributes: %s\n", tmpCir->cx, tmpCir->cy, tmpCir->r, tmpCir->units, att);

    free(att);
	
	return tmpStr;
}
int compareCircles(const void *first, const void *second){

    return 0;
}

//function to delete a path from the SVG struct
void deletePath(void* data){

    Path* tmpPath = NULL;
	
	if (data == NULL){
		return;
	}
	
	tmpPath = (Path*)data;

    freeList(tmpPath->otherAttributes);
	free(tmpPath);

}

//function turn a path into string contents
char* pathToString(void* data){

    char* tmpStr;
	Path *tmpPath = NULL;
	int len = 0;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpPath = (Path*)data;
    char *att = toString(tmpPath->otherAttributes);
    
    len = strlen(att) + 100;
	tmpStr = (char*)malloc(sizeof(char) * strlen(tmpPath->data) * len);
	
    sprintf(tmpStr, "\ndata = %s \nAttributes: %s\n", tmpPath->data, att);

    free(att);
	
	return tmpStr;
}
int comparePaths(const void *first, const void *second){

    return 0;
}

//function to get a list of pointers to all of the rectangles in the SVG struct
List* getRects(const SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return NULL;
    }
      
    List * list = initializeList(&rectangleToString, &dummyFree, &compareRectangles); //initialize the list

    rectList(list, img->rectangles);
    groupToRectList(list, img->groups);

    return list;
}

// Function that returns a list of all circles in the struct.  
List* getCircles(const SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return NULL;
    }

    List *list = initializeList(&circleToString, &dummyFree, &compareCircles);

    circleList(list, img->circles);
    groupToCircleList(list, img->groups);

    return list;
}

// Function that returns a list of all groups in the struct.  
List* getGroups(const SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return NULL;
    }

    List * list = initializeList(&groupToString, &dummyFree, &compareGroups);

    groupList(list, img->groups);

    return list;

}

// Function that returns a list of all paths in the struct.  
List* getPaths(const SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return NULL;
    }

    List * list = initializeList(&pathToString, &dummyFree, &comparePaths);

    pathList(list, img->paths);
    groupToPathList(list, img->groups);

    return list;

}

// Function that returns the number of all rectangles with the specified area
int numRectsWithArea(const SVG* img, float area){

    if(img == NULL || area <= 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return 0;
    }

    int recCounter = 0;

    recCounter += getRect(img->rectangles, area);
    recCounter += getGroupRect(img->groups, area);

    return recCounter;
    

}

// Function that returns the number of all circles with the specified area
int numCirclesWithArea(const SVG* img, float area){

    if(img == NULL || area <= 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return 0;
    }

    int cirCounter = 0;

    cirCounter += getCir(img->circles, area);
    cirCounter += getGroupCir(img->groups, area);

    return cirCounter;

}

// Function that returns the number of all paths with the specified data
int numPathsWithdata(const SVG* img, const char* data){

    if(img == NULL || strcmp(data, "") == 0 || data == NULL|| img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return 0;
    }

    int pathCounter = 0;

    pathCounter += getPath(img->paths, data);
    pathCounter += getGroupPath(img->groups, data);

    return pathCounter;

}

// Function that returns the number of all groups with the specified length 
int numGroupsWithLen(const SVG* img, int len){

    if(img == NULL || len <= 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return 0;
    }

    int result = 0;

    result = getGroup(img->groups, len);

    return result;

}

//function to return the number of attributes in the whole SVG struct
int numAttr(const SVG* img){

    if(img == NULL || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return 0;
    }

    int attCounter = 0;

    attCounter += getLength(img->otherAttributes);
    attCounter += pathAtt(img->paths);
    attCounter += rectAtt(img->rectangles);
    attCounter += cirAtt(img->circles);
    attCounter += groupAtt(img->groups);

    return attCounter;

}

//A2 Functions -------------------------------------------------------------------------------------------

//function to set or create an attribute 
bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute*newAttribute){

    if(img == NULL || elemType > 4 || strcmp(newAttribute->name, "") == 0 || strcmp(newAttribute->value, "") == 0 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return false;
    }

    //if SVG, then set the attribute
    if(elemType == 0){

        bool valid = getOtherAttributes(img->otherAttributes, newAttribute);

        if(valid == true){ 

            return true;
            
        } else { 

            return false;
        }

    }

    //if circle, then set the correct attribute
    if(elemType == 1){

        ListIterator itr = createIterator(img->circles);

        void* elem;

        int i = 0;
        while ((elem = nextElement(&itr)) != NULL && elemIndex <= getLength(img->circles)){ //iterate through the circle list

            Circle *tmpC = (Circle*)elem;

            if(i == elemIndex){ //set the correct attribute given the index

                if(strcmp(newAttribute->name, "cx") == 0){

                    float cx = atof(newAttribute->value);
                    tmpC->cx = cx;
                    
                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(strcmp(newAttribute->name, "cy") == 0){

                    float cy = atof(newAttribute->value);
                    tmpC->cy = cy;
                    
                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(strcmp(newAttribute->name, "r") == 0){
                    
                    float r = atof(newAttribute->value);

                    if(r < 0){

                        printf("\nInvalid value of r\n");
                        return false;
                    }

                    tmpC->r = r;
                    
                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(getOtherAttributes(tmpC->otherAttributes, newAttribute) == true){

                    return true;
                }


            }

            i++;
        }

    }

    //if rectangle, then set correct attribute
    if(elemType == 2){

        ListIterator itr = createIterator(img->rectangles);

        void* elem;

        int i = 0;
        while ((elem = nextElement(&itr)) != NULL && elemIndex <= getLength(img->rectangles)){ //iterate through the group list

            Rectangle *tmpA = (Rectangle*)elem;

            if(i == elemIndex){

                if(strcmp(newAttribute->name, "x") == 0){

                    float x = atof(newAttribute->value);
                    tmpA->x = x;
                    
                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(strcmp(newAttribute->name, "y") == 0){

                    float y = atof(newAttribute->value);
                    tmpA->y = y;

                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(strcmp(newAttribute->name, "width") == 0){

                    float width = atof(newAttribute->value);

                    if(width < 0){

                        return false;
                    }

                    tmpA->width = width;

                    deleteAttribute((void*)newAttribute);
                    return true;
                }

                if(strcmp(newAttribute->name, "height") == 0){

                    float height = atof(newAttribute->value);

                    if(height < 0){

                        return false;
                    }

                    tmpA->height = height;

                    deleteAttribute((void*)newAttribute);
                    return true;
                } 

                if(getOtherAttributes(tmpA->otherAttributes, newAttribute) == true){

                    return true;
                }
            }

            i++;

        }


    }

    //if path, then set the correct attribute
    if(elemType == 3){

        ListIterator itr = createIterator(img->paths);

        void* elem;

        int i = 0;
        while ((elem = nextElement(&itr)) != NULL && elemIndex <= getLength(img->paths)){ //iterate through the group list

            Path *tmpP = (Path*)elem;

            if(i == elemIndex){

                if(strcmp(newAttribute->name, "d") == 0){
                    
                    //tmpP = realloc(tmpP, sizeof(Path) * strlen(newAttribute->value) + 10);
                    strcpy(tmpP->data, newAttribute->value);
                    
                    deleteAttribute((void*)newAttribute);
                    return true;

                }

                if(getOtherAttributes(tmpP->otherAttributes, newAttribute) == true){

                    return true;

                } else {

                    return false;
                }

            }

            i++;

        }

    }

    //if group, then change the correct attribute
    if(elemType == 4){

        ListIterator itr = createIterator(img->groups);

        void* elem;

        int i = 0;
        while ((elem = nextElement(&itr)) != NULL && elemIndex <= getLength(img->groups)){ //iterate through the group list

            Group *tmpP = (Group*)elem;

            if(i == elemIndex){

                if(getOtherAttributes(tmpP->otherAttributes, newAttribute) == true){

                    return true;

                } else {

                    return false;
                }

            }

            i++;

        }

    }

    return false;

}

//function to add a new component to a SVG struct
void addComponent(SVG* img, elementType type, void* newElement){

    if(img == NULL || newElement == NULL || type < 1 || type > 3 || img->otherAttributes == NULL || img->circles == NULL || img->groups == NULL || img->paths == NULL || img->rectangles == NULL || strcmp(img->namespace, "") == 0){

        return;
    }

    if(type == 1){

        insertBack(img->circles, newElement);

    } else if(type == 2){

        insertBack(img->rectangles, newElement);

    } else if(type == 3){

        insertBack(img->paths, newElement);

    } else {

        return;
    }

}

//A2 Module 3-----------------------

//function to turn an attribute to a JSON string
char* attrToJSON(const Attribute *a){

    if(a == NULL || a->name == NULL){ //if parameters are NULL, then return a '{}' string

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    char *tmpStr = NULL;
    
    len = strlen(a->name) + strlen(a->value) + 100;
	tmpStr = malloc(sizeof(char) * len);
	
    sprintf(tmpStr, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
	
	return tmpStr;
}

//function to turn a Circle to a JSON string
char* circleToJSON(const Circle *c){

    if(c == NULL || c->otherAttributes == NULL){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    char *tmpStr = NULL;
    char cx[100];
    char cy[100];
    char r[100];

    sprintf(cx, "%f", c->cx);
    sprintf(cy, "%f", c->cy);
    sprintf(r, "%f", c->r);

    int listLen = getLength(c->otherAttributes);
    
    len = strlen(cx) + strlen(cy) + strlen(r) + 100;
	tmpStr = malloc(len);
	
    sprintf(tmpStr, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, listLen, c->units);
	
	return tmpStr;

}

//function to turn a Rectangle to a JSON string
char* rectToJSON(const Rectangle *r){

    if(r == NULL || r->otherAttributes == NULL){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    char *tmpStr = NULL;
    char x[100];
    char y[100];
    char width[100];
    char height[100];

    sprintf(x, "%f", r->x);
    sprintf(y, "%f", r->y);
    sprintf(width, "%f", r->width);
    sprintf(height, "%f", r->height);

    int listLen = getLength(r->otherAttributes);
    
    len = strlen(x) + strlen(y) + strlen(width) + strlen(height) + listLen + 100;
	tmpStr = malloc(len);
	
    sprintf(tmpStr, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, listLen, r->units);
	
	return tmpStr;

}

//Function to turn a Path to a JSON string
char* pathToJSON(const Path *p){

    if(p == NULL || p->otherAttributes == NULL){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    char *tmpStr = NULL;
    
    int listLen = getLength(p->otherAttributes); //get the length of the other attributes list

    len = strlen(p->data) + listLen + 100;
	tmpStr = malloc(sizeof(char) + len);
	
    sprintf(tmpStr, "{\"d\":\"%.64s\",\"numAttr\":%d}", p->data, listLen);
	
	return tmpStr;

}

//Function to turn a Group to a JSON string
char* groupToJSON(const Group *g){

    if(g == NULL || g->otherAttributes == NULL){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    int listLen = 0;
    char *tmpStr = NULL;
    
    int attLen = getLength(g->otherAttributes);
    listLen += getLength(g->rectangles);
    listLen += getLength(g->circles);
    listLen += getLength(g->paths);
    listLen += getLength(g->groups);

    len = attLen + listLen + 100;
	tmpStr = malloc(sizeof(char) * len);
	
    sprintf(tmpStr, "{\"children\":%d,\"numAttr\":%d}", listLen, attLen);
	
	return tmpStr;

}

//function to get the number of lists of an SVG struct to a JSON string
char* SVGtoJSON(const SVG* image){

    if(image == NULL || image->otherAttributes == NULL || image->circles == NULL || image->rectangles == NULL || image->paths == NULL || image->groups == NULL || strcmp(image->namespace, "") == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "{}");

        return str;
    }

    int len = 0;
    int gLen = 0;
    int rLen = 0;
    int cLen = 0;
    int pLen = 0;
    char *tmpStr = NULL;

    rLen += getLength(image->rectangles);
    cLen += getLength(image->circles);
    pLen += getLength(image->paths);
    gLen += getLength(image->groups);

    numLists(image->groups, &rLen, &cLen, &pLen, &gLen);

    len = 100;
	tmpStr = malloc(sizeof(char) * len);

    sprintf(tmpStr, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", rLen, cLen, pLen, gLen);
	
	return tmpStr;

}

//function to turn a list of attributes to a JSON string
char* attrListToJSON(const List *list){

    if(list == NULL || getLength((List*)list) == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "[]");

        return str;
    }

    char *att = malloc(10);
    strcpy(att, "[");

    ListIterator itr = createIterator((List*)list);

    void* elem;

    int i = 0;
    while ((elem = nextElement(&itr)) != NULL){ //loop through the list

        Attribute *tmpA = (Attribute*)elem;

        char *newAtt = attrToJSON(tmpA); //turn the current attribute to a JSON string

        att = (char*) realloc(att, strlen(att) + strlen(newAtt) + 100); //reallocate att for the JSON string
 
        strcat(att, newAtt); //add the attribute to the list string

        if(i != getLength((List *)list) - 1) { //only add comma if the attribute is not the last element on the list

            strcat(att, ",");
        }
        i++;

        free(newAtt);

    }

    strcat(att, "]"); //add ']' to close the JSON string

    return att;
}

//function to turn a list of circles to a JSON string
char* circListToJSON(const List *list){

    if(list == NULL || getLength((List*)list) == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "[]");

        return str;
    }

    char *att = malloc(10);
    strcpy(att, "[");

    ListIterator itr = createIterator((List*)list);

    void* elem;

    int i = 0;
    while ((elem = nextElement(&itr)) != NULL){

        Circle *tmpA = (Circle*)elem;

        char *newAtt = circleToJSON(tmpA);

        att = (char*) realloc(att, strlen(att) + strlen(newAtt) + 100);
 
        strcat(att, newAtt);

        if(i != getLength((List *)list) - 1) {

            strcat(att, ",");
        }
        i++;

        free(newAtt);
    }

    strcat(att, "]");

    return att;
}

//function to turn a list of rectangles to a JSON string
char* rectListToJSON(const List *list){

    if(list == NULL || getLength((List*)list) == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "[]");

        return str;
    }

    char *att = malloc(10);
    strcpy(att, "[");

    ListIterator itr = createIterator((List*)list);

    void* elem;

    int i = 0;
    while ((elem = nextElement(&itr)) != NULL){

        Rectangle *tmpA = (Rectangle*)elem;

        char *newAtt = rectToJSON(tmpA);

        att = (char*) realloc(att, strlen(att) + strlen(newAtt) + 100);
 
        strcat(att, newAtt);

        if(i != getLength((List *)list) - 1) {

            strcat(att, ",");
        }
        i++;

        free(newAtt);
    }

    strcat(att, "]");

    return att;
}

//function to turn a list of paths to a JSON string
char* pathListToJSON(const List *list){

    if(list == NULL || getLength((List*)list) == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "[]");

        return str;
    }

    char *att = malloc(10);
    strcpy(att, "[");

    ListIterator itr = createIterator((List*)list);

    void* elem;

    int i = 0;
    while ((elem = nextElement(&itr)) != NULL){

        Path *tmpA = (Path*)elem;

        char *newAtt = pathToJSON(tmpA);

        att = (char*) realloc(att, strlen(att) + strlen(newAtt) + 100);
 
        strcat(att, newAtt);

        if(i != getLength((List *)list) - 1) {

            strcat(att, ",");
        }
        i++;

        free(newAtt);
    }

    strcat(att, "]");

    return att;

}

//function to turn a list of groups to a JSON string
char* groupListToJSON(const List *list){

    if(list == NULL || getLength((List*)list) == 0){

        char *str = malloc(strlen("{}--"));
        strcpy(str, "[]");

        return str;
    }

    char *att = malloc(10);
    strcpy(att, "[");

    ListIterator itr = createIterator((List*)list);

    void* elem;

    int i = 0;
    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpA = (Group*)elem;

        char *newAtt = groupToJSON(tmpA);

        att = (char*) realloc(att, strlen(att) + strlen(newAtt) + 100);
 
        strcat(att, newAtt);

        if(i != getLength((List *)list) - 1) {

            strcat(att, ",");
        }
        i++;

        free(newAtt);
    }

    strcat(att, "]");

    return att;

}

//Module 3 Bonus
SVG* JSONtoSVG(const char* svgString){

    if(svgString == NULL){

        return NULL;
    }

    SVG *newSVG = (SVG*)malloc(sizeof(SVG)); //allocate memory for the svg struct

    strcpy(newSVG->title, "");
    strcpy(newSVG->description, "");

    //initialize all of the lists of the svg struct
    newSVG->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    newSVG->rectangles = initializeList(&rectangleToString,&deleteRectangle, &compareRectangles);
    newSVG->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    newSVG->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    newSVG->paths = initializeList(&pathToString, &deletePath, &comparePaths);

    strcpy(newSVG->namespace, "http://www.w3.org/2000/svg");

    char *title = malloc(strlen(svgString));
    char *desc = malloc(strlen(svgString));
    strcpy(title, "");
    strcpy(desc, "");

    sscanf(svgString, "{\"title\":\"%[^\"]\",\"descr\":\"%[^\b]\"}", title, desc);

    if(strlen(title) == 0){

        sscanf(svgString, "{\"title\":\"\",\"descr\":\"%[^\b]\"}", desc);
        desc[strlen(desc) - 2] = '\0';

        strcpy(newSVG->title, "");
        strcpy(newSVG->description, desc);

    } else if (strlen(desc) == 0){

        strcpy(newSVG->description, "");
        
    } else {

        desc[strlen(desc) - 2] = '\0';
        strcpy(newSVG->title, title);
        strcpy(newSVG->description, desc);


    }
    
    free(title);
    free(desc);
    return newSVG;

}

//function to turn a svgString into a Rectangle struct
Rectangle* JSONtoRect(const char* svgString){

    if(svgString == NULL){

        return NULL;
    }

    Rectangle *newRec = (Rectangle*)malloc(sizeof(Rectangle)); //allocate memory for the Rectangle struct

    //initialize all of the lists of the Rectangle struct
    newRec->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    float x = 0;
    float y = 0;
    float w = 0;
    float h = 0;
    char *units = malloc(strlen(svgString));
    strcpy(units, "");

    sscanf(svgString, "{\"x\":%f,\"y\":%f,\"w\":%f,\"h\":%f,\"units\":\"%[a-zA-Z]\"}", &x, &y, &w, &h, units);

    newRec->x = x;
    newRec->y = y;
    newRec->width = w;
    newRec->height = h;

    if(strlen(units) == 0){

        strcpy(newRec->units, "");

    } else{

        strcpy(newRec->units, units);

    }
   
    free(units);

    return newRec;
}   

//function to turn a JSON string into a Circle struct
Circle* JSONtoCircle(const char* svgString){

    if(svgString == NULL){

        return NULL;
    }

    Circle *newC = (Circle*)malloc(sizeof(Circle)); //allocate memory for the Circle struct

    //initialize all of the lists of the Circle struct
    newC->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    float cx = 0;
    float cy = 0;
    float r = 0;
    char *units = malloc(strlen(svgString));
    strcpy(units, "");

    sscanf(svgString, "{\"cx\":%f,\"cy\":%f,\"r\":%f,\"units\":\"%[a-zA-Z]\"}", &cx, &cy, &r, units); //get the values for the circle struct

    //set the values
    newC->cx = cx;
    newC->cy = cy;
    newC->r = r;

    if(strlen(units) == 0){

        strcpy(newC->units, "");

    } else{

        strcpy(newC->units, units);

    }

    free(units);

    return newC;

}   

char *fileToJSON(char *file){

    const char *schema = "svg.xsd";
    SVG *newSVG = createValidSVG(file, schema);
    bool valid = validateSVG(newSVG, schema);

    if(valid != false){

        char *svgJSON = SVGtoJSON(newSVG);
        deleteSVG(newSVG);
        return svgJSON;
    }

    char *svgJSON = malloc(strlen("{} "));
    strcpy(svgJSON, "{}");
    deleteSVG(newSVG);
    return svgJSON;
    
}

char *getTitle(char *file){

    SVG *newSVG = createSVG(file);
    char *title;

    if(newSVG == NULL){

        title = malloc(2);
        strcpy(title, "");

        return title;
    }

    title = malloc(strlen(newSVG->title) + 1);
    strcpy(title, newSVG->title);

    return title;
}

char *getDesc(char *file){

    SVG *newSVG = createSVG(file);
    char *desc;

    if(newSVG == NULL){

        desc = malloc(2);
        strcpy(desc, "");
        
        return desc;
    }

    desc = malloc(strlen(newSVG->description) + 1);
    strcpy(desc, newSVG->description);

    return desc;
}

char *getSVGAttrList(char *file){

    SVG *newSVG = createSVG(file);
    char *attList;

    if(newSVG == NULL){

        attList = malloc(3);
        strcpy(attList, "[]");
        return attList;
    } 

    attList = attrListToJSON(newSVG->otherAttributes);
    deleteSVG(newSVG);

    return attList;
}

char *getRectList(char *file){

    SVG *newSVG = createSVG(file);
    char *rectList;

    if(newSVG == NULL){

        rectList = malloc(3);
        strcpy(rectList, "[]");
        return rectList;
    } 

    rectList = rectListToJSON(newSVG->rectangles);
    deleteSVG(newSVG);

    return rectList;
}

char *getCircList(char *file){

    SVG *newSVG = createSVG(file);
    char *circList;

    if(newSVG == NULL){

        circList = malloc(3);
        strcpy(circList, "[]");
        return circList;
    } 

    circList = circListToJSON(newSVG->circles);
    deleteSVG(newSVG);

    return circList;

}

char *getPathList(char *file){

    SVG *newSVG = createSVG(file);
    char *pathList;

    if(newSVG == NULL){

        pathList = malloc(3);
        strcpy(pathList, "[]");
        return pathList;
    } 

    pathList = pathListToJSON(newSVG->paths);
    deleteSVG(newSVG);

    return pathList;

}

char *getGroupList(char *file){

    SVG *newSVG = createSVG(file);
    char *groupList;

    if(newSVG == NULL){

        groupList = malloc(3);
        strcpy(groupList, "[]");
        return groupList;
    } 

    groupList = groupListToJSON(newSVG->groups);
    deleteSVG(newSVG);

    return groupList;

}

char *getOtherAttr(char *file, int index, int type){

    SVG *newSVG = createSVG(file);

    if(newSVG == NULL){

        char *empty = malloc(3);
        strcpy(empty, "[]");
        return empty;
    }

    if(type == 1){

        Circle *cir;
        void *elem;
        int currentIdx = 0;

        ListIterator itr = createIterator(newSVG->circles);

        while(currentIdx != index){

            elem = nextElement(&itr);
            cir = (Circle*)elem;
            currentIdx++;
        }

        char *otherAttr = attrListToJSON(cir->otherAttributes);
        deleteSVG(newSVG);

        return otherAttr;

    } else if(type == 2){

        Rectangle *rec;
        void *elem;
        int currentIdx = 0;

        ListIterator itr = createIterator(newSVG->rectangles);

        while(currentIdx != index){

            elem = nextElement(&itr);
            rec = (Rectangle*)elem;
            currentIdx++;
        }

        char *otherAttr = attrListToJSON(rec->otherAttributes);
        deleteSVG(newSVG);

        return otherAttr;

    } else if(type == 3){

        Path *path;
        void *elem;
        int currentIdx = 0;

        ListIterator itr = createIterator(newSVG->paths);

        while(currentIdx != index){

            elem = nextElement(&itr);
            path = (Path*)elem;
            currentIdx++;
        }

        char *otherAttr = attrListToJSON(path->otherAttributes);
        deleteSVG(newSVG);

        return otherAttr;

    } else if(type == 4){

        Group *g;
        void *elem;
        int currentIdx = 0;

        ListIterator itr = createIterator(newSVG->groups);

        while(currentIdx != index){

            elem = nextElement(&itr);
            g = (Group*)elem;
            currentIdx++;
        }

        char *otherAttr = attrListToJSON(g->otherAttributes);
        deleteSVG(newSVG);

        return otherAttr;
    }

    char *empty = malloc(3);
    strcpy(empty, "[]");
    return empty;
}

char *renameSVG(char *file, char *newName, char *type){

    SVG *newSVG = createSVG(file);

    if(strcmp(type, "Description") == 0){

        strcpy(newSVG->description, newName);

        if(writeSVG(newSVG, file) != false){

            char *msg = malloc(strlen("Success[]"));
            strcpy(msg, "Success");
            deleteSVG(newSVG);
            return msg;
        }

    } else if(strcmp(type, "Title") == 0){

        strcpy(newSVG->title, newName);

        if(writeSVG(newSVG, file) != false){

            char *msg = malloc(strlen("Success[]"));
            strcpy(msg, "Success");
            deleteSVG(newSVG);
            return msg;
        }

    }

    char *msg = malloc(strlen("Fail[]"));
    strcpy(msg, "Fail");
    deleteSVG(newSVG);
    return msg;

}

char *createSVGtoJSON(char *file, char *JSON){

    SVG *newSVG = JSONtoSVG((const char*)JSON);
    char *schema = "svg.xsd";
    char *msg;

    Rectangle *rec = malloc(sizeof(Rectangle)); //allocate memory for circle struct
    rec->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    rec->x = 1;
    rec->y = 1;
    rec->width = 2;
    rec->height = 1; 
    strcpy(rec->units, "cm");

    Attribute *cirAtt = malloc(sizeof(Attribute) + sizeof(char) * 100);
    cirAtt->name = malloc(strlen("some name like this") + 1);
    strcpy(cirAtt->name, "fill");
    strcpy(cirAtt->value, "red");
    insertBack(rec->otherAttributes, (void*)cirAtt);
    insertBack(newSVG->rectangles, (void*)rec);

    bool valid = validateSVG(newSVG, schema);

    if(valid != false){

        bool write = writeSVG(newSVG, file);

        if(write == false){

            msg = malloc(strlen("Error: Could not Write  "));
            strcpy(msg, "Error: Could not Write");

        } else {

            msg = malloc(strlen("Success: New SVG image created. "));
            strcpy(msg, "Success: New SVG image created.");
        }

    } else {

        msg = malloc(strlen("Error: SVG image is not valid "));
        strcpy(msg, "Error: SVG image is not valid");
    }

    deleteSVG(newSVG);
    return msg;

}

char *addShape(char *file, char *jsonStr, int type){

    SVG *newSVG = createSVG(file);

    if(type == 0){

        Circle * cir = JSONtoCircle(jsonStr);
        addComponent(newSVG, CIRC, (void*)cir);

    } else {

        Rectangle *rec = JSONtoRect(jsonStr);
        addComponent(newSVG, RECT, (void*)rec);

    }

    bool valid = writeSVG(newSVG, file);

    if(valid == true){

        char *msg = malloc(strlen("Success: Component has been added "));
        strcpy(msg, "Success: Component has been added");
        deleteSVG(newSVG);
        return msg;

    } 


    char *msg = malloc(strlen("Fail: Component is not valid "));
    strcpy(msg, "Fail: Component is not valid");
    deleteSVG(newSVG);

    return msg;

}

char *editAttr(char *file, char *name, char *value, int type, int index){

    SVG *newSVG = createSVG(file);
    char *message;

    Attribute *newAttribute = malloc(sizeof(Attribute) + strlen(value) + 1);
    newAttribute->name = malloc(strlen((char *)name) + 1);
    strcpy(newAttribute->name, name);
    strcpy(newAttribute->value, value);

    bool success = setAttribute(newSVG, type, index - 1, newAttribute);

    if(success != false){

        char *schema = "svg.xsd";
        bool valid = validateSVG(newSVG, schema);

        if(valid != false){

            writeSVG(newSVG, file);
            message = malloc(strlen("Success: Attribute has been implemented "));
            strcpy(message, "Success: Attribute has been implemented");
            return message;

        } 
        
        message = malloc(strlen("Error: Attribute is not valid "));
        strcpy(message, "Error: Attribute is not valid");
        return message;

    }

    message = malloc(strlen("Error: Attribute could not be set "));
    strcpy(message, "Error: Attribute could not be set");
    return message;

}

char *scaleShapes(char *file, int type, float scale){

    SVG *newSVG = createSVG(file);

    if(type == 1){

        List *circles = getCircles(newSVG);

        Circle *cir;
        void *elem;

        ListIterator itr = createIterator(circles);

        while((elem = nextElement(&itr)) != NULL){
            
            cir = (Circle*)elem;
            cir->r = cir->r * scale;
        }

        bool valid = validateSVG(newSVG, "svg.xsd");

        if(valid == true){

            writeSVG(newSVG, file);
            deleteSVG(newSVG);

            char *msg = malloc(strlen("Success: Circles scaled "));
            strcpy(msg, "Success: Circles scaled");
            return msg;
        }

        char *msg = malloc(strlen("Fail to validate circles "));
        strcpy(msg, "Fail to validate circles");
        return msg;


    } else {

        List *rectangles = getRects(newSVG);

        Rectangle *rec;
        void *elem;

        ListIterator itr = createIterator(rectangles);

        while((elem = nextElement(&itr)) != NULL){
            
            rec = (Rectangle*)elem;
            rec->width = rec->width * scale;
            rec->height = rec->height * scale;
        }

        bool valid = validateSVG(newSVG, "svg.xsd");

        if(valid == true){

            writeSVG(newSVG, file);
            deleteSVG(newSVG);

            char *msg = malloc(strlen("Success Rectangles scaled "));
            strcpy(msg, "Success: Rectangles Scaled");
            return msg;
        }

        char *msg = malloc(strlen("Fail to validate rectangles "));
        strcpy(msg, "Fail to validate rectangles");
        return msg;
    }

    deleteSVG(newSVG);
    char *msg = malloc(strlen("Fail "));
    strcpy(msg, "Fail");
    return msg;
}
