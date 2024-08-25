#include "AttrCacheTable.h"

#include <cstring>
AttrCacheEntry* AttrCacheTable::attrCache[MAX_OPEN];

/* returns the attrOffset-th attribute for the relation corresponding to relId
NOTE: this function expects the caller to allocate memory for `*attrCatBuf`
*/
int AttrCacheTable::getAttrCatEntry(int relId, int attrOffset, AttrCatEntry* attrCatBuf) {
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise
   if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  // check if attrCache[relId] == nullptr and return E_RELNOTOPEN if true
  if (attrCache[relId] == nullptr) {
    return E_RELNOTOPEN;
  }
  // traverse the linked list of attribute cache entries
  for (AttrCacheEntry* entry = attrCache[relId]; entry != nullptr; entry = entry->next) {
    if (entry->attrCatEntry.offset == attrOffset) {
	      *attrCatBuf = entry->attrCatEntry;
		return SUCCESS;
      // copy entry->attrCatEntry to *attrCatBuf and return SUCCESS;
    }
  }

  // there is no attribute at this offset
  return E_ATTRNOTEXIST;
}

/* Converts a attribute catalog record to AttrCatEntry struct
    We get the record as Attribute[] from the BlockBuffer.getRecord() function.
    This function will convert that to a struct AttrCatEntry type.
*/
void AttrCacheTable::recordToAttrCatEntry(union Attribute record[ATTRCAT_NO_ATTRS],AttrCatEntry* attrCatEntry){
    strcpy(attrCatEntry->relName,record[ATTRCAT_REL_NAME_INDEX].sVal);
    strcpy(attrCatEntry->attrName ,record[ATTRCAT_ATTR_NAME_INDEX].sVal);
    attrCatEntry->attrType= (int)record[ATTRCAT_ATTR_TYPE_INDEX].nVal;
    attrCatEntry->primaryFlag = (bool)record[ATTRCAT_PRIMARY_FLAG_INDEX].nVal;
    attrCatEntry->rootBlock = (int)record[ATTRCAT_ROOT_BLOCK_INDEX].nVal;
    attrCatEntry->offset = (int) record[ATTRCAT_OFFSET_INDEX].nVal;
    
}


// here we overload getattrcatentry  to get attribute with particualr  name
int AttrCacheTable::getAttrCatEntry(int relId, char attrName[ATTR_SIZE], AttrCatEntry* attrCatBuf) {

  if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  // check if attrCache[relId] == nullptr and return E_RELNOTOPEN if true
  if (attrCache[relId] == nullptr) {
    return E_RELNOTOPEN;
  }
  for (AttrCacheEntry* start = attrCache[relId]; start != nullptr; start = start->next) {
    if (strcmp(start->attrCatEntry.attrName, attrName) == 0) {
        strcpy(attrCatBuf->attrName, start->attrCatEntry.attrName);
        attrCatBuf->attrType = start->attrCatEntry.attrType;
        attrCatBuf->offset = start->attrCatEntry.offset;
        attrCatBuf->primaryFlag = start->attrCatEntry.primaryFlag;
        strcpy(attrCatBuf->relName, start->attrCatEntry.relName);
        attrCatBuf->rootBlock = start->attrCatEntry.rootBlock;
        return SUCCESS;
    }
}

 /*  AttrCacheEntry *ptr = attrCache[relId];
    while (ptr != nullptr)
    {
        if (strcmp(ptr->attrCatEntry.attrName, attrName) == 0)
        {
            strcpy(attrCatBuf->attrName, ptr->attrCatEntry.attrName);
            attrCatBuf->attrType = ptr->attrCatEntry.attrType;
            attrCatBuf->offset = ptr->attrCatEntry.offset;
            attrCatBuf->primaryFlag = ptr->attrCatEntry.primaryFlag;
            strcpy(attrCatBuf->relName, ptr->attrCatEntry.relName);
            attrCatBuf->rootBlock = ptr->attrCatEntry.rootBlock;
            return SUCCESS;
        }
        ptr = ptr->next;
    }*/
  // no attribute with name attrName for the relation
  return E_ATTRNOTEXIST;
}
