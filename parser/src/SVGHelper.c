/*Name: Sunveer Khunkhun 
* Student No: 1064512
*/

#include "SVGHelper.h"

//funtion to parse a group
Group *parseGroup(xmlNode *node){

    Group *group = malloc(sizeof(Group)); //allocate memory for group

    //initialize all lists in group
    group->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    group->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    group->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    group->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    group->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);

    xmlNode *newNode = NULL;

    xmlAttr *attr = NULL;
    for (attr = node->properties; attr != NULL; attr = attr->next) { //loop through the attributes of the group and add them to list
    
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);

        Attribute *svgAtt = malloc(sizeof(Attribute) + sizeof(char) * strlen(cont) + 1);
        svgAtt->name = malloc(strlen((char *)attr->name) + 1);
        strcpy(svgAtt->name, (char *)attr->name);
        strcpy(svgAtt->value, cont);

        insertBack(group->otherAttributes, (void*)svgAtt);
    }

    //loop through the children of the group
    for (newNode = node->children; newNode != NULL; newNode = newNode->next) {

        if(newNode->type == XML_ELEMENT_NODE){

            //if group contains rect, path, circle, or group, then parse and add to list
            if(strcmp("rect", (const char*)newNode->name) == 0){ 

                Rectangle *rec = parseRect(newNode);
                insertBack(group->rectangles, (void*)rec);
            }

            if(strcmp("path", (const char*)newNode->name) == 0){

                Path *path = parsePath(newNode);
                insertBack(group->paths, (void*)path);

            } 
            
            if(strcmp("circle", (const char*)newNode->name) == 0){

                Circle *circle = parseCir(newNode);
                insertBack(group->circles, (void*)circle);

            } 
            
            if(strcmp("g", (const char*)newNode->name) == 0){
                
                Group *g = parseGroup(newNode);
                insertBack(group->groups, (void*)g);
                
            } 
        }
    }

    return group;

}

//function to parse a circle 
Circle *parseCir(xmlNode *node){

    Circle *cir = malloc(sizeof(Circle)); //allocate memory for circle struct
    cir->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); //initialize attributes list
    cir->cx = 0;
    cir->cy = 0;
    cir->r = 0;
    strcpy(cir->units, "");

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next) { //loop through attributes of circle 
    
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if(strcmp("cx", attrName) == 0){

            getUnits(cont, cir->units);
        
            float x = atof(cont);
            cir->cx = x;

        } else if(strcmp("cy", attrName) == 0){

            getUnits(cont, cir->units);
            
            float y = atof(cont);
            cir->cy = y;

        } else if(strcmp("r", attrName) == 0){
            
            getUnits(cont, cir->units);

            float r = atof(cont);
            cir->r = r;

        } else {
            
            //get other attributes of circle and add it to list 
            Attribute *cirAtt = malloc(sizeof(Attribute) + sizeof(char) * strlen(cont) + 1);
            cirAtt->name = malloc(strlen((char *)attr->name) + 1);
            strcpy(cirAtt->name, (char *)attr->name);
            strcpy(cirAtt->value, cont);
            insertBack(cir->otherAttributes, (void*)cirAtt);

        }

    }

    return cir;

}

//function to parse a rectangle
Rectangle *parseRect(xmlNode *node){

    Rectangle *rec = malloc(sizeof(Rectangle)); //allocate memory for rectangle
    rec->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); //initialize list of attributes
    rec->x = 0;
    rec->y = 0;
    rec->width = 0;
    rec->height = 0;
    strcpy(rec->units, "");

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next) { //loop through the rectangle attributes
    
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        //map correct values into rectangle struct
        if(strcmp("x", attrName) == 0){

            getUnits(cont, rec->units);

            float x = atof(cont);
            rec->x = x;

        } else if(strcmp("y", attrName) == 0){

            getUnits(cont, rec->units);

            float y = atof(cont);
            rec->y = y;

        } else if(strcmp("width", attrName) == 0){

            getUnits(cont, rec->units);

            float width = atof(cont);
            rec->width = width;

        } else if(strcmp("height", attrName) == 0){

            getUnits(cont, rec->units);

            float height = atof(cont);
            rec->height = height;

        } else {

            Attribute *rectAtt = malloc(sizeof(Attribute) + sizeof(char) * strlen(cont) + 1);
            rectAtt->name = malloc(strlen((char *)attr->name) + 1);
            strcpy(rectAtt->name, (char *)attr->name);
            strcpy(rectAtt->value, cont);
            insertBack(rec->otherAttributes, (void*)rectAtt);

        }


    }

    return rec;

}

//function to parse a path
Path *parsePath(xmlNode *node){

    Path *path;

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next) { //loop through attributes of path to get the length of the path
    
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if(strcmp("d", attrName) == 0){

            path = malloc(sizeof(Path) + sizeof(char) * strlen(cont) + 1); //allocate memory for path struct
            
        }
    }
    
    path->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); //initialize attributes
    strcpy(path->data, "");

    for (attr = node->properties; attr != NULL; attr = attr->next) {
    
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if(strcmp("d", attrName) == 0){

            strcpy(path->data,cont);
            

        } else {

            Attribute *pathAtt = malloc(sizeof(Attribute) * sizeof(char) + strlen(cont) + 1);
            pathAtt->name = malloc(strlen((char *)attr->name) + 1);
            strcpy(pathAtt->name, (char *)attr->name);
            strcpy(pathAtt->value, cont);
            insertBack(path->otherAttributes, (void*)pathAtt);

        }
    }

    return path;

}

//function to count the number of rectangles that have a given area
int getRect(List *list, float area){

    void* elem;
    int recCounter = 0; //initialize rectangle counter

    ListIterator itr = createIterator(list); //create a list iterator

    while ((elem = nextElement(&itr)) != NULL){ //loop through the rectangle list

        Rectangle *tmpRec = (Rectangle*)elem;

        float recArea = tmpRec->width * tmpRec->height; //calculate the area
        float result = ceil(recArea);

        if(result == ceil(area)){ //if the result is equal to the ceiling of the area, then add 1 to counter
            recCounter++;
        }

    }

    return recCounter;

}

//function to count number of rectangles that have a given area in a group
int getGroupRect(List *list, float area){

    void* elem2;
    int recCounter = 0;

    ListIterator gItr = createIterator(list);

    while ((elem2 = nextElement(&gItr)) != NULL){ //loop through list

        Group *tmpG = (Group*)elem2;

        if(getLength(tmpG->rectangles) > 0){ //if there is a rectangle list, then call getRect and add 1 to recCounter

            recCounter += getRect(tmpG->rectangles, area);


        }

        if(getLength(tmpG->groups) > 0){ //if there is a group list, then recursively call this function and add to recCounter

            recCounter += getGroupRect(tmpG->groups, area);
        }

    }

    return recCounter;

}

//function to count the number of circles that have a given area
int getCir(List *list, float area){

    void* elem;
    int cirCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Circle *tmpCir = (Circle*)elem;
 
        float cirArea = (tmpCir->r * tmpCir->r) * acos(-1); //calculate area of circle
        float result = ceil(cirArea);

        if(result == ceil(area)){
            cirCounter++;
        }

    }

    return cirCounter;

}

//function to count the number of circles in a group that have a given area
int getGroupCir(List *list, float area){

    void* elem2;
    int cirCounter = 0;

    ListIterator gItr = createIterator(list);

    while ((elem2 = nextElement(&gItr)) != NULL){

        Group *tmpG = (Group*)elem2;

        if(getLength(tmpG->circles) > 0){ //if there are a list of circles, then call getCir and increment counter

            cirCounter += getCir(tmpG->circles, area);


        }

        if(getLength(tmpG->groups) > 0){ //if there are a list of groups, then do a recursive call and add to counter

            cirCounter += getGroupCir(tmpG->groups, area);
        }

    }

    return cirCounter;

}

//function to count the number of paths given a path string
int getPath(List *list, const char *data){

    void* elem;
    int pathCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Path *tmpPath = (Path*)elem;
 
        if(strcmp(tmpPath->data, data) == 0){ //if the path matches the data, then increment counter

            pathCounter++;
        }

    }

    return pathCounter;

}

//function to count the number of paths in a group given a path string 
int getGroupPath(List *list, const char *data){ 

    void* elem2;
    int pathCounter = 0;

    ListIterator gItr = createIterator(list);

    while ((elem2 = nextElement(&gItr)) != NULL){

        Group *tmpG = (Group*)elem2;

        if(getLength(tmpG->paths) > 0){ //if there is a list of paths, then call getPath and increment the counter

            pathCounter += getPath(tmpG->paths, data);


        }

        if(getLength(tmpG->groups) > 0){ //if there are a list of group, then do a recursive call and add to the counter

            pathCounter += getGroupPath(tmpG->groups, data);
        }

    }

    return pathCounter;

}

//function to count the number of group with the given length
int getGroup(List *list, int len){

    void* elem;
    int result = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpG = (Group*)elem;

        int groupCounter = 0;

        //get the length of each group and add to the counter
        groupCounter += getLength(tmpG->rectangles);
        groupCounter += getLength(tmpG->circles);
        groupCounter += getLength(tmpG->paths);
        groupCounter += getLength(tmpG->groups);

        if(len == groupCounter){ //if the given length matches the counter, then increment the result

            result++;

        }

        if(getLength(tmpG->groups) > 0){ //if there is a list of groups, then call recursively and to result

            result += getGroup(tmpG->groups, len);

        }

    }

    return result;

}

//function to get the length of the path attributes
int pathAtt(List *list){

    void* elem;
    int attCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Path *tmpPath = (Path*)elem;

        attCounter += getLength(tmpPath->otherAttributes);

    }

    return attCounter;

}

//function to get the length rectangle attributes
int rectAtt(List *list){

    void* elem;
    int attCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Rectangle *tmpRec = (Rectangle*)elem;

        attCounter += getLength(tmpRec->otherAttributes);

    }

    return attCounter;

}

//function to get the length of the circle attributes
int cirAtt(List *list){

    void* elem;
    int attCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Circle *tmpCir = (Circle*)elem;

        attCounter += getLength(tmpCir->otherAttributes);

    }

    return attCounter;

}

//function to get the length of group attributes
int groupAtt(List *list){

    void* elem;
    int attCounter = 0;

    ListIterator itr = createIterator(list);

    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpG = (Group*)elem;

        //get length of all attribute lists in group structure
        attCounter += getLength(tmpG->otherAttributes);
        attCounter += pathAtt(tmpG->paths);
        attCounter += rectAtt(tmpG->rectangles);
        attCounter += cirAtt(tmpG->circles);

        if(getLength(tmpG->groups) > 0){

            attCounter += groupAtt(tmpG->groups);

        }


    }

    return attCounter;

}

//function to add a circle struct to a list
void circleList(List *list, List *circles){

    void* elem;

    ListIterator itr = createIterator(circles); //iterate through the list of circles

    while ((elem = nextElement(&itr)) != NULL){

        Circle *tmpCir = (Circle*)elem;

        insertBack(list, (void*)tmpCir); //add to list

    }

}

//function to get a list of circles given a group structure
void groupToCircleList(List *list, List *group){

    ListIterator itr = createIterator(group);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ //iterate through the list of groups

        Group *tmpG = (Group*)elem;

        if(getLength(tmpG->circles) > 0){ //if there are a list of circles, then call circleList and add to the list

            circleList(list, tmpG->circles);

        }

        if(getLength(tmpG->groups) > 0){ //if there are a list of groups, then call recursively 

            groupToCircleList(list, tmpG->groups);
        }

    }

}

//function to add a rectangle struct to a list
void rectList(List *list, List *rectangles){

    void* elem;

    ListIterator itr = createIterator(rectangles);

    while ((elem = nextElement(&itr)) != NULL){

        Rectangle *tmpRec = (Rectangle*)elem;

        insertBack(list, (void*)tmpRec);

    }

}

//function to get a list of rectangles given a group structure
void groupToRectList(List *list, List *group){

    ListIterator itr = createIterator(group);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpG = (Group*)elem;

        if(getLength(tmpG->rectangles) > 0){ //if there are a list of rectangles, then call rectList and add to the list

            rectList(list, tmpG->rectangles);

        }

        if(getLength(tmpG->groups) > 0){

            groupToRectList(list, tmpG->groups);
        }

    }

}

//function to get a list of paths
void pathList(List *list, List *path){

    void* elem;

    ListIterator itr = createIterator(path);

    while ((elem = nextElement(&itr)) != NULL){

        Path *tmpRec = (Path*)elem;

        insertBack(list, (void*)tmpRec);

    }

}

//function to get a list of paths given a group structure
void groupToPathList(List *list, List *group){

    ListIterator itr = createIterator(group);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpG = (Group*)elem;

        if(getLength(tmpG->paths) > 0){ //if there are a list of paths, then call pathList and add to the list

            pathList(list, tmpG->paths);

        }

        if(getLength(tmpG->groups) > 0){

            groupToPathList(list, tmpG->groups);
        }

    }

}

//function to get a list of groups
void groupList(List *list, List *group){

    ListIterator itr = createIterator(group);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ //iterate through the group list

        Group *tmpG = (Group*)elem;

        insertBack(list, (void*)tmpG); //insert the group to the list

        if(getLength(tmpG->groups) > 0){ //if there are a list of groups, then call recursively 

            groupList(list, tmpG->groups);
        }

    }

}

//dummy function to free weak references
void dummyFree() {
    
}

//function to get the units of circles and rectangles
void getUnits(char *cont, char units[50]){

    char *newUnits;
    strtof(cont, &newUnits);
    strcpy(units, newUnits);
}

//A2 Functions ----------------------------------------------------------------------------------------------------------

//function to create an xmlDoc given a SVG struct
xmlDocPtr SVGtoXMLTree(const SVG *img){

    if(img == NULL || img->otherAttributes == NULL){

        return NULL;
    }

    xmlDocPtr doc = NULL; 
    xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg"); //set the root node
    xmlDocSetRootElement(doc, root_node);

    if(strcmp(img->namespace, "") == 0){ //if no namespace, then return NULL

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }

    xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST img->namespace, BAD_CAST NULL); //set the namespace
    xmlSetNs(root_node, ns);

    if(SVGtoXMLTreeAttributes(root_node, img->otherAttributes) == false){ //set the attributes for the root node

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }

    //set the title
    node = xmlNewNode(ns, BAD_CAST "title"); 
    node1 = xmlNewText(BAD_CAST img->title);
    xmlAddChild(node, node1);
    xmlAddChild(root_node, node);

    //set the description
    node = xmlNewNode(ns, BAD_CAST "desc");
    node1 = xmlNewText(BAD_CAST img->description);
    xmlAddChild(node, node1);
    xmlAddChild(root_node, node);

    //add all of the lists
    if(SVGtoXMLTreeRects(root_node, img->rectangles) == false || SVGtoXMLTreeCirc(root_node, img->circles) == false ||  SVGtoXMLTreePath(root_node, img->paths) == false || SVGtoXMLTreeGroup(root_node, img->groups) == false){
         
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }

    return doc;

}

//function to convert SVG attributes to a tree
bool SVGtoXMLTreeAttributes(xmlNodePtr node, List *otherAttributes){

    if(otherAttributes == NULL){

        return false;
    }

    ListIterator itr = createIterator(otherAttributes);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ 

        Attribute *tmpAtt = (Attribute*)elem;

        if(tmpAtt->name == NULL){ //if attribute name is NULL, then return false

            return false;
        }

        xmlNewProp(node, BAD_CAST tmpAtt->name, BAD_CAST tmpAtt->value); //add the attribute to the tree

    }

    return true;
}

//function to parse a list of rectangles to a tree
bool SVGtoXMLTreeRects(xmlNodePtr root_node, List *rectangles){

    if(rectangles == NULL){

        return false;
    }

    ListIterator itr = createIterator(rectangles);
    char tmpStr[100];

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ 

        Rectangle *tmpRec = (Rectangle*)elem;

        if(tmpRec->width < 0 || tmpRec->height < 0 || tmpRec->otherAttributes == NULL){
            return false;
        }

        xmlNodePtr node;
        node = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL); //add a child node for rectangles

        sprintf(tmpStr, "%f", tmpRec->x);
        strcat(tmpStr, tmpRec->units);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST tmpStr); //add x value and units

        sprintf(tmpStr, "%f", tmpRec->y);
        strcat(tmpStr, tmpRec->units);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST tmpStr); //add y value and units

        sprintf(tmpStr, "%f", tmpRec->width);
        strcat(tmpStr, tmpRec->units);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST tmpStr); //add width value and units

        sprintf(tmpStr, "%f", tmpRec->height);
        strcat(tmpStr, tmpRec->units);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST tmpStr); //add height value and units

        bool valid = SVGtoXMLTreeAttributes(node, tmpRec->otherAttributes); //set the attributes

        if(valid == false){

            return false;
        }
        
    }

    return true;

}

//function to parse a list of circles to a tree
bool SVGtoXMLTreeCirc(xmlNodePtr root_node, List *circles){

    if(circles == NULL){

        return false;
    }

    ListIterator itr = createIterator(circles);
    char tmpStr[1000];

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ 

        Circle *tmpC = (Circle*)elem;

        if(tmpC->r < 0 || tmpC->otherAttributes == NULL){

            return false;
        }

        xmlNodePtr node;
        node = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL); //add a new circle child

        sprintf(tmpStr, "%f", tmpC->cx);
        strcat(tmpStr, tmpC->units);
        xmlNewProp(node, BAD_CAST "cx", BAD_CAST tmpStr); //add cx value and units

        sprintf(tmpStr, "%f", tmpC->cy);
        strcat(tmpStr, tmpC->units);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST tmpStr); //add cy and units

        sprintf(tmpStr, "%f", tmpC->r);
        strcat(tmpStr, tmpC->units);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST tmpStr); //add r value and units

        bool valid = SVGtoXMLTreeAttributes(node, tmpC->otherAttributes); //get attributes of circles

        if(valid == false){

            return false;
        }
        
    }

    return true;

}

//function to parse a path to a tree
bool SVGtoXMLTreePath(xmlNodePtr root_node, List *path){

    if(path == NULL || root_node == NULL){

        return false;
    }

    ListIterator itr = createIterator(path);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ 

        Path *tmpP = (Path*)elem;

        if(tmpP->data == NULL || tmpP == NULL || tmpP->otherAttributes == NULL){ 
            
            return false;
        }

        xmlNodePtr node;
        node = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL); //add path child

        xmlNewProp(node, BAD_CAST "d", BAD_CAST tmpP->data); //add path value

        bool valid = SVGtoXMLTreeAttributes(node, tmpP->otherAttributes); //add path attributes

        if(valid == false){

            return false;
        }
        
    }

    return true;

}

//function to parse a group to a tree
bool SVGtoXMLTreeGroup(xmlNodePtr root_node, List *group){

    if(group == NULL){

        return false;
    }

    ListIterator itr = createIterator(group);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ 

        Group *tmpG = (Group*)elem;

        if(tmpG->otherAttributes == NULL){

            return false;
        }

        xmlNodePtr node;
        node = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL); //add group child

        if(SVGtoXMLTreeAttributes(node, tmpG->otherAttributes) == false){ //add group attributes
            
            return false;
        }
        
        //add rectangle, circle, path, and group lists 
        if(SVGtoXMLTreeRects(node, tmpG->rectangles) == false || SVGtoXMLTreeCirc(node, tmpG->circles) == false || SVGtoXMLTreePath(node, tmpG->paths) == false ||  SVGtoXMLTreeGroup(node, tmpG->groups) == false){

            return false;
        }
        
    }

    return true;

}

//function to validate an xmlDoc
bool validXMLDoc(xmlDocPtr doc, const char *schemaFile){

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    if (doc == NULL || schemaFile == NULL) { //if xmldoc is null then return null

        //printf("error: could not parse file\n");
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return false;

    } else {

        ctxt = xmlSchemaNewParserCtxt(schemaFile);

        xmlLineNumbersDefault(1);

        xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        schema = xmlSchemaParse(ctxt);
        xmlSchemaFreeParserCtxt(ctxt);

        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);

        if(ret == 0){

            //printf("Tree is valid\n");

        } else if(ret > 0){

            //printf("Tree not valid\n");

            xmlSchemaFreeValidCtxt(ctxt); 
            xmlFreeDoc(doc);
            
            if(schema != NULL) {
                xmlSchemaFree(schema);
            }

            xmlSchemaCleanupTypes();
            xmlMemoryDump();
            xmlCleanupParser();

            return false;

        } else {

            //printf("validation generated an error\n");

            xmlSchemaFreeValidCtxt(ctxt); 
            xmlFreeDoc(doc);
            if(schema != NULL) {
                xmlSchemaFree(schema);
            }

            xmlSchemaCleanupTypes();
            xmlMemoryDump();
            xmlCleanupParser();

            return false;

        }

        xmlSchemaFreeValidCtxt(ctxt);  

    }

    if(schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlMemoryDump();

    return true;
}

//Module 2 -------------------------------------

//function to add or change an attribute 
bool getOtherAttributes(List *otherAttributes, Attribute*newAttribute){ // MIGHT NEED TO REALLOCATE??

    if(otherAttributes == NULL || newAttribute == NULL){

        return false;
    }

    if(getLength(otherAttributes) == 0){ //add the attribute if list is empty

        insertBack(otherAttributes, (void*)newAttribute);
        return true;
    }

    bool found = false;

    ListIterator itr = createIterator(otherAttributes);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){ //iterate through the list

        Attribute *tmpA = (Attribute*)elem;

        if(strcmp((const char*)tmpA->name, (const char*)newAttribute->name) == 0){ //if name matches, then change the old value to the new

            found = true; //set find to true
            strcpy(tmpA->value, (const char*)newAttribute->value);
            
            deleteAttribute((void*)newAttribute); //delete the attribute
            return true;

        } 

    }

    if(found == false){ //if not found, then append attribute

        insertBack(otherAttributes, (void*)newAttribute);
        return true;
    }

    return false;

}

//function to get the total number of each list in the SVG struct
void numLists(List *groups, int *rNum, int *cNum, int *pNum, int *gNum){

    if(groups == NULL){

        return;
    }

    ListIterator itr = createIterator(groups);

    void* elem;

    while ((elem = nextElement(&itr)) != NULL){

        Group *tmpG = (Group*)elem;

        //get the length of each list
        *rNum += getLength(tmpG->rectangles);
        *cNum += getLength(tmpG->circles);
        *pNum += getLength(tmpG->paths);
        *gNum += getLength(tmpG->groups);

        numLists(tmpG->groups, rNum, cNum, pNum, gNum); //get number of lists for the current group

    }

}

//A3 Javascript glue --------------------------------------------------------------------------------
