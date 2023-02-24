# IosScaffold



## Swift Packages
可以包含源码(```.target```)以及二进制库(```.binaryTarget```)

可以指定静态或动态依赖

通过 Swift Package Manager 可以支持，从 github 添加依赖，以及，将自己制作的 package 推送到 github

## Framework

### 编译
1. 项目导航栏中 -> 选中框架项目
2. 选中```TARGETS```中的目标 -> ```Build Settings``` 
3. 选中```Build Options``` -> ```Build Libraries For Distribution```

这将产生一个模块接口文件，当有人跳转到Xcode中的模块定义时，该文件将显示公共API 。

### 框架被其他项目引入 
1. 在项目导航栏中 -> 选中目标项目
2. 选中```TARGETS```中的目标 -> ```General```
    a. 选中```Frameworks，Libraries，and Embedded Content```
    b. 将依赖的 framework 文件拖动到此处
    c. 此时，应用程序就知道依赖的框架以及在何处找到它
3. 选中```TARGETS```中的目标 -> ```Build Phases```
    a. 选中```Link Binary With Libraries```
    b. 点击```+```添加依赖
    c. 这一步的用途：？？？

### 将框架项目添加到应用程序项目中(表现为：导航栏中的项目层级目录结构)
1. 右键单击项目导航栏中的根节点(一般就是应用程序项目节点) -> 单击```Add Files to "IosScaffold"```
2. 在文件选择器中，导航到目标 Framework 的 ```*.xcodeproj``` 文件
3. 单击"Add"将其添加为子项目

注意：并非必须将框架项目添加到应用程序项目中。也可以只添加框架项目的输出(```*.framework```文件)为依赖即可。

但是，将项目组合在一起可以更轻松地同时开发框架和应用程序。对框架项目所做的任何更改都会自动传播到应用程序。

## XCFramework
可以将多个 framework 打包为二进制框架 XCFramework。使用时，XCFramework 可以像 framework 一样被项目依赖。

并且，可以轻松地在 Swift Packages 中分发 XCFramework

## Reference
- [翻译：Swift 5创建和使用Framework, XCFramework 从入门到精通](https://blog.csdn.net/zgpeace/article/details/115057441)
- [creating_a_standalone_swift_package_with_xcode](https://developer.apple.com/documentation/xcode/creating_a_standalone_swift_package_with_xcode)
- [Swift Package Manager工程实践](https://maimai.cn/article/detail?fid=1758568260&efid=LnLf4PsHKi00KjtDZ9XbuA)
