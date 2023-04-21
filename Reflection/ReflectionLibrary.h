// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReflectionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EDITORSHARE_API UReflectionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetBoolProperty(UObject* Object, const FString& PropertyName, bool Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static bool GetBoolProperty(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetByteProperty(UObject* Object, const FString& PropertyName, uint8 Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static uint8 GetByteProperty(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetInt32Property(UObject* Object, const FString& PropertyName, int32 Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static int32 GetInt32Property(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetInt64Property(UObject* Object, const FString& PropertyName, int64 Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static int64 GetInt64Property(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetFloatProperty(UObject* Object, const FString& PropertyName, float Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static float GetFloatProperty(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetStringProperty(UObject* Object, const FString& PropertyName, const FString& Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static FString GetStringProperty(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetTextProperty(UObject* Object, const FString& PropertyName, const FText& Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static FText GetTextProperty(UObject* Object, const FString& PropertyName);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetNameProperty(UObject* Object, const FString& PropertyName, const FName& Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static FName GetNameProperty(UObject* Object, const FString& PropertyName);

	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void SetEnumProperty(UObject* Object, const FString& PropertyName, int64 Value);
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static int64 GetEnumProperty(UObject* Object, const FString& PropertyName);

	UFUNCTION(BlueprintCallable, Category=Reflection)
	static void CallScriptFunction(UObject* Object, const FString& FunctionName);

	UFUNCTION(BlueprintCallable, Category=Reflection)
	static FString GetSuperType(UObject* Object, bool IsRecursion);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static bool IsInstanceOf(UObject* Object, const FString& CPPTypeName, bool IsRecursion);
	
	UFUNCTION(BlueprintCallable, Category=Reflection)
	static UObject* LoadBlueprintByPath(const FString& Path);
private:
	static void* GetPropertyValue(UObject* Object, const FString& PropertyName);
};
