### 在```GameMode```中进行UMG初始化以及按键事件绑定

### 消息处理的大致流程

### "add to root"  
了解UObject类

### void UAttributesui::SetDecalVisible()

### MessageManger::PushMessage(MessageType_POST_ASSETS_LIST, archive);主线程的消息存储到mainSendMessage_Map


### UMaterialTextObject类

###　深入材质选择改变武器样式的过程

### 接收网格列表，以及对应的拷贝网格

### UMaterialInstanceDynamic 
运行时动态修改材质参数需要


### receiveMaterialInstance  UDecalReceiveMeshCompment  UMeshComponent UMaterialInstanceDynamic 

### receiveMesh 是作为过度吗？


```C++
UMaterialInstanceDynamic* MaterialInstance = dynamic_cast<UMaterialInstanceDynamic*>(ADecalManager::selfInstance->GetCurrentMesh(1)->GetReceiveDecalMesh()->GetMaterial(0));
receiveMaterialInstance = UMaterialInstanceDynamic::Create(receiveMesh->GetMaterial(0), this);
```

