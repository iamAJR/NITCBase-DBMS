#include "OpenRelTable.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

OpenRelTable::OpenRelTable() {
    // Initialize relCache and attrCache with nullptr
    for (int i = 0; i < MAX_OPEN; ++i) {
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
    }

    // Setting up Relation Cache entries
    setupRelCache(RELCAT_BLOCK, RELCAT_SLOTNUM_FOR_RELCAT, RELCAT_RELID);
    setupRelCache(RELCAT_BLOCK, RELCAT_SLOTNUM_FOR_ATTRCAT, ATTRCAT_RELID);
    setupRelCache(RELCAT_BLOCK, 2, 2); // Students relation

    // Setting up Attribute Cache entries
    setupAttrCache(ATTRCAT_BLOCK, 0, RELCAT_NO_ATTRS, RELCAT_RELID);
    setupAttrCache(ATTRCAT_BLOCK, RELCAT_NO_ATTRS, ATTRCAT_NO_ATTRS + RELCAT_NO_ATTRS, ATTRCAT_RELID);
    setupAttrCache(ATTRCAT_BLOCK, 12, 16, 2); // Students attributes
}

void OpenRelTable::setupRelCache(int block, int slot, int relId) {
    RecBuffer relCatBlock(block);
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    relCatBlock.getRecord(relCatRecord, slot);

    RelCacheEntry relCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
    relCacheEntry.recId.block = block;
    relCacheEntry.recId.slot = slot;

    RelCacheTable::relCache[relId] = new RelCacheEntry(relCacheEntry);
}

void OpenRelTable::setupAttrCache(int block, int startSlot, int endSlot, int relId) {
    RecBuffer attrCatBlock(block);
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

    AttrCacheEntry* head = new AttrCacheEntry;
    AttrCacheEntry* current = head;

    for (int i = startSlot; i < endSlot; ++i) {
        attrCatBlock.getRecord(attrCatRecord, i);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &current->attrCatEntry);
        current->recId.block = block;
        current->recId.slot = i;

        if (i < endSlot - 1) {
            current->next = new AttrCacheEntry;
            current = current->next;
        } else {
            current->next = nullptr;
        }
    }

    AttrCacheTable::attrCache[relId] = head;
}

/*
#include "OpenRelTable.h"
#include <cstdlib>

#include <cstring>


OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
  }

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)




  /**** setting up Relation Catalog relation in the Relation Cache Table****
  RecBuffer relCatBlockRel(RELCAT_BLOCK);

  Attribute relCatRecordRel[RELCAT_NO_ATTRS];
  relCatBlockRel.getRecord(relCatRecordRel, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntryRel;
  RelCacheTable::recordToRelCatEntry(relCatRecordRel, &relCacheEntryRel.relCatEntry);
  relCacheEntryRel.recId.block = RELCAT_BLOCK;
  relCacheEntryRel.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntryRel;



  /**** setting up Attribute Catalog relation in the Relation Cache Table ***

RecBuffer relCatBlockAttr(RELCAT_BLOCK);

    Attribute relCatRecordAttr[RELCAT_NO_ATTRS];

    relCatBlockAttr.getRecord(relCatRecordAttr,RELCAT_SLOTNUM_FOR_ATTRCAT);

    struct RelCacheEntry relCacheEntryAttr; //this is for the attribute relation

    RelCacheTable::recordToRelCatEntry(relCatRecordAttr,&relCacheEntryAttr.relCatEntry);

    relCacheEntryAttr.recId.block = RELCAT_BLOCK;
    relCacheEntryAttr.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

   RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));

    *(RelCacheTable::relCache[ATTRCAT_RELID]) = relCacheEntryAttr;



/*********Students into relation cache********

    //this is for the students relation

    //here we are using the RecBuffer of relCatRecordRel block

    relCatBlockRel.getRecord(relCatRecordRel,RELCAT_SLOTNUM_FOR_RELCAT+1);

    RelCacheTable::recordToRelCatEntry(relCatRecordRel,&relCacheEntryRel.relCatEntry);

    relCacheEntryRel.recId.block = RELCAT_BLOCK;
    relCacheEntryRel.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT+1;

    RelCacheTable::relCache[ATTRCAT_RELID+1] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));;
    *(RelCacheTable::relCache[ATTRCAT_RELID+1]) = relCacheEntryRel;



 /************ Setting up Attribute cache entries **********/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Attribute Cache Table***
  RecBuffer attrCatBlockRel(ATTRCAT_BLOCK);
  Attribute attrCatRecordRel[ATTRCAT_NO_ATTRS];
 
 struct  AttrCacheEntry* attrCacheEntryRel=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
struct AttrCacheEntry* attrCacheEntryRelTemp=attrCacheEntryRel;

 for(int i=0;i<RELCAT_NO_ATTRS;i++){
	attrCatBlockRel.getRecord(attrCatRecordRel,i);

        //this converts the record to a attr catalog entry which is an entity in the struct AttrCacheEntry
        AttrCacheTable::recordToAttrCatEntry(attrCatRecordRel,&(attrCacheEntryRelTemp->attrCatEntry));
    attrCacheEntryRelTemp->recId.block=ATTRCAT_BLOCK;
    attrCacheEntryRelTemp->recId.slot=i;
    

 if (i < RELCAT_NO_ATTRS - 1) {
        attrCacheEntryRelTemp->next = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        attrCacheEntryRelTemp = attrCacheEntryRelTemp->next;  // Move to the next entry
    } else {
        // If it's the last entry, set next to nullptr
        attrCacheEntryRelTemp->next = nullptr;
    }



}
  AttrCacheTable::attrCache[RELCAT_RELID] = attrCacheEntryRel;








  /**** setting up Attribute Catalog relation in the Attribute Cache Table **
 RecBuffer attrCatBlockAttr(ATTRCAT_BLOCK);

    Attribute attrCatRecordAttr[ATTRCAT_NO_ATTRS];

    struct AttrCacheEntry* attrCacheEntryAttr = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry)) ;
    struct AttrCacheEntry* attrCacheEntryAttrTemp = attrCacheEntryAttr;

    int startAttrIndex = RELCAT_NO_ATTRS,lastAttrIndex = RELCAT_NO_ATTRS + ATTRCAT_NO_ATTRS;

    for(int i = startAttrIndex;i <= lastAttrIndex;i++){

attrCatBlockAttr.getRecord(attrCatRecordAttr,i);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecordAttr,&(attrCacheEntryAttrTemp->attrCatEntry));       
 attrCacheEntryAttrTemp->recId.block = ATTRCAT_BLOCK;
        attrCacheEntryAttrTemp->recId.slot = i;

        if(i<lastAttrIndex) attrCacheEntryAttrTemp->next = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        else attrCacheEntryAttrTemp->next = nullptr;

        attrCacheEntryAttrTemp = attrCacheEntryAttrTemp->next;
    }

    AttrCacheTable::attrCache[ATTRCAT_RELID] = attrCacheEntryAttr;
  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately

  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]
}


/*************students into attribute cache*******/

  //  struct AttrCacheEntry* attrCacheEntryRel=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
   // struct AttrCacheEntry* attrCacheEntryRelTemp=attrCacheEntryRel;
  /*  for(int i=12;i<16;i++){

        attrCatBlockRel.getRecord(attrCatRecordRel,i);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecordRel,&(attrCacheEntryRelTemp->attrCatEntry));

        attrCacheEntryRelTemp->recId.block = ATTRCAT_BLOCK;
        attrCacheEntryRelTemp->recId.slot = i;

        if(i<15)attrCacheEntryRelTemp->next = new AttrCacheEntry;
        else attrCacheEntryRelTemp->next = nullptr;

        attrCacheEntryRelTemp = attrCacheEntryRelTemp->next;

    }
  AttrCacheTable::attrCache[ATTRCAT_RELID+1] = attrCacheEntryRel;

*/

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
   if (strcmp(relName, RELCAT_RELNAME) == 0) return RELCAT_RELID;
   if (strcmp(relName, ATTRCAT_RELNAME) == 0) return ATTRCAT_RELID;
   if(strcmp(relName,"Students")==0) return 2;
   return E_RELNOTOPEN;
}
void freeAttrCacheEntry(AttrCacheEntry* head){
    if(!head)return;
    freeAttrCacheEntry(head->next);
    delete head;
}

OpenRelTable::~OpenRelTable() {
  for(int relCacheIdx=0;relCacheIdx<MAX_OPEN;relCacheIdx++){
        if(RelCacheTable::relCache[relCacheIdx]!=nullptr) delete RelCacheTable::relCache[relCacheIdx];
        
    }

    // here we recursively free all allocated memory as it is of linked list type and we have to traverse the whole list to free the the list
    // using the freeAttrCacheEntry function
    for(int attrCacheIdx=0;attrCacheIdx<MAX_OPEN;attrCacheIdx++) freeAttrCacheEntry(AttrCacheTable::attrCache[attrCacheIdx]);

}


