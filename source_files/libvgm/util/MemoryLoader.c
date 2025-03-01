#include <stdlib.h>
#include <string.h>

#include "DataLoader.h"
#include "MemoryLoader.h"

enum
{
	// mode: compression
	MLMODE_CMP_RAW = 0x00,
};

typedef struct _memory_loader MEMORY_LOADER;

typedef UINT32 (*MLOAD_READ)(MEMORY_LOADER *loader, UINT8 *buffer, UINT32 numBytes);

struct _memory_loader
{
	UINT8 modeCompr;
	UINT32 srcSize;	// size of source data
	const UINT8 *srcData;
	UINT32 decSize;	// decompressed size
	UINT32 pos;

	MLOAD_READ ReadData;
};


static UINT8 MemoryLoader_dopen(void *context);
static UINT32 MemoryLoader_dread(void *context, UINT8 *buffer, UINT32 numBytes);
static UINT32 MemoryLoader_ReadDataRaw(MEMORY_LOADER *loader, UINT8 *buffer, UINT32 numBytes);
static UINT32 MemoryLoader_dlength(void *context);
static INT32 MemoryLoader_dtell(void *context);
static UINT8 MemoryLoader_dseek(void *context, UINT32 offset, UINT8 whence);
static UINT8 MemoryLoader_dclose(void *context);
static UINT8 MemoryLoader_deof(void *context);


INLINE UINT32 ReadLE32(const UINT8 *data)
{
	return	(data[0x03] << 24) | (data[0x02] << 16) |
			(data[0x01] <<  8) | (data[0x00] <<  0);
}

static UINT8 MemoryLoader_dopen(void *context)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;

	loader->pos = 0;

	loader->modeCompr = MLMODE_CMP_RAW;
	loader->decSize = loader->srcSize;
	loader->ReadData = &MemoryLoader_ReadDataRaw;
	return 0x00;
}

static UINT32 MemoryLoader_dread(void *context, UINT8 *buffer, UINT32 numBytes)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;

	if(loader->pos >= loader->decSize) return 0;
	if(loader->pos + numBytes > loader->decSize)
		numBytes = loader->decSize - loader->pos;

	return loader->ReadData(loader, buffer, numBytes);
}

static UINT32 MemoryLoader_ReadDataRaw(MEMORY_LOADER *loader, UINT8 *buffer, UINT32 numBytes)
{
	memcpy(buffer, &loader->srcData[loader->pos], numBytes);
	loader->pos += numBytes;
	return numBytes;
}

static UINT32 MemoryLoader_dlength(void *context)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;
	return loader->decSize;
}

static INT32 MemoryLoader_dtell(void *context)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;
	return loader->pos;
}

static UINT8 MemoryLoader_dseek(void *context, UINT32 offset, UINT8 whence)
{
	(void)context;
	(void)offset;
	(void)whence;
	return 0; /* TODO */
}

static UINT8 MemoryLoader_dclose(void *context)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;
	return 0x00;
}

static UINT8 MemoryLoader_deof(void *context)
{
	MEMORY_LOADER *loader = (MEMORY_LOADER *)context;
	return loader->pos >= loader->decSize;
}

DATA_LOADER *MemoryLoader_Init(const UINT8 *buffer, UINT32 length)
{
	DATA_LOADER *dLoader;
	MEMORY_LOADER *mLoader;

	dLoader = (DATA_LOADER *)calloc(1, sizeof(DATA_LOADER));
	if(dLoader == NULL) return NULL;

	mLoader = (MEMORY_LOADER *)calloc(1, sizeof(MEMORY_LOADER));
	if(mLoader == NULL)
	{
		free(dLoader);
		return NULL;
	}

	mLoader->srcData = buffer;
	mLoader->srcSize = length;
	mLoader->decSize = 0;

	DataLoader_Setup(dLoader,&memoryLoader,mLoader);

	return dLoader;
}

const DATA_LOADER_CALLBACKS memoryLoader = {
	0x4D454D20,		// "MEM "
	"Memory Loader",
	MemoryLoader_dopen,
	MemoryLoader_dread,
	MemoryLoader_dseek,
	MemoryLoader_dclose,
	MemoryLoader_dtell,
	MemoryLoader_dlength,
	MemoryLoader_deof,
	NULL,
};
