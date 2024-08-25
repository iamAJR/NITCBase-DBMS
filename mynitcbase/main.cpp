#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;
//sTAGE1
/*void printBMAP(const unsigned char* buffer, int bufferSize) {
    // Print values directly from the buffer
    for (int i = 0; i < bufferSize; ++i) {
        std::cout << "position- " << i << " value- " <<static_cast <int>(buffer[i]) << std::endl;
    }
}
stage 1 main function
//stage1
 // unsigned char buffer[BLOCK_SIZE];
 // Disk::readBlock(buffer,0);
   //   printBMAP(buffer, BLOCK_SIZE);

*/



/*stage 2


 RecBuffer relCatBuffer(RELCAT_BLOCK);
  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);
  HeadInfo relCatHeader;
  HeadInfo attrCatHeader;

 // Load the headers of both blocks into relCatHeader and attrCatHeader
 relCatBuffer.getHeader(&relCatHeader);
 attrCatBuffer.getHeader(&attrCatHeader);

 int totalrelations=relCatHeader.numEntries;
 int attributentry=attrCatHeader.numEntries;
 for (int i=0;i<totalrelations;i++) {

    Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

    relCatBuffer.getRecord(relCatRecord, i);

    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
    const char* currentRelationName = relCatRecord[RELCAT_REL_NAME_INDEX].sVal;
    int no_of_attri=relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
	int  count=0;

	while(count<no_of_attri){
    for (int j=0;j<attributentry;j++) {
      // declare attrCatRecord and load the attribute catalog entry into it
      Attribute attrCatRecord[ATTRCAT_NO_ATTRS]; 
      attrCatBuffer.getRecord(attrCatRecord, j);

      if (strcmp(currentRelationName,attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal)==0) {
        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
        printf("  %s: %s\n",attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal , attrType);
		count++;
      }
    }
	if(count<no_of_attri){
//	RecBuffer attrCatBuffer(attrCatHeader.rblock);
	attrCatBuffer=RecBuffer(attrCatHeader.rblock);
	 attrCatBuffer.getHeader(&attrCatHeader);
}
    printf("\n");}
  }
*/
/*void updateAttributeName (const char* relName, const char* oldAttrName, const char* newAttrName) {
	// used to hold reference to the block which referred to 
	// for getting records, headers and updating them
	RecBuffer attrCatBuffer (ATTRCAT_BLOCK);
	
	HeadInfo attrCatHeader;
	attrCatBuffer.getHeader(&attrCatHeader);

	// iterating the records in the Attribute Catalog
	// to find the correct entry of relation and attribute
	for (int recIndex = 0; recIndex < attrCatHeader.numEntries; recIndex++) {
		Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
		attrCatBuffer.getRecord(attrCatRecord, recIndex);

		// matching the relation name, and attribute name
		if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, relName) == 0
			&& strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, oldAttrName) == 0) 
		{
			strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newAttrName);
			attrCatBuffer.setRecord(attrCatRecord, recIndex);
			std::cout << "Update successful!\n\n";
			break;
		}

		// reaching at the end of the block, and thus loading
		// the next block and setting the attrCatHeader & recIndex
		if (recIndex == attrCatHeader.numSlots-1) {
			recIndex = -1;
			attrCatBuffer = RecBuffer (attrCatHeader.rblock);
			attrCatBuffer.getHeader(&attrCatHeader);
		}
	}

}

//stage 3
/*  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;
  RelCatEntry relCatEntry;
  AttrCatEntry attrCatEntry;

for (int i = 0; i <= 2;i++) {
    // Get the relation catalog entry for relation ID i
     RelCacheTable::getRelCatEntry(i, &relCatEntry);
    // Print the relation name
    printf("Relation: %s\n", relCatEntry.relName);

    // Loop over the attributes of the relation
    for (int j = 0; j < relCatEntry.numAttrs;j++) {
      // Get the attribute catalog entry for attribute offset j
     AttrCacheTable::getAttrCatEntry(i, j, &attrCatEntry);
     // Print the attribute name and type
	int val= attrCatEntry.attrType;
	if(val==1)
		 printf("  %s: STR\n", attrCatEntry.attrName);
       else 
      printf("  %s: NUM\n", attrCatEntry.attrName);
    }
  }

*/

int main(int argc, char *argv[]) {

//Disk disk_run;
//updateAttributeName ("Students", "Class", "Batch");

Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  return FrontendInterface::handleFrontend(argc, argv);  /* Initialize the Run Copy of Disk */
 // return 0;
 }


