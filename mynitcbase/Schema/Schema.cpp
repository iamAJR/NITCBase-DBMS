#include "Schema.h"

#include <cmath>
#include <cstring>


int Schema::openRel(char relName[ATTR_SIZE]) {
  int ret = OpenRelTable::openRel(relName);

  // the OpenRelTable::openRel() function returns the rel-id if successful
  // a valid rel-id will be within the range 0 <= relId < MAX_OPEN and any
  // error codes will be negative
  if(ret >= 0){
    return SUCCESS;
  }

  //otherwise it returns an error message
  return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE]) {
  if ((strcmp(relName,ATTRCAT_RELNAME)==0) || (strcmp(relName,RELCAT_RELNAME)==0) ){
    return E_NOTPERMITTED;
  }

  // this function returns the rel-id of a relation if it is open or
  // E_RELNOTOPEN if it is not. we will implement this later.
  int relId = OpenRelTable::getRelId(relName);

  if (relId<0) {
    return E_RELNOTOPEN;
  }

  return OpenRelTable::closeRel(relId);
}


int Schema::renameRel(char oldRelName[ATTR_SIZE], char newRelName[ATTR_SIZE]) {
   if ((strcmp(oldRelName,ATTRCAT_RELNAME)==0) || (strcmp(oldRelName,RELCAT_RELNAME)==0) ){
    return E_NOTPERMITTED;
  }
  if ((strcmp(newRelName,ATTRCAT_RELNAME)==0) || (strcmp(newRelName,RELCAT_RELNAME)==0) ){
    return E_NOTPERMITTED;
  }

    int relId=OpenRelTable::getRelId(oldRelName);
    if(relId!=E_RELNOTOPEN)
      return E_RELOPEN;
    int retVal = BlockAccess::renameRelation(oldRelName, newRelName);
    return retVal;
}


int Schema::renameAttr(char *relName, char *oldAttrName, char *newAttrName) {
   if ((strcmp(relName,ATTRCAT_RELNAME)==0) || (strcmp(relName,RELCAT_RELNAME)==0) ){
    return E_NOTPERMITTED;
  }
    int relId=OpenRelTable::getRelId(oldAttrName);
    if(relId!=E_RELNOTOPEN)
      return E_RELOPEN;
    
int c=BlockAccess::renameAttribute(relName,  oldAttrName,  newAttrName);
  return c;
}