#include "StaticBuffer.h"

//fields of  static buffer
unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer() {

  // initialise all blocks as free
  for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty=false;
    metainfo[bufferIndex].timeStamp=-1;
    metainfo[bufferIndex].free=-1;

  }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {

  for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++){
     if(metainfo[bufferIndex].free==false and metainfo[bufferIndex].dirty==true){
      Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
    }
  }


}

int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  int allocatedBuffer=-1;

  int maxTimeStamp=-1; 
 for(int i=0;i<BUFFER_CAPACITY;i++){
  if (!metaInfo[i].free) {
            metaInfo[i].timeStamp++;
        }

	if(metainfo[i].free){
	allocatedBuffer=i;
	break;
}}

if (allocatedBuffer == -1) {
        for (int i = 0; i < BUFFER_CAPACITY; i++) {
            if (metaInfo[i].timeStamp > maxTimeStamp) {
                maxTimeStamp = metaInfo[i].timeStamp;
                allocatedBuffer = i;
            }
        }
        if (metaInfo[allocatedBuffer].dirty) {
            Disk::writeBlock(metaInfo[allocatedBuffer].blockNum, buffer[allocatedBuffer]);
        }
    }


    metainfo[allocatedBuffer].free = false;
  metainfo[allocatedBuffer].blockNum = blockNum;
  metainfo[allocatedBuffer].dirty=false;
  metainfo[allocatedBuffer].timeStamp=0;

  return allocatedBuffer;
}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
 if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }

  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
 for(int i=0;i<BUFFER_CAPACITY;i++){
if(metainfo[i].blockNum==blockNum)
	return i;
}
  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}


int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
        int index=getBufferNum(blockNum);

        if(index==E_BLOCKNOTINBUFFER)
          return E_BLOCKNOTINBUFFER;
        if(index==E_OUTOFBOUND)
          return E_OUTOFBOUND;
        else 
          {
            metainfo[index].dirty=true;
          }
}