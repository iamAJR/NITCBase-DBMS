#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>


BlockBuffer::BlockBuffer(int blockNum) {

this->blockNum=blockNum;
}

// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}


int BlockBuffer::getHeader(struct HeadInfo *head) {
 // unsigned char buffer[BLOCK_SIZE];
unsigned char *bufferPtr;
 int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }
  // read the block at this.blockNum into the buffer
//  Disk::readBlock(buffer, this->blockNum);

  // populate the numEntries, numAttrs and numSlots fields in *head
  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries, bufferPtr+16, 4);
  memcpy(&head->numAttrs,bufferPtr+20, 4);
  memcpy(&head->rblock, bufferPtr+12, 4);
  memcpy(&head->lblock, bufferPtr+8, 4);

  return SUCCESS;
}


int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  struct HeadInfo head;
 this->getHeader(&head);
 // unsigned char buffer[BLOCK_SIZE];
unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;
// Disk::readBlock(buffer, this->blockNum);

 int recordSize = attrCount * ATTR_SIZE;
    int slotMapSize = slotCount; // Assuming each slot is represented by 1 byte

    unsigned char *slotPointer = bufferPtr + HEADER_SIZE + slotMapSize + (recordSize * slotNum);
    memcpy(rec, slotPointer, recordSize);

return SUCCESS;


}


int RecBuffer::setRecord(union Attribute *rec, int slotNum)
{
  struct HeadInfo head;

  // get the header using this.getHeader() function
  this->getHeader(&head);

  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  // read the block at this.blockNum into a buffer
  unsigned char buffer[BLOCK_SIZE];
  Disk::readBlock(buffer, this->blockNum);

  /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
     - each record will have size attrCount * ATTR_SIZE
     - slotMap will be of size slotCount
  */
  int recordSize = attrCount * ATTR_SIZE;
  unsigned char *slotPointer = buffer + 32 + slotCount + recordSize * slotNum;

  // load the record into the rec data structure
  memcpy(slotPointer, rec, recordSize);
  Disk::writeBlock(buffer,this->blockNum);

  return SUCCESS;
}


int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
  // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
  int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

  if (bufferNum == E_BLOCKNOTINBUFFER) {
    bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

    if (bufferNum == E_OUTOFBOUND) {
      return E_OUTOFBOUND;
    }

    Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
  }

  // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
  *buffPtr = StaticBuffer::blocks[bufferNum];

  return SUCCESS;
}


/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

   struct HeadInfo head;
    this->getHeader(&head);

  int slotCount = head.numSlots;

  // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
  memcpy(slotMap,slotMapInBuffer,slotCount);
  return SUCCESS;
}


int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if(attrType==STRING)
	diff=strcmp(attr1.sVal,attr2.sVal);
    else
	diff=attr1.nVal-attr2.nVal;

    if(diff>0)  return 1;
    else if(diff<0) return -1;
    else  return 0;
}

