/*
   AngelCode Scripting Library
   Copyright (c) 2003-2012 Andreas Jonsson

   This software is provided 'as-is', without any express or implied 
   warranty. In no event will the authors be held liable for any 
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any 
   purpose, including commercial applications, and to alter it and 
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you 
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product 
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and 
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source 
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas Jonsson
   andreas@angelcode.com
*/


//
// as_restore.h
//
// Functions for saving and restoring module bytecode
// asCRestore was originally written by Dennis Bollyn, dennis@gyrbo.be
// It was later split in two classes asCReader and asCWriter by me

#ifndef AS_RESTORE_H
#define AS_RESTORE_H

#include "as_scriptengine.h"
#include "as_context.h"
#include "as_map.h"

BEGIN_AS_NAMESPACE

class asCReader
{
public:
	asCReader(asCModule *module, asIBinaryStream *stream, asCScriptEngine *engine);

	int Read();

protected:
	asCModule       *module;
	asIBinaryStream *stream;
	asCScriptEngine *engine;
	bool             error;

	void               ReadData(void *data, asUINT size);
	void               ReadString(asCString *str);
	asCScriptFunction *ReadFunction(bool addToModule = true, bool addToEngine = true, bool addToGC = true);
	void               ReadFunctionSignature(asCScriptFunction *func);
	void               ReadGlobalProperty();
	void               ReadObjectProperty(asCObjectType *ot);
	void               ReadDataType(asCDataType *dt);
	asCObjectType *    ReadObjectType();
	void               ReadObjectTypeDeclaration(asCObjectType *ot, int phase);
	void               ReadByteCode(asDWORD *bc, int length);
	asUINT             ReadEncodedUInt();

	void ReadUsedTypeIds();
	void ReadUsedFunctions();
	void ReadUsedGlobalProps();
	void ReadUsedStringConstants();
	void ReadUsedObjectProps();

	asCObjectType *    FindObjectType(int idx);
	int                FindTypeId(int idx);
	short              FindObjectPropOffset(asWORD index);
	asCScriptFunction *FindFunction(int idx);

	// After loading, each function needs to be translated to update pointers, function ids, etc
	void TranslateFunction(asCScriptFunction *func);

	// Temporary storage for persisting variable data
	asCArray<int>                usedTypeIds;
	asCArray<asCObjectType*>     usedTypes;
	asCArray<asCScriptFunction*> usedFunctions;
	asCArray<void*>              usedGlobalProperties;
	asCArray<int>                usedStringConstants;

	asCArray<asCScriptFunction*>  savedFunctions;
	asCArray<asCDataType>         savedDataTypes;
	asCArray<asCString>           savedStrings;

	struct SObjProp
	{
		asCObjectType *objType;
		int            offset;
	};
	asCArray<SObjProp> usedObjectProperties;

	struct SObjChangeSize
	{
		asCObjectType *objType;
		asUINT         oldSize;
	};
	asCArray<SObjChangeSize> oldObjectSizes;

	asCMap<void*,bool> existingShared;
	asCMap<asCScriptFunction*,bool> dontTranslate;
};

#ifndef AS_NO_COMPILER

class asCWriter
{
public:
	asCWriter(asCModule *module, asIBinaryStream *stream, asCScriptEngine *engine);

	int Write();

protected:
	asCModule       *module;
	asIBinaryStream *stream;
	asCScriptEngine *engine;

	void WriteData(const void *data, asUINT size);

	void WriteString(asCString *str);
	void WriteFunction(asCScriptFunction *func);
	void WriteFunctionSignature(asCScriptFunction *func);
	void WriteGlobalProperty(asCGlobalProperty *prop);
	void WriteObjectProperty(asCObjectProperty *prop);
	void WriteDataType(const asCDataType *dt);
	void WriteObjectType(asCObjectType *ot);
	void WriteObjectTypeDeclaration(asCObjectType *ot, int phase);
	void WriteByteCode(asDWORD *bc, int length);
	void WriteEncodedUInt(asUINT i);

	// Helper functions for storing variable data
	int FindObjectTypeIdx(asCObjectType*);
	int FindTypeIdIdx(int typeId);
	int FindFunctionIndex(asCScriptFunction *func);
	int FindGlobalPropPtrIndex(void *);
	int FindStringConstantIndex(int id);
	int FindObjectPropIndex(short offset, int typeId);

	// Intermediate data used for storing that which isn't constant, function id's, pointers, etc
	void WriteUsedTypeIds();
	void WriteUsedFunctions();
	void WriteUsedGlobalProps();
	void WriteUsedStringConstants();
	void WriteUsedObjectProps();

	// Temporary storage for persisting variable data
	asCArray<int>                usedTypeIds;
	asCArray<asCObjectType*>     usedTypes;
	asCArray<asCScriptFunction*> usedFunctions;
	asCArray<void*>              usedGlobalProperties;
	asCArray<int>                usedStringConstants;
	asCMap<int, int>             stringIdToIndexMap;

	asCArray<asCScriptFunction*>  savedFunctions;
	asCArray<asCDataType>         savedDataTypes;
	asCArray<asCString>           savedStrings;
	asCMap<asCStringPointer, int> stringToIdMap;

	struct SObjProp
	{
		asCObjectType *objType;
		int            offset;
	};
	asCArray<SObjProp>           usedObjectProperties;
};

#endif

END_AS_NAMESPACE

#endif // AS_RESTORE_H
