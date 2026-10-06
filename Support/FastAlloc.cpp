#ifndef _H_FastAlloc
#include "FastAlloc.h"
#endif
// FOSS: 
// A memory allocator using Photoshop's Buffer Suites: I think the old plugin used standard library memset, but docs are docs
#include "PhotoshopSDK.h"
#include <stdexcept>

// Malloc
SpMemoryPtr FastAlloc::New( SpUlong size )
{
	PSBufferSuite1* sPSBuffer = NULL;

	SPErr error = sSPBasic->AcquireSuite(kPSBufferSuite, kPSBufferSuiteVersion1, (const void**)&sPSBuffer);

	if (error != kSPNoError || sPSBuffer == NULL) {
		return NULL;
	}

	Ptr photoshopHandle = sPSBuffer->New(NULL, size);
	error = sSPBasic->ReleaseSuite(kPSBufferSuite, kPSBufferSuiteVersion1);
	return (SpMemoryPtr)photoshopHandle;
}

// Malloc but with a memset that inits the range to 0
SpMemoryPtr FastAlloc::NewClear( SpUlong size )
{
	SpMemoryPtr newPtr = New(size); // so this one is easy
	std::memset(newPtr, 0, size);
	return newPtr;
}

// Free
void FastAlloc::Delete( SpMemoryPtr ptr )
{
	PSBufferSuite1* sPSBuffer = NULL;

	SPErr error = sSPBasic->AcquireSuite(kPSBufferSuite, kPSBufferSuiteVersion1, (const void**)&sPSBuffer);

	if (error != kSPNoError || sPSBuffer == NULL) {
		throw std::runtime_error("suite error!");
	}
	
	sPSBuffer->Dispose((Ptr*)ptr); // delete the ptr safely
	error = sSPBasic->ReleaseSuite(kPSBufferSuite, kPSBufferSuiteVersion1);
}

// Get size of a void*
SpUlong FastAlloc::GetSize(SpMemoryPtr ptr)
{
	PSBufferSuite1* sPSBuffer = NULL;

	SPErr error = sSPBasic->AcquireSuite(kPSBufferSuite, kPSBufferSuiteVersion1, (const void**)&sPSBuffer);

	if (error != kSPNoError || sPSBuffer == NULL) {
		throw std::runtime_error("suite error!");
	}

	SpUlong size = sPSBuffer->GetSize((Ptr)ptr);
	error = sSPBasic->ReleaseSuite(kPSBufferSuite, kPSBufferSuiteVersion1);
	return size;
}

// Change the size of a void*
SpMemoryPtr FastAlloc::Resize(SpMemoryPtr ptr, SpUlong newSize)
{
	PSBufferSuite1* sPSBuffer = NULL;
	
	SPErr error = sSPBasic->AcquireSuite(kPSBufferSuite, kPSBufferSuiteVersion1, (const void**)&sPSBuffer);

	if (error != kSPNoError || sPSBuffer == NULL) {
		throw std::runtime_error("suite error!");
	}
	
	// copy here
	SpUlong oldSize = GetSize(ptr);
	SpMemoryPtr ptrToReturn = New(newSize);
	std::memcpy(ptrToReturn, ptr, oldSize);
	Delete(ptr); // safely dispose of the original 
	return ptrToReturn;
}

// Memset (by default 0)
void FastAlloc::ClearNumBytes( SpMemoryPtr ptr, SpUlong numToClear, SpByte pattern = 0 )
{
	std::memset(ptr, pattern, numToClear);
}