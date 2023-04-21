// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Def.h"

class UUGCEntityComponent;

/**
 *
 */
class EDITORSHARE_API ReflectionHelper
{
public:
    template <typename T>
    static T* FindPropertyChecked(UStruct* Class, const TCHAR* PropertyName)
    {
       T* Property = Cast<T>(Class->FindPropertyByName(PropertyName));
       checkf(Property, TEXT("Property with given name not found in class: %s"), PropertyName);
       return Property;
    }

    template <typename T>
    static T* FindPropertyByShortNameChecked(UStruct* Class, const TCHAR* PropertyName)
    {
       for (FProperty* Property = Class->PropertyLink; Property; Property = Property->PropertyLinkNext)
       {
        if (Property->GetName().StartsWith(PropertyName))
        {
         if (T* CastedProperty = Cast<T>(Property))
         {
          return CastedProperty;
         }
        }
       }
       checkf(false, TEXT("Property with given name not found in class: %s"), PropertyName);
       return nullptr;
    }

    static void*  FindParentObject(void* objectPtr, FString propertyName,FProperty* &outProperty);

    template <typename T>
    static typename T::TCppType GetPropertyValue(const UObject* Object, const TCHAR* PropertyName, int32 ArrayIndex = 0)
    {
      T* Property = CastField<T>(Object->GetClass()->FindPropertyByName(PropertyName));
      checkf(Property, TEXT("Property not found in class %s: %s"), *Object->GetClass()->GetPathName(), PropertyName);
      return Property->GetPropertyValue_InContainer(Object, ArrayIndex);
    }

    template <typename T>
    static typename T::TCppType GetPropertyValue(const void* Object, FProperty* property,int32 ArrayIndex = 0)
    {
       T::TCppType reValue = T::TCppType();
       if (Object == nullptr || property == nullptr)
       {
        return reValue;
       }
       T* Property = CastField<T>(property);
	  
       if (Property)
       {
        return Property->GetPropertyValue_InContainer(Object,ArrayIndex);
       }
       return reValue;
    }

    template <typename T>
    static T* GetObjectPropertyValue(const UObject* Object, const TCHAR* PropertyName)
    {
        UObject* FieldValue = GetPropertyValue<FObjectProperty>(Object, PropertyName);
        T* CastedValue = CastField<T>(FieldValue);
        checkf(CastedValue, TEXT("Cannot cast class property %s:%s value to type %s"),
         *Object->GetClass()->GetPathName(), PropertyName, *T::StaticClass()->GetName());
        return CastedValue;
    }

    template <typename T>
    static T* GetObjectPropertyValue(void*  Object, FProperty* property)
    {
     	   T* CastedValue = nullptr;
     	   if (Object != nullptr && property != nullptr)
     	   {
     		   FObjectProperty* ObjectProperty = CastField<FObjectProperty>(property);
     		   if (ObjectProperty)
     		   {
     			   const void* valuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(Object);
     			   UObject* objPtr = ObjectProperty->GetObjectPropertyValue(valuePtr);
     			   if (objPtr && objPtr->IsValidLowLevel())
     			   {
     				   CastedValue = Cast<T>(objPtr);
     			   }
     		   }
     	   }
     	   
     	   return CastedValue;
    }

    static void SetStructPropertyValue(UObject* Object, const TCHAR* PropertyName, const void* ValuePointer, int32 ArrayIndex = 0);

    template <typename T>
    static void SetStructPropertyValue(void*  Object, FProperty* property, T Value, int32 ArrayIndex = 0)
    {
        FStructProperty* StructProp = CastField<FStructProperty>(property);
        if (StructProp)
        {
         *StructProp->ContainerPtrToValuePtr<T>(Object) = Value;
        }
    }

    template <typename T>
    static T* GetStructPropertyValue(void* Object, FProperty* property, int32 ArrayIndex = 0)
    {
         FStructProperty* StructProp = CastField<FStructProperty>(property);
         if (StructProp)
         {
           return StructProp->ContainerPtrToValuePtr<T>(Object);
         }
         return nullptr;
    }

    template <typename T>
    static void SetPropertyValue(UObject* Object, const TCHAR* PropertyName, const typename T::TCppType& Value, int32 ArrayIndex = 0)
    {
         T* Property = CastField<T>(Object->GetClass()->FindPropertyByName(PropertyName));
         checkf(Property, TEXT("Property not found in class %s: %s"), *Object->GetClass()->GetPathName(), PropertyName);
         Property->SetPropertyValue_InContainer(Object, Value, ArrayIndex);
    }

     template <typename T>
     static void SetPropertyValue(void* Object, FProperty* property,const typename T::TCppType& Value, int32 ArrayIndex = 0)
    {
        T* Property = CastField<T>(property);
        if (Property)
        {
         Property->SetPropertyValue_InContainer(Object, Value, ArrayIndex);
        }
    }

    static FProperty* FindParameterByIndex(UFunction* Function, int32 Index);

    static UFunction* CallScriptFunction(UObject* Object, const TCHAR* FunctionName);

    static void GetPropertyClassAndValuePtr(FString propertyName, UStruct* inStruct,const void* InValuePtr,FProperty* &outProperty,UStruct*& outStruct,const void* &outValuePtr);

    static void strSplit(FString inStr,int32 count,FString &str1,FString &str2);

    template <typename T>
    static UFunction* CallScriptFunction(UObject* Object, const TCHAR* FunctionName, T* ParamStruct)
    {
        UFunction* Function = Object->FindFunction(FunctionName);
        checkf(Function, TEXT("Function not found: %s"), FunctionName);
        checkf(
         Function->ParmsSize == static_cast<uint16>(sizeof(T)) || (Function->ParmsSize == 0 && ParamStruct == NULL),
         TEXT("Function parameter layout doesn't match provided parameter struct: Expected %d bytes, got %llu"),
         Function->ParmsSize, sizeof(T));
        Object->ProcessEvent(Function, ParamStruct);
        return Function;
    }

    /**
    * 根据Property 返回参数类型
    * @brief 根据Property 返回参数类型
    */
    static  EUYParamType GetParamTyeByProperty(const FProperty* propertys);

    /**
     * 黑白名单判断规则为先使用白名单筛选，然后使用黑名单，先使用基础黑白名单筛选，然后使用特殊黑白名单
     * @brief 获得UClass或者struct的成员属性
     * @param propertys 属性map，键为属性名称
     * @param basicWhiteList 基础白名单
     * @param basicBlackList 基础黑名单
     * @param specialWhiteList 特殊白名单
     * @param specialBlackList 特殊黑名单
     * @return 如果classObject为空，或发生其他错误则返回false
    */
    static bool GetClassProperty(const UStruct* classObject, TMap<FString, FProperty*>& propertys,
                                 TArray<FString> basicWhiteList = {}, TArray<FString> basicBlackList = {},
                                 TArray<FString> specialWhiteList = {}, TArray<FString> specialBlackList = {});

   /**
   * 获取未知深度层级属性
   * @brief 获取未知深度层级属性
   * @param classObject 输入对象的数据
   * @param dataSturt   输入的对象class
   * @param unknowProperty 返回的位置深度变量指针
   * @param unknowEntityComp 返回的位置深度实体指针
   * @param propertyName 当前变量名称用于筛选特殊白名单用
   * @param rePropertyName 返回新版变量名称（因为entity的id可能会变化）
   * @return 失败返回false 成功返回true
  */
   static bool GetUnknownDepthProperty(const void* classObject,UStruct* dataSturt,FProperty* &unknowProperty, UUGCEntityComponent* &unknowEntityComp, FString propertyName,FString &rePropertyName);

    /**
     * 黑白名单判断规则为先使用白名单筛选，然后使用黑名单，先使用基础黑白名单筛选，然后使用特殊黑白名单
     * @brief 获得UObject的全部属性
     * @param propertys UObject的属性，键为属性名称 （若对象为entitycompoent则返回值不包括selfcomponet和childcomponet）
     * @param selfComponentPropertys EntityComponent属性，键为UUGCEntityComponent指针，值为类名称 (若entitycompent为特殊指定的，则存特殊指定的名称)
     * @param childComponentPropertys EntityComponent属性，键为UUGCEntityComponent指针，值为类名称 (若entitycompent为特殊指定的，则存特殊指定的名称)
     * @param basicWhiteList 基础白名单
     * @param basicBlackList 基础黑名单
     * @param specialWhiteList 特殊白名单
     * @param specialBlackList 特殊黑名单
     * @param createList 直接按照这个名单返回属性，即黑白名单失效
     * @return 如果objectptr为空，或发生其他错误则返回false
    */
    static bool GetProperty(const void* objectptr, TMap<FString, FProperty*>& propertys,
                            TMap<UUGCEntityComponent*, FString>& selfComponentPropertys, TMap<UUGCEntityComponent*, FString>& childComponentPropertys,
                            TArray<FString> basicWhiteList = {}, TArray<FString> basicBlackList = {},
                            TArray<FString> specialWhiteList = {}, TArray<FString> specialBlackList = {}, TArray<FString> createList = {}, UStruct*
                             classPtr = nullptr);

private:
    /**
     * 检测规则为白名单checkNames中有一个存在即可通过，黑名单checkNames中有一个存在即不通过，名单如果为空则为不使用
     * @brief 检测类名/属性名是否符合规则
     * @param checkNames 检测姓名组，可单独使用类名或者属性名，或者两个一起使用
     * @param basicWhiteList 基础白名单
     * @param basicBlackList 基础黑名单
     * @param specialWhiteList 特殊白名单
     * @param specialBlackList 特殊黑名单
     * @param propertyName 返回表中属性名称
     * @return 是否符合规则
    */
    static bool CheckProperty(const TArray<FString>& checkNames, const TArray<FString>& basicWhiteList, const TArray<FString>& basicBlackList,
        const TArray<FString>& specialWhiteList, const TArray<FString>& specialBlackList);
};
