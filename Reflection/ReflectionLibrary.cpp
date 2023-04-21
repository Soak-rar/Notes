// Fill out your copyright notice in the Description page of Project Settings.


#include "Reflection/ReflectionLibrary.h"

#include "Reflection/ReflectionHelper.h"

void UReflectionLibrary::SetBoolProperty(UObject* Object, const FString& PropertyName, bool Value)
{
	ReflectionHelper::SetPropertyValue<FBoolProperty>(Object, *PropertyName, Value);
}

bool UReflectionLibrary::GetBoolProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FBoolProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetByteProperty(UObject* Object, const FString& PropertyName, uint8 Value)
{
	ReflectionHelper::SetPropertyValue<FByteProperty>(Object, *PropertyName, Value);
}

uint8 UReflectionLibrary::GetByteProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FByteProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetInt32Property(UObject* Object, const FString& PropertyName, int32 Value)
{
	ReflectionHelper::SetPropertyValue<FIntProperty>(Object, *PropertyName, Value);
}

int32 UReflectionLibrary::GetInt32Property(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FIntProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetInt64Property(UObject* Object, const FString& PropertyName, int64 Value)
{
	ReflectionHelper::SetPropertyValue<FInt64Property>(Object, *PropertyName, Value);
}

int64 UReflectionLibrary::GetInt64Property(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FInt64Property>(Object, *PropertyName);
}

void UReflectionLibrary::SetFloatProperty(UObject* Object, const FString& PropertyName, float Value)
{
	ReflectionHelper::SetPropertyValue<FFloatProperty>(Object, *PropertyName, Value);
}

float UReflectionLibrary::GetFloatProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FFloatProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetStringProperty(UObject* Object, const FString& PropertyName, const FString& Value)
{
	ReflectionHelper::SetPropertyValue<FStrProperty>(Object, *PropertyName, Value);
}

FString UReflectionLibrary::GetStringProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FStrProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetTextProperty(UObject* Object, const FString& PropertyName, const FText& Value)
{
	ReflectionHelper::SetPropertyValue<FTextProperty>(Object, *PropertyName, Value);
}

FText UReflectionLibrary::GetTextProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FTextProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetNameProperty(UObject* Object, const FString& PropertyName, const FName& Value)
{
	ReflectionHelper::SetPropertyValue<FNameProperty>(Object, *PropertyName, Value);
}

FName UReflectionLibrary::GetNameProperty(UObject* Object, const FString& PropertyName)
{
	return ReflectionHelper::GetPropertyValue<FNameProperty>(Object, *PropertyName);
}

void UReflectionLibrary::SetEnumProperty(UObject* Object, const FString& PropertyName, int64 Value)
{
	FProperty* Property = Object->GetClass()->FindPropertyByName(FName(PropertyName));
	if (Property != nullptr)
	{
		void* PropertyValue = GetPropertyValue(Object, *PropertyName);
		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(PropertyValue, Value);

		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			if (ByteProperty->IsEnum())
				ByteProperty->SetIntPropertyValue(PropertyValue, Value);
		}
	}
}

int64 UReflectionLibrary::GetEnumProperty(UObject* Object, const FString& PropertyName)
{
	FProperty* Property = Object->GetClass()->FindPropertyByName(FName(PropertyName));
	if (Property != nullptr)
	{
		void* PropertyValue = GetPropertyValue(Object, *PropertyName);
		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			return EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(PropertyValue);

		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			if (ByteProperty->IsEnum())
				return ByteProperty->GetPropertyValue(PropertyValue);
		}
	}
	return 0;
}

void UReflectionLibrary::CallScriptFunction(UObject* Object, const FString& FunctionName)
{
	ReflectionHelper::CallScriptFunction(Object, *FunctionName);
}

FString UReflectionLibrary::GetSuperType(UObject* Object, bool IsRecursion)
{
	if (!Object)
		return "";

	const UClass* Class = Object->GetClass();
	if (!Class)
		return "";

	UClass* SuperClass = Class->GetSuperClass();
	if(!SuperClass)
		return Class->GetName();
	
	if (!IsRecursion)
		return SuperClass->GetName();

	FString Ret;
	while (SuperClass)
	{
		Ret = SuperClass->GetName();
		SuperClass = SuperClass->GetSuperClass();
	}

	return Ret;
}

bool UReflectionLibrary::IsInstanceOf(UObject* Object, const FString& CPPTypeName, bool IsRecursion)
{
	if (!Object)
		return false;

	const UClass* Class = Object->GetClass();
	if (!Class)
		return false;

	UClass* SuperClass = Class->GetSuperClass();
	if(!SuperClass)
		return Class->GetName() == CPPTypeName;

	if(!IsRecursion)
		return SuperClass->GetName() == CPPTypeName;
	
	while (SuperClass)
	{
		if(SuperClass->GetName() == CPPTypeName)
			return true;
		SuperClass = SuperClass->GetSuperClass();
	}

	return false;
}

UObject* UReflectionLibrary::LoadBlueprintByPath(const FString& Path)
{
	return StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
}

void* UReflectionLibrary::GetPropertyValue(UObject* Object, const FString& PropertyName)
{
	FProperty* Property = Object->GetClass()->FindPropertyByName(FName(PropertyName));
	if (Property)
		return Property->ContainerPtrToValuePtr<void>(Object);

	return nullptr;
}
