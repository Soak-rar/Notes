### 标准c++ std::string <--> FString

```C++

// std::string --> FString
std::string str;
FString fstr = FString(str.c_str());

// std::string <-- FString

FString fstr;
std::string str = TCHAR_TO_UTF8(*fstr);

```

### 字符串切分
```C++
FString SourceStr = "45,489, 489, 456, 15";
TArray<FString> NewList;
SourceStr.ParseIntoArray(NewList, TEXT(","), true); // 45  489  489  456  15

```
