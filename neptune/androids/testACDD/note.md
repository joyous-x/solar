#### ACDD

> ACDD的仓库包含两个项目:
* ACDDCore: ACDD的核心库
* ACDDLauncher: 加载插件的宿主APP

> ACDDExtension的仓库包含的内容:
* 插件资源分组架构图: Architecture/images/Architecture.001.jpg
* 编译好的demo: Dist/ACDDLauncher.apk
* 插件demo: Samples
* 针对机型的测试报告: TestReport
* 编译: buildTools
* aapt: 修改过的appt, 增加对versionName的处理
* ACDDExt: 处理插件apk，生成bundle-info.json


#### NOTE：
* 需要在宿主中生成 bundle-info.json 文件
* 插件中的 Activity不能继承自 AppCompatActivity, 否则会在 setContentView(R.layout.xx) 时, 抛出异常：找不到资源
* 编译插件的gradle不能使用 2.1.0 及以上版本, 因为新版本默认提供了功能：Instant Run [File->Setting->Build,Execution,Deployment]
* 插件的 application 是被动实例化的, 启动插件中 activity，它所在的 application 就会自动被启动; 也可以通过反射主动触发
