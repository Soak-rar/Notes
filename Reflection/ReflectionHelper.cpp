// Fill out your copyright notice in the Description page of Project Settings.


#include "Reflection/ReflectionHelper.h"
#include "Component/UGCEntityComponent.h"
#include "Datasmith/DatasmithCore/Public/DatasmithDefinitions.h"
#include "Logging/LogMacros.h"

#pragma optimize("",off)

void* ReflectionHelper::FindParentObject(void* objectPtr, FString propertyName,FProperty* &outProperty)
{
	if (objectPtr== nullptr || propertyName.IsEmpty())
	{
		return nullptr;
	}
	
	int32 entityCount = propertyName.Find("#");
	int32 subCount = propertyName.Find("@");
	FString tempPropertyName = propertyName;

	if ( (entityCount != -1 && subCount == -1) ||
		(entityCount < subCount && entityCount != -1 && subCount != -1))
	{
		FString entityPropertyName;
		strSplit(propertyName,entityCount,tempPropertyName,entityPropertyName);
			
		int32 objectId = FCString::Atoi(*tempPropertyName);
		objectPtr = UYComponentManager::GetComponentByID(objectId);
		if (!entityPropertyName.IsEmpty())
		{
			return FindParentObject(objectPtr,entityPropertyName,outProperty);
		}
	}
	else if ((entityCount == -1 && subCount != -1)||
	(entityCount > subCount && entityCount != -1 && subCount != -1) )
	{
		const UObject* tempPtr = reinterpret_cast<const UObject*>(objectPtr);
		if (tempPtr)
		{
			FString subPropertyName;
			strSplit(propertyName,subCount,subPropertyName,tempPropertyName);
			UStruct* structValuePtr = nullptr;
			const void* destAddress = nullptr;
			FProperty* subProperty = nullptr;
			GetPropertyClassAndValuePtr(subPropertyName,tempPtr->GetClass(),objectPtr,subProperty,structValuePtr,destAddress);
			if (subProperty != nullptr)
			{
				outProperty = subProperty;
				void* resPtr = const_cast<void*>(destAddress);
				if (resPtr && !tempPropertyName.IsEmpty())
				{
					return FindParentObject(resPtr,tempPropertyName,outProperty);
				}
			}
		}

	}
	
	return objectPtr;
}

void ReflectionHelper::SetStructPropertyValue(UObject* Object, const TCHAR* PropertyName, const void* ValuePointer,
                                              int32 ArrayIndex /*= 0*/)
{
	FStructProperty* Property = CastField<FStructProperty>(Object->GetClass()->FindPropertyByName(PropertyName));
	checkf(Property, TEXT("Property not found in class %s: %s"), *Object->GetClass()->GetPathName(), PropertyName);
	void* DestAddress = Property->ContainerPtrToValuePtr<void>(Object, ArrayIndex);
	Property->CopyValuesInternal(DestAddress, ValuePointer, 1);
}

FProperty* ReflectionHelper::FindParameterByIndex(UFunction* Function, int32 Index)
{
	int32 CurrentIndex = 0;
	for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
	{
		if (CurrentIndex++ == Index)
			return *It;
	}
	return nullptr;
}

UFunction* ReflectionHelper::CallScriptFunction(UObject* Object, const TCHAR* FunctionName)
{
	UFunction* Function = Object->FindFunction(FunctionName);
	checkf(Function, TEXT("Function not found: %s"), FunctionName);
	Object->ProcessEvent(Function, nullptr);
	return Function;
}

void ReflectionHelper::GetPropertyClassAndValuePtr(FString propertyName, UStruct* inStruct,const void* InValuePtr,FProperty* &outProperty, UStruct*& outStruct,const void* &outValuePtr)
{
	if (inStruct != nullptr)
	{
		outProperty = inStruct->FindPropertyByName(*propertyName);
		if (outProperty != nullptr)
		{
			FObjectProperty* ObjectProperty = CastField<FObjectProperty>(outProperty);
			if (ObjectProperty)
			{
				const void* ValuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(InValuePtr);
				UObject* ObjectPtr = ObjectProperty->GetObjectPropertyValue(ValuePtr);
				if (ObjectPtr)
				{
					outStruct = ObjectPtr->GetClass();
					outValuePtr = (void*)ObjectPtr;
				}	
			}
			else
			{
				outValuePtr = outProperty->ContainerPtrToValuePtr<void>(InValuePtr);
				FStructProperty* StructProperty = CastField<FStructProperty>(outProperty);
				if (StructProperty)
				{
					outStruct = StructProperty->Struct;
				}
			}
		}
	}
}

void ReflectionHelper::strSplit(FString inStr, int32 count, FString& str1, FString& str2)
{
	if (inStr.Len() > count && count != -1)
	{
		str1 = inStr.Left(count);
		str2 = inStr.Mid(count+1,inStr.Len());
	}
}

bool ReflectionHelper::GetClassProperty(const ::UStruct* classObject, TMap<FString, FProperty*>& propertys,
                                        TArray<FString> basicWhiteList, TArray<FString> basicBlackList,
                                        TArray<FString> specialWhiteList, TArray<FString> specialBlackList)
{
	if (nullptr == classObject)
	{
		return false;
	}

	for (TFieldIterator<FProperty> It(classObject); It; ++It)
	{
		if (FProperty* property = CastField<FProperty>(*It))
		{
			if (CheckProperty({ property->GetCPPType(), property->GetFName().ToString() }, basicWhiteList, basicBlackList, specialWhiteList, specialBlackList))
			{
				propertys.Add(property->GetFName().ToString(), property);
				
			}
		}
	}

	return true;
}

bool ReflectionHelper::GetUnknownDepthProperty(const void* classObject,UStruct* dataSturt,FProperty* &unknowProperty, UUGCEntityComponent* &unknowEntityComp,FString propertyName,FString &rePropertyName)
{
	int32 entityCount = propertyName.Find("#");
	int32 subCount = propertyName.Find("@");
	FString tempPropertyName = propertyName;
	
	
	if ( (entityCount != -1 && subCount == -1) ||
		(entityCount < subCount && entityCount != -1 && subCount != -1))
	{
		FString entityPropertyName;
		strSplit(propertyName,entityCount,tempPropertyName,entityPropertyName);
		
		const UUGCEntityComponent* entitycomp = reinterpret_cast<const UUGCEntityComponent*>(classObject);
		UUGCEntityComponent* retEntityCompoent = nullptr;
		if (entitycomp)
		{
			TSharedPtr<FJsonObject> changeList = entitycomp->GetChangeList();

			int32 childObjectid =  entitycomp->FindChildObjectId(tempPropertyName);
			
			for (auto& tempEntityComponent : entitycomp->selfComponent)
			{
				UUGCEntityComponent* entityComponent = tempEntityComponent.Value;
				if (childObjectid == entityComponent->GetObjectId())
				{
					retEntityCompoent = entityComponent;
					break;
				}
			}

			if (retEntityCompoent == nullptr)
			{
				for (auto& tempEntityComponent : entitycomp->childComponent)
				{
					UUGCEntityComponent* entityComponent = tempEntityComponent.Value;
					if (childObjectid == entityComponent->GetObjectId())
					{
						retEntityCompoent = entityComponent;
						break;
					}
				}
			}
			
			if (retEntityCompoent)
			{
				FString tempPropetyName = FString::FromInt(retEntityCompoent->GetObjectId());
				rePropertyName.Append(tempPropetyName+"#");
				if (!entityPropertyName.IsEmpty())
				{
					return GetUnknownDepthProperty(retEntityCompoent,retEntityCompoent->GetClass(),unknowProperty,unknowEntityComp,entityPropertyName,rePropertyName);
				}
				else
				{
					unknowEntityComp = retEntityCompoent;
					return true;
				}
			}



		}
	}
	else if ((entityCount == -1 && subCount != -1)||
		(entityCount > subCount && entityCount != -1 && subCount != -1) )
	{
		FString subPropertyName;
		strSplit(propertyName,subCount,tempPropertyName,subPropertyName);

		FProperty* subProperty = nullptr;
		const void* valuePtr = nullptr;
		UStruct* structValue = nullptr;
		GetPropertyClassAndValuePtr(tempPropertyName,dataSturt,classObject,subProperty,structValue,valuePtr);
		if (subProperty == nullptr)
		{
			return false;
		}
		
		rePropertyName.Append(tempPropertyName+"@");

		if (!subPropertyName.IsEmpty())
		{
			return GetUnknownDepthProperty(valuePtr,structValue,unknowProperty,unknowEntityComp,subPropertyName,rePropertyName);
		}
		
		unknowProperty = subProperty;
		return true;
		
	}

	return false;
}

bool ReflectionHelper::GetProperty(const void* objectptr, TMap<FString, FProperty*>& propertys,
                                   TMap<UUGCEntityComponent*, FString>& selfComponentPropertys, TMap<UUGCEntityComponent*, FString>& childComponentPropertys,
                                   TArray<FString> basicWhiteList, TArray<FString> basicBlackList,
                                   TArray<FString> specialWhiteList, TArray<FString> specialBlackList, TArray<FString> createList, UStruct* classPtr)
{
	if (nullptr == objectptr)
	{
		return false;
	}

	const UObject* objPtr = reinterpret_cast<const UObject*>(objectptr);
	const UUGCEntityComponent* entityCompPtr = nullptr;
	
	if (classPtr == nullptr && objPtr != nullptr && objPtr->IsValidLowLevel())
	{
		classPtr = objPtr->GetClass();
		entityCompPtr = Cast<UUGCEntityComponent>(objPtr);
	}

	if (createList.Num() == 0)
	{
		TArray<FString> TempSpecialWhiteList;
		TArray<FString> tempBasicBlackList = basicBlackList;
		for(FString &whiteList : specialWhiteList)
		{
			if (whiteList.Find("@") || whiteList.Find("#"))
			{
				TempSpecialWhiteList.Add(whiteList);
			}
		}

		

		for(FString &specialName : TempSpecialWhiteList)
		{
			FProperty* backProperty = nullptr;
			UUGCEntityComponent* backEntityComp = nullptr;
			FString newPropertyName;
			GetUnknownDepthProperty(objectptr,classPtr,backProperty,backEntityComp,specialName,newPropertyName);
			if (backProperty)
			{
				FString tempProperty = specialName + "," + newPropertyName;
				propertys.Add(tempProperty,backProperty);
			}

			if (backEntityComp)
			{
				FString tempProperty = specialName + "," + newPropertyName;
				selfComponentPropertys.Add(backEntityComp,tempProperty);
			}
		}
		
		if (entityCompPtr)
		{
			TArray<int32> blackObjectIdList;
			tempBasicBlackList.Add("selfComponent");
			tempBasicBlackList.Add("childComponent");

			for(FString &blackList : specialBlackList)
			{
				int32 entityCount = blackList.Find("#");
				if (entityCount != -1)
				{
					FString tempChildEntity = blackList.Left(entityCount);
					int32 childObjectid = entityCompPtr->FindChildObjectId(tempChildEntity);
					blackObjectIdList.Add(childObjectid);
				}
			}
		
			for (auto& tempEntityComponent : entityCompPtr->selfComponent)
			{
				UUGCEntityComponent* entityComponent = tempEntityComponent.Value;
				if (entityComponent == nullptr || nullptr == entityComponent->GetClass() || blackObjectIdList.Find(entityComponent->GetObjectId()) != -1)
				{
					continue;
				}
				if (CheckProperty({ entityComponent->GetClass()->GetName() }, basicWhiteList, basicBlackList, specialWhiteList, specialBlackList))
				{
					selfComponentPropertys.Add(entityComponent, entityComponent->GetClass()->GetName());
				
				}
			}

			for (auto& tempEntityComponent : entityCompPtr->childComponent)
			{
				UUGCEntityComponent* entityComponent = tempEntityComponent.Value;
				if (entityComponent == nullptr || nullptr == entityComponent->GetClass() || blackObjectIdList.Find(entityComponent->GetObjectId()) != -1)
				{
					continue;
				}
				if (CheckProperty({ entityComponent->GetClass()->GetName() }, basicWhiteList, basicBlackList, specialWhiteList, specialBlackList))
				{
					childComponentPropertys.Add(entityComponent, entityComponent->GetClass()->GetName());
				}
			}
		}
		
		if (!GetClassProperty(classPtr, propertys, basicWhiteList, tempBasicBlackList, specialWhiteList, specialBlackList))
		{
			return false;
		}
	}
	else
	{
		for(FString &createValue : createList)
		{
			if (createValue.Find("@") != -1 || createValue.Find("#") != -1)
			{
				FProperty* backProperty = nullptr;
				UUGCEntityComponent* backEntityComp = nullptr;
				FString newPropertyName;
				GetUnknownDepthProperty(objectptr,classPtr,backProperty,backEntityComp,createValue,newPropertyName);
				if (backProperty)
				{
					FString tempProperty = createValue + "," + newPropertyName;
					propertys.Add(tempProperty,backProperty);
				}

				if (backEntityComp)
				{
					FString tempProperty = createValue + "," + newPropertyName;
					selfComponentPropertys.Add(backEntityComp,tempProperty);
				}
			}
			else
			{
				FProperty* property = classPtr->FindPropertyByName(*createValue);
				if (property)
				{
					propertys.Add(createValue,property);
				}
			}
		}
	}

	
	return true;
}

bool ReflectionHelper::CheckProperty(const TArray<FString>& checkNames, const TArray<FString>& basicWhiteList,
	const TArray<FString>& basicBlackList, const TArray<FString>& specialWhiteList, const TArray<FString>& specialBlackList)
{
	for (const FString& checkName : checkNames)
	{
		if (INDEX_NONE != specialWhiteList.Find(checkName))
		{
			return true;
		}
	}
	
	for (const FString& checkName : checkNames)
	{
		if (INDEX_NONE != specialBlackList.Find(checkName))
		{
			return false;
		}
	}
	
	for (const FString& checkName : checkNames)
	{
		for (const FString& basicWhrite : basicWhiteList)
		{
			if (INDEX_NONE != basicWhiteList.Find(checkName))
			{
				return true;
			}
		}
	}
	
	for (const FString& checkName : checkNames)
	{
		if (INDEX_NONE != basicBlackList.Find(checkName))
		{
			return false;
		}
	}
	
	if (specialWhiteList.Num() != 0 || basicWhiteList.Num() != 0)
	{
		return false;
	}

	return true;
}

EUYParamType ReflectionHelper::GetParamTyeByProperty(const FProperty* propertys)
{
	EUYParamType Type = EUYParamType::EParamNone;
	if (propertys)
	{
		if (CastField<FIntProperty>(propertys))
		{
			Type = EUYParamType::EParamInt32;
		}
		else if (CastField<FBoolProperty>(propertys))
		{
			Type = EUYParamType::EParamBool;
		}
		else if (CastField<FByteProperty>(propertys))
		{
			Type = EUYParamType::EParamByte;
		}
		else if (CastField<FInt64Property>(propertys))
		{
			Type = EUYParamType::EParamInt64;
		}
		else if (CastField<FFloatProperty>(propertys))
		{
			Type = EUYParamType::EParamFloat;
		}
		else if (CastField<FDoubleProperty>(propertys))
		{
			Type = EUYParamType::EParamDouble;
		}
		else if (CastField<FStrProperty>(propertys))
		{
			Type = EUYParamType::EParamFString;
		}
		else if (CastField<FTextProperty>(propertys))
		{
			Type = EUYParamType::EParamFText;
		}
		else if (CastField<FNameProperty>(propertys))
		{
			Type = EUYParamType::EParamFName;
		}
		else if (CastField<FEnumProperty>(propertys))
		{
			Type = EUYParamType::EParamEnum;
		}
		else if (CastField<FStructProperty>(propertys))
		{

			FString TypeName = propertys->GetCPPType();

			Type = EUYParamType::EParamStruct;
			if (TypeName == TEXT("FTransform"))
			{
				Type = EUYParamType::EParamTransfrom;
			}
			else if (TypeName == TEXT("FLinearColor"))
			{
				Type = EUYParamType::EParamColor;
			}
		}
		else if (CastField<FObjectProperty>(propertys))
		{
			Type = EUYParamType::EParamObject;

			FString TypeName = propertys->GetCPPType();
			if (TEXT("TObjectPtr<UTexture>") == TypeName)
			{
				Type = EUYParamType::EParamDTexture;
			}
		}
		else if (CastField<FArrayProperty>(propertys))
		{
			Type = EUYParamType::EParamTArray;
		}

	}
	return  Type;
}

#pragma optimize("", on)