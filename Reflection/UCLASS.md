### 生成阶段
UCLASS在第一次调用StaticClass()方法时创建，分为两个过程，UCLASS的构造，其中会包含对应UObject的简单信息了包括UObject的大小，对齐，标志信息，构造函数指针（在UObject构造之前创建的UCLASS）等。

### 收集阶段
使用静态注册的方式（即 全局静态对象的创建 早于 main函数）创建TClassCompiledInDefer<MyClass>对象，并保存到一个全局列表DeferredClassRegistration 中，该对象将 MyClass::StaticClass的方法封装到自己的Register()方法中，并在后续的注册阶段遍历这个全局列表进行 MyClass::StaticClass方法的第一次调用，改调用只是记录了MyClass的类基本信息。
MyClass的UProperty和UFunction元信息补充 将会在Z_Construct_UClass_方法中完成

### 注册阶段
CoreUObject模块加载时进行，首先会遍历DeferredClassRegistration 列表，调用TClassCompiledInDefer中的Register()方法，在Register()方法中调用 MyClass::StaticClass
```C++
inline static UClass* StaticClass() \
{ \
	return GetPrivateStaticClass(); \
} \

UClass* TClass::GetPrivateStaticClass() \
{ \
	static UClass* PrivateStaticClass = NULL; \
	if (!PrivateStaticClass) \
	{ \
		/* this could be handled with templates, but we want it external to avoid code bloat */ \
		GetPrivateStaticClassBody( \
			StaticPackage(), \
			(TCHAR*)TEXT(#TClass) + 1 + ((StaticClassFlags & CLASS_Deprecated) ? 11 : 0), \
			PrivateStaticClass, \
			StaticRegisterNatives##TClass, \
			sizeof(TClass), \
			alignof(TClass), \
			(EClassFlags)TClass::StaticClassFlags, \
			TClass::StaticClassCastFlags(), \
			TClass::StaticConfigName(), \
			(UClass::ClassConstructorType)InternalConstructor<TClass>, \
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>, \
			&TClass::AddReferencedObjects, \
			&TClass::Super::StaticClass, \
			&TClass::WithinClass::StaticClass \
		); \
	} \
	return PrivateStaticClass; \
}

void GetPrivateStaticClassBody(
	const TCHAR* PackageName,
	const TCHAR* Name,
	UClass*& ReturnClass,
	void(*RegisterNativeFunc)(),
	uint32 InSize,
	uint32 InAlignment,
	EClassFlags InClassFlags,
	EClassCastFlags InClassCastFlags,
	const TCHAR* InConfigName,
	UClass::ClassConstructorType InClassConstructor,
	UClass::ClassVTableHelperCtorCallerType InClassVTableHelperCtorCaller,
	UClass::ClassAddReferencedObjectsType InClassAddReferencedObjects,
	UClass::StaticClassFunctionType InSuperClassFn,
	UClass::StaticClassFunctionType InWithinClassFn,
	bool bIsDynamic /*= false*/,
	UDynamicClass::DynamicClassInitializerType InDynamicClassInitializerFn /*= nullptr*/
	)
{
	......
	ReturnClass = (UClass*)GUObjectAllocator.AllocateUObject(sizeof(UClass), alignof(UClass), true);
	ReturnClass = ::new (ReturnClass)
		UClass
		(
		EC_StaticConstructor,
		Name,
		InSize,
		InAlignment,
		InClassFlags,
		InClassCastFlags,
		InConfigName,
		EObjectFlags(RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet),
		InClassConstructor,
		InClassVTableHelperCtorCaller,
		InClassAddReferencedObjects
		);
	check(ReturnClass);
	......
	InitializePrivateStaticClass(
		InSuperClassFn(),
		ReturnClass,
		InWithinClassFn(),
		PackageName,
		Name
		);

	// Register the class's native functions.
	RegisterNativeFunc();
}
```

