#include "MemoryManager.h"

namespace MemoryManager
{
	void memoryCopy(void* _dst, void* _src, int _size);
	int skimMemory(int& _totalFree, int& _maxFree, int& _minFree);
	void createModDataCap(int _indexLocation, int _addressOfTail, bool _freeData, int _sizeOfData = 2);
	void removeDataCap(int _indexLocation);
	void* itterateMemory(bool _itterateByAddress, void* _address, unsigned short _sizeNeeded);


  const int MM_POOL_SIZE = 65536;
  char MM_pool[MM_POOL_SIZE];

  // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {
	  // TODO : IMPLEMENT ME

		//tags to ID for free and taken memory
		//free is 254
		//taken is 253

	  //get the last index of the array with size of the MM_POOL (its adaptible)
	  unsigned short dataTail = (sizeof(MM_pool) / sizeof(char))/*sizeOfArray*/ - (sizeof(char) * 1);

	  //create the orginal dataCap for the system to work
	  //it let the algorithm identify what blocks are availible
	  //and which ones are taken
	  createModDataCap(0, dataTail, true);
	  
  }

  // return a pointer inside the memory pool
  // If no chunk can accommodate aSize call onOutOfMemory()
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME
	  int freeMem = largestFree();
	  if (aSize > largestFree())
	  {
		  onOutOfMemory();
		  return nullptr;
	  }

    return itterateMemory(false, nullptr, aSize);
  }

  // Free up a chunk previously allocated
  void deallocate(void* aPointer)
  {
    // TODO: IMPLEMENT ME

	  //error check for if outside of scope of memory;
	  if ((int)aPointer < (int)&MM_pool[0] || (int)aPointer >(int)&MM_pool[MM_POOL_SIZE - 1])
		  onIllegalOperation((char*)aPointer);

	  itterateMemory(true, aPointer, 0);
  }

  //---
  //--- support routines
  //--- 

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void)
  {
    // TODO: IMPLEMENT ME
	  int freeMemory = 0;
	  int maxFreeMemory = -1;
	  int minFreeMemory = 66000;

	  //ask helper function to perform task (centralize work)
	  int errorCheck = skimMemory(freeMemory, maxFreeMemory, minFreeMemory);


	  //error check
	  if (errorCheck >= 0)
		  return freeMemory;
	  else
		return errorCheck;
  }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
    // TODO: IMPLEMENT ME

	  int freeMemory = 0;
	  int maxFreeMemory = -1;
	  int minFreeMemory = 66000;

	  //ask helper function to perform task (centralize work)
	  int errorCheck = skimMemory(freeMemory, maxFreeMemory, minFreeMemory);


	  //error check
	  if (errorCheck >= 0)
		  return maxFreeMemory;
	  else
		  return errorCheck;

  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
    // TODO: IMPLEMENT ME


	  int freeMemory = 0;
	  int maxFreeMemory = -1;
	  int minFreeMemory = 66000;

	  //ask helper function to perform task (centralize work)
	  int errorCheck = skimMemory(freeMemory, maxFreeMemory, minFreeMemory);


	  //error check
	  if (errorCheck >= 0)
		  return minFreeMemory;
	  else
		  return errorCheck;
	  
  }

  //---
  //--- helper functions
  //--- 

  /////////////////////////////////////////////////////
  // --To copy block of memory to another (Helper Func)
  //
  // I* _dst = destination to copy to
  // I* _src = where copying from
  // I* _size = how much memory to copy
  ////////////////////////////////////////////////////
  void memoryCopy(void* _dst, void* _src, int _size)
  {
	  for (int i = 0; i < _size; i++)
	  {
		  ((char*)_dst)[i] = ((char*)_src)[i];
	  }
  }

  ///////////////////////////////////////////////////////
  // --To seek through data and get stats (Helper Func)
  //
  // I* _totalFree = total amount of free data in alotment
  // I* _maxFree = the largest data block in set
  // I* _minFree = the smallest data block in set
  // O* int = for error checking
  /////////////////////////////////////////////////////////
  int skimMemory(int& _totalFree, int& _maxFree, int& _minFree)
  {
	  _totalFree = 0;
	  _maxFree = -1;
	  _minFree = 66000;

	  int itter = 0;
	  do
	  {
		  unsigned short memoryTail; //know where the end of the data block
		  int memoryHead = (itter + 3); //know where the start of data block
		  memoryCopy(&memoryTail, &MM_pool[itter + 1], 2); //get the address end of block
		  		  
		  switch ((unsigned char)(MM_pool[itter]))
		  {
		  case 253: //taken memory
			  break;
		  case 254: //free memory
		  {
			  int memoryBlock = memoryTail - memoryHead + 1;
			  _totalFree += memoryBlock;
			  if (memoryBlock > _maxFree)
				  _maxFree = memoryBlock;
			  if (memoryBlock < _minFree)
				  _minFree = memoryBlock;
		  }
			  break;
		  default:
			  return -1; //use for error check
		  }


		  //update the itter to calc new block
		  itter = memoryTail + 1;


	  } while (itter <= 65535/*max index*/);

	  return 1;// everything worked fine
  }

  /////////////////////////////////////////////////////
  // --To create or change the data bumper/ cap in front of the data block
  //  (Helper Func)
  // I* _indexLocation =  what index to perform change
  // I* _addressOfTail = pointer of last peice of data in block
  // I* _freeData = is the data block suppose to be availble or not to write to
  // I* _sizeOfData = how many bytes is the address of tail (default is 2)
  //////////////////////////////////////////////////////
  void createModDataCap(int _indexLocation, int _addressOfTail, bool _freeData, int _sizeOfData)
  {
	  //Set to takken memory by default
	  unsigned short freeOrNotID = 253; 

	  if (_freeData)
		  freeOrNotID = 254;
	  
	  MM_pool[_indexLocation] = freeOrNotID;
	  memoryCopy(&MM_pool[_indexLocation] + 1, &_addressOfTail, _sizeOfData);
	

  }

  ////////////////////////////////////////////////
  // --To remove the cap/bumper on an non-existant datablock
  //  (Helper Func)
  //
  // I* _indexLocation = where is location for clean up
  ////////////////////////////////////////////
  void removeDataCap(int _indexLocation)
  {
	  //hard release of these 3 bytes that create a data cap
	  MM_pool[_indexLocation] = MM_pool[_indexLocation + 1] = MM_pool[_indexLocation + 2] = 0;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // --To traverse through all blocks of data to assign and release sections
  // (Helper Func)
  //
  // I* _itterateByAddress = looking for an address for deallocation or for space for allocation
  // I* _address = the location to find the data block needed to be released
  // I* _sizeNeeded = the size to search for available block of said size
  //O* void* = to return address of new blocked assigned for use
  /////////////////////////////////////////////////////////////////////////////
  void* itterateMemory(bool _itterateByAddress, void* _address, unsigned short _sizeNeeded)
  {
	  void* returnAddress = nullptr;

	  int itter = 0;
	  unsigned short dataTail;

	  unsigned char prevDataBlockID;
	  unsigned short prevDataCapIndex;
	  unsigned short prevDataBlockTail;

	  unsigned char curDataBlockID;
	  unsigned short curDataCapIndex;
	  unsigned short curDataBlockTail;

	  unsigned char nextDataBlockID =0;
	  unsigned short nextDataCapIndex =0;
	  unsigned short nextDataBlockTail;


	  //set pre loop
	  prevDataBlockID = curDataBlockID = MM_pool[itter];
	  memoryCopy(&prevDataBlockTail,&MM_pool[itter+1],2);
	  prevDataCapIndex = itter;
	  memoryCopy(&curDataBlockTail, &MM_pool[itter + 1], 2);
	  curDataCapIndex = itter;


	  //loop to cycle through memory
	  do
	  {
		  bool tailAtEnd = true;

		  //get next next datacap
		  memoryCopy(&dataTail, &MM_pool[itter + 1], 2);
		  if (dataTail < MM_POOL_SIZE - 4)//if not at end/over of data allotment
		  {
			  nextDataCapIndex = dataTail + 1;
			  nextDataBlockID = MM_pool[nextDataCapIndex];
			  memoryCopy(&nextDataBlockTail, &MM_pool[nextDataCapIndex + 1], 2);
			  tailAtEnd = false;
		  }
		  

		  //-----------------check location to do work ---------


		  //Do a check for location of itter
		  if (_itterateByAddress)
		  {
			  //check to see if this is the allocation location/adddress
			  if (&MM_pool[itter + 3] == _address)
			  {
				  //data before and after the datablock are free
				  if (prevDataBlockID == 254/*FreeBlock*/ && nextDataBlockID == 254/*FreeBlock*/)
				  {
					  createModDataCap(prevDataCapIndex, nextDataBlockTail, true);
					  removeDataCap(curDataCapIndex);
					  removeDataCap(nextDataCapIndex);
					  break;
				  }
				  //data before the datablock is free
				  else if (prevDataBlockID == 254)
				  {
					  createModDataCap(prevDataCapIndex, curDataBlockTail, true);
					  removeDataCap(curDataCapIndex);
					  break;
				  }
				  //data after the datablock is free
				  else if (nextDataBlockID == 254/*FreeBlock*/)
				  {
					  createModDataCap(curDataCapIndex, nextDataBlockTail, true);
					  removeDataCap(nextDataCapIndex);
					  break;
				  }
				  //neither ahead or after datablock is free
				  else
				  {
					  createModDataCap(curDataCapIndex, curDataBlockTail, true);
					  break;
				  }
			  }
			  //error check if request to deallocate section that doesn't line up
			  else if ((int)&MM_pool[itter + 3] > (int)_address)
				  onIllegalOperation((char*)_address);
		  }
		  //find a free space for allocation
		  else if (curDataBlockID == 254/*FreeBlock*/)
		  {
			  int freeMemorySize = curDataBlockTail - (curDataCapIndex +3) + 1;
			  
			  //the free space has extra room to add another data block
			  if ((freeMemorySize - _sizeNeeded) >= 4)
			  {
				  createModDataCap(curDataCapIndex, _sizeNeeded + curDataCapIndex +2, false);
				  returnAddress = &MM_pool[curDataCapIndex + 3];//pass back the address
				  createModDataCap(curDataCapIndex + 3 + _sizeNeeded, curDataBlockTail, true);
				  break;
			  }
			  //the free space doesn't have enough extra room so just fill up to the extra 3 bytes
			  else if (freeMemorySize >= _sizeNeeded)
			  {
				  createModDataCap(curDataCapIndex, curDataBlockTail, false);
				  returnAddress =  &MM_pool[curDataCapIndex + 3];//pass back the address
				  break;
			  }
		  }


		 

			  

		  if (curDataBlockID == 254/*FreeBlock*/)
			  ;

		  //----------------Update data data -----------

		  //sets previous before changing current
		  prevDataBlockID = curDataBlockID;
		  memoryCopy(&prevDataBlockTail, &curDataBlockTail, 2);
		  prevDataCapIndex = curDataCapIndex;


		  //get next itter
		  itter = dataTail +1;
		  
		  //set current dataCap
		  curDataBlockID = nextDataBlockID;
		  curDataCapIndex = nextDataCapIndex;
		  if (!tailAtEnd)
			  memoryCopy(&curDataBlockTail, &MM_pool[itter + 1], 2);

		  
		 		  


	  } while (itter < MM_POOL_SIZE -4);

	  return returnAddress;
  }
 }



//Test code I used to test cetain theories
/* 

//the way I orgianly set up inside init
MM_pool[0] = 254; // Identifier for free memory
memoryCopy(&MM_pool[1], &dataTail, 2);

//to test memory for copying
int testValue = 65535;
void *testPointer = &testValue;
int testaddress = (int)testPointer;
char t1 = ((char*)testPointer)[0];
char t2 = ((char*)testPointer)[1];
char t3 = ((char*)testPointer)[2];
char t4 = ((char*)testPointer)[3];


// to test allocation and deallocation
allocate(10);
void* dealTest01 = allocate(12);
void* dealTest03 = allocate(5);
void* dealTest02 = allocate(10);
allocate(4);
deallocate(dealTest01);
deallocate(&MM_pool[31]);
allocate(4);
allocate(1);

int space = freeRemaining();
allocate(1);



*/