# MaSpmTool

## SPM 简介
Swift Package Manager（简称SPM）是苹果在2018年推出的供Swift开发者进行包管理的工具。类似于常用的Cocoapods或Carthage。

有以下优点：
1. 导入第三方库非常简单
    + 有可视化导入界面
2. 定义文件简单
    + 只需将源码放入对应的文件夹内即可，Xcode就会自动生成工程文件，并生成编译目标产物所需要的相关配置
3. 侵入性低
    + SPM 是通过 library 或 workspace 方式集成的，侵入性非常低
4. 多平台编译的能力非常好
    + UIKit 一次编写即可适配 iOS/iPadOS/tvOS/watchOS 多个平台
5. SPM 与 Cocapods兼容，可以一起使用


## 使用 SPM 搭建开发框架
先初始化项目(注意，可以生成 executable 或 library，这里生成为命令行可执行文件)：
```
$ mkdir MaSpmTool
$ cd MaSpmTool
$ swift package init --type executable
```
这样就可以生成 SPM 项目的基础文件结构，以及 .gitignore 文件(其中已经包含必要的内容)。

其次，一般会保证 Sources 目录下有两个文件夹，一个用于 executable 相关，一个用于 framework 相关，就像下面一样：
```
$ cd Sources 
$ mkdir MaSpmToolCore
```

这里值得说明一下 SPM 的一个非常好的特性：它使用文件系统作为它的处理依据，也就是说，只要采用上述操作提供的文件结构，就等于定义了两个模块

紧接着，我们在 Package.swift 里定义两个target，也就是 MaSpmTool 和 MaSpmToolCore：
```
let package = Package(
    ...
    
    targets: [
        .executableTarget(
            name: "MaSpmTool",
            dependencies: ["MaSpmToolCore"]),
        .testTarget(
            name: "MaSpmToolTests",
            dependencies: ["MaSpmTool"]),
        .target(
            name: "MaSpmToolCore",
            dependencies: []),
    ]
)
```
通过上面这种方式，我们让 executable 模块依赖了 framework 模块。

再者，如果我们声明了新的依赖关系，只需要求 SPM 解析新的依赖关系并安装它们，然后重新生成 Xcode 项目即可(在新版 xcode 中这一步可以省掉)。
```
$ swift package update
$ swift package generate-xcodeproj
```

最后，编译运行：
```
$ swift build -c debug
$ swift run
```
其中，```-c```参数是可选的，最后生成的文件位于：```./build/debug```目录下

> 使用```swift package tools-version```可以获得 SPM 编译版本

