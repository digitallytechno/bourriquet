/***************************************************************************
Bourriquet 
	digitally
  
***************************************************************************/
#ifndef HASH_TABLE_H
#define HASH_TABLE_H 1
#include <exec/types.h>

#include "SDI_compiler.h"

struct HashTable;
struct HashEntryHeader
	{
  		ULONG keyHash;
	};

struct HashEntry
	{
  		struct HashEntryHeader header;
  		void *key;
	};

struct HashTableOps
	{
  		// Crochets obligatoires. 
  		void *       (* allocTable)(struct HashTable *table, ULONG capacity, ULONG entrySize);
  		void         (* freeTable)(struct HashTable *table, void *ptr);
  		const void * (* getKey)(struct HashTable *table, const struct HashEntryHeader *entry);
  		ULONG        (* hashKey)(struct HashTable *table, const void *key);
  		BOOL         (* matchEntry)(struct HashTable *table, const struct HashEntryHeader *entry, const void *key);
  		void         (* moveEntry)(struct HashTable *table, const struct HashEntryHeader *from, struct HashEntryHeader *to);
  		void         (* clearEntry)(struct HashTable *table, struct HashEntryHeader *entry);
  		void         (* finalize)(struct HashTable *table);
  		// Crochets optionnels. Si c'est NULL, ils ne sont pas appelés.
  		BOOL         (* initEntry)(struct HashTable *table, const struct HashEntryHeader *entry, const void *key);
  		void         (* destroyEntry)(struct HashTable *table, const struct HashEntryHeader *entry);
	};

struct HashTable
	{
  		const struct HashTableOps *ops;   
  		void *data;                       
  		UWORD shift;                      
  		UBYTE maxAlphaFrac;               
  		UBYTE minAlphaFrac;              
  		ULONG entrySize;                 
  		ULONG entryCount;                 
  		ULONG removedCount;               
  		ULONG generation;                 
  		char *entryStore;                 
	};

enum HashTableOperator
	{
  		htoLookup = (1<<0),               // entrée de recherche
  		htoAdd    = (1<<1),               // ajoute une entrée
  		htoRemove = (1<<2),               // supprime l'entrée
  		htoNext   = (1<<3),               // continuer
  		htoStop   = (1<<4),               // stopper
	};

#define HASH_BITS                   32
#define HASH_GOLDEN_RATIO           0x9e3778b9UL
#define HASH_MIN_SIZE               16
#define HASH_SIZE_LIMIT             (1UL << 24)
#define HASH_TABLE_SIZE(table)      (1UL << (HASH_BITS - (table)->shift))
#define HASH_ENTRY_IS_LIVE(entry)   ((entry)->keyHash >= 2)
#define HASH_ENTRY_IS_FREE(entry)   ((entry)->keyHash == 0)
#define HASH_ENTRY_IS_BUSY(entry)   (!HASH_ENTRY_IS_FREE(entry))

/*** Fonctions publiques ***/
struct HashTable *HashTableNew(const struct HashTableOps *ops, void *data, ULONG entrySize, ULONG capacity);
void HashTableDestroy(struct HashTable *table);
BOOL HashTableInit(struct HashTable *table, const struct HashTableOps *ops, void *data, ULONG entrySize, ULONG capacity);
void HashTableSetAlphaBounds(struct HashTable *table, float maxAlpha, float minAlpha);
void HashTableCleanup(struct HashTable *table);
struct HashEntryHeader *HashTableOperate(struct HashTable *table, const void *key, enum HashTableOperator op);
void HashTableRawRemove(struct HashTable *table, struct HashEntryHeader *entry);
const struct HashTableOps *HashTableGetDefaultOps(void);
const struct HashTableOps *HashTableGetDefaultStringOps(void);
ULONG HashTableEnumerate(struct HashTable *table, enum HashTableOperator (* etor)(struct HashTable *table, struct HashEntryHeader *entry, ULONG number, void *arg), void *arg);
void *DefaultHashAllocTable(UNUSED struct HashTable *table, ULONG capacity, ULONG entrySize);
void DefaultHashFreeTable(UNUSED struct HashTable *table, void *ptr);
const void *DefaultHashGetKey(UNUSED struct HashTable *table, const struct HashEntryHeader *entry);
ULONG DefaultHashHashKey(UNUSED struct HashTable *table, const void *key);
BOOL DefaultHashMatchEntry(UNUSED struct HashTable *table, const struct HashEntryHeader *entry, const void *key);
void DefaultHashMoveEntry(struct HashTable *table, const struct HashEntryHeader *from, struct HashEntryHeader *to);
void DefaultHashClearEntry(struct HashTable *table, struct HashEntryHeader *entry);
void DefaultHashFinalize(UNUSED struct HashTable *table);
ULONG StringHashHashKey(struct HashTable *table, const void *key);
BOOL StringHashMatchEntry(UNUSED struct HashTable *table, const struct HashEntryHeader *entry, const void *key);
void StringHashClearEntry(struct HashTable *table, struct HashEntryHeader *entry);

#endif /* HASH_TABLE_H */

