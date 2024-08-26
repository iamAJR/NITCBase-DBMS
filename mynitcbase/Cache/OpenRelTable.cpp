#include "OpenRelTable.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
AttrCacheEntry *createAttrCacheEntryList(int size) {
  AttrCacheEntry *head = nullptr, *curr = nullptr;
  head = curr = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
  size--;
  while (size--) {
    curr->next = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    curr = curr->next;
  }
  curr->next = nullptr;
  return head;
}

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {
    // Initialize relCache and attrCache with nullptr
    for (int i = 0; i < MAX_OPEN; ++i) {
        tableMetaInfo[i].free = true;
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

// table meta info stores tableMetaInfo is used to store which entries of the caches are free 
//and the relation to which an occupied entry belongs.
    tableMetaInfo[RELCAT_RELID].free = false;
    tableMetaInfo[ATTRCAT_RELID].free = false;
    strcpy(tableMetaInfo[RELCAT_RELID].relName, "RELATIONCAT");
    strcpy(tableMetaInfo[ATTRCAT_RELID].relName, "ATTRIBUTECAT");
}





OpenRelTable::~OpenRelTable() {

  // Close all open relations starting from rel-id = 2 onwards
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // Close the relation
    }
  }

  // Free the memory allocated for rel-id 0 and 1 in the caches

  free(RelCacheTable::relCache[0]);

 AttrCacheEntry *head = AttrCacheTable::attrCache[1];
  AttrCacheEntry *next = head;
  while(head){
    next=head->next;
    free(head);
    head=next;
  }
 
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



int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
  for (int i = 0; i < MAX_OPEN; i++) {
    if (strcmp(relName, tableMetaInfo[i].relName) == 0) {
   // and tableMetaInfo[i].free == false) {
      return i;
    }
  }
  return E_RELNOTOPEN;
}

int OpenRelTable::getFreeOpenRelTableEntry() {

  for (int i = 0; i < MAX_OPEN; i++) {
    if (tableMetaInfo[i].free) {
      return i;
    }
  }
  return E_CACHEFULL;
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) {

//   std::cout << "Free slots:" << std::endl;
// for (int i = 0; i < MAX_OPEN; ++i) {
//     std::cout << "Slot " << i << ": " << (tableMetaInfo[i].free ? "Free" : "Occupied") << std::endl;
// }

  //  int relId = getRelId(relName);
  // 	if(relId >= 0){
  //   	// return that relation id;
	// 	return relId;
  // 	}

  int rel=OpenRelTable::getRelId(relName);
  if(rel>-1 and rel<12)
    return rel;

  int ca=getFreeOpenRelTableEntry();
  if(ca==E_CACHEFULL )  
    return E_CACHEFULL;
  
  // let relId be used to store the free slot.
  //int relId;

  /****** Setting up Relation Cache entry for the relation ******/
// Reset search index and perform linear search



  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

	RelCacheTable::resetSearchIndex(RELCAT_RELID);
  Attribute attrVal;
  strcpy(attrVal.sVal,relName);
  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  //EQ means equal to checks if the reln names are equal
  RecId relcatRecId=BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME,attrVal,EQ);
  
  if (relcatRecId.block==-1 and relcatRecId.slot==-1) {
		//! the relation is not found in the Relation Catalog
		return E_RELNOTEXIST;
	}
  // read the record entry corresponding to relcatRecId and create a relCacheEntry
    //  on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
     
   // NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  RecBuffer relationBuffer (relcatRecId.block);
	Attribute relationRecord [RELCAT_NO_ATTRS];
  relationBuffer.getRecord(relationRecord,relcatRecId.slot);
	RelCacheEntry *relCacheBuffer = nullptr;
  relCacheBuffer = (RelCacheEntry*) malloc (sizeof(RelCacheEntry));
  RelCacheTable::recordToRelCatEntry(relationRecord,&(relCacheBuffer->relCatEntry));


// update the recId field of this Relation Cache entry to relcatRecId.
//use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
  relCacheBuffer->recId.block = relcatRecId.block;

	relCacheBuffer->recId.slot = relcatRecId.slot;
	RelCacheTable::relCache[ca] = relCacheBuffer;	




  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  AttrCacheEntry* listHead=nullptr;
  AttrCacheEntry* head=listHead;
  	int numberOfAttributes = RelCacheTable::relCache[ca]->relCatEntry.numAttrs;

  head = createAttrCacheEntryList(numberOfAttributes);
	listHead= head;

	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

  for (int attr = 0; attr < numberOfAttributes; attr++)
	{
		// AttrCacheTable::resetSearchIndex(relId, attr);
		RecId attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, RELCAT_ATTR_RELNAME, attrVal, EQ);

		RecBuffer attrCatBlock(attrcatRecId.block);
		attrCatBlock.getRecord(attrCatRecord, attrcatRecId.slot);

		AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&(listHead->attrCatEntry));

		listHead->recId.block = attrcatRecId.block;
		listHead->recId.slot = attrcatRecId.slot;

		listHead = listHead->next;
	}
  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
 // {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
    //  RecId attrcatRecId;

      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  //}

  // set the relIdth entry of the AttrCacheTable to listHead.
	AttrCacheTable::attrCache[ca] = head;

  /****** Setting up metadata in the Open Relation Table for the relation******/
  tableMetaInfo[ca].free = false;
	strcpy(tableMetaInfo[ca].relName, relName);  

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.

  return ca;
}

int OpenRelTable::closeRel(int relId) {
  if (relId==0 or relId==1) {
    return E_NOTPERMITTED;
  }

  if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }
  if (tableMetaInfo[relId].free) {
    return E_RELNOTOPEN;
  }

  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function
  free(RelCacheTable::relCache[relId]);
  AttrCacheEntry *head = AttrCacheTable::attrCache[relId];
  AttrCacheEntry *next = head;
  while(head){
    next=head->next;
    free(head);
    head=next;
  }
  // while (next) {
  //   free(head);
  //   head = next;
  //   next = next->next;
  // }
  // free(head);


  // update `tableMetaInfo` to set `relId` as a free slot
  tableMetaInfo[relId].free=true;
  
  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
    RelCacheTable::relCache[relId] = nullptr;
    AttrCacheTable::attrCache[relId] = nullptr;

  return SUCCESS;
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

/*int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
   if (strcmp(relName, RELCAT_RELNAME) == 0) return RELCAT_RELID;
   if (strcmp(relName, ATTRCAT_REL NAME) == 0) return ATTRCAT_RELID;
   if(strcmp(relName,"Students")==0) return 2;
   return E_RELNOTOPEN;
}*/
/*void freeAttrCacheEntry(AttrCacheEntry* head){
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

}*/ 


