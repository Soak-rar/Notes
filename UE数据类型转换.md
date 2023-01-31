### 标准c++ std::string <--> FString

```C++

// std::string --> FString
std::string str;
FString fstr = FString(str.c_str());

// std::string <-- FString

FString fstr;
std::string str = TCHAR_TO_UTF8(*fstr);

```
