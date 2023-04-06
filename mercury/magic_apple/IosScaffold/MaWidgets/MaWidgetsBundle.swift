import WidgetKit
import SwiftUI

@main
struct MaWidgetsBundle: WidgetBundle {
    var body: some Widget {
        // MaWidgetsLiveActivity()

        MaWidgetsSmall()
        MaWidgetsMedium()
        MaWidgetsLarge()
        if #available(iOSApplicationExtension 16.1, *) {
            MaLockWidgetsBundle().body
        }
    }
}

@available(iOSApplicationExtension 16.1, *)
struct MaLockWidgetsBundle: WidgetBundle {
    @WidgetBundleBuilder
    var body: some Widget {
        MaWidgetsCircular()
        MaWidgetsRectangular()
    }
}

struct MaWidgetsSmall: Widget {
    let kind: String = "MaWidgetsSmall"
    
    var body: some WidgetConfiguration {
        IntentConfiguration(kind: kind, intent: MaSmallConfigurationIntent.self, provider: MaSmallProvider()) { entry in
            MaWidgetsEntryView(entry: entry)
        }
        .supportedFamilies([.systemSmall])
        .configurationDisplayName("小号组件")
        .description("添加至桌面后长按小组件进行编辑")
    }
}

struct MaWidgetsMedium: Widget {
    let kind: String = "MaWidgetsMedium"
    
    var body: some WidgetConfiguration {
        IntentConfiguration(kind: kind, intent: MaMediumConfigurationIntent.self, provider: MaMediumProvider()) { entry in
            MaWidgetsEntryView(entry: entry)
        }
        .supportedFamilies([.systemMedium])
        .configurationDisplayName("中号组件")
        .description("添加至桌面后长按小组件进行编辑")
    }
}

struct MaWidgetsLarge: Widget {
    let kind: String = "MaWidgetsLarge"
    
    var body: some WidgetConfiguration {
        IntentConfiguration(kind: kind, intent: MaLargeConfigurationIntent.self, provider: MaLargeProvider()) { entry in
            MaWidgetsEntryView(entry: entry)
        }
        .supportedFamilies([.systemLarge])
        .configurationDisplayName("大号组件")
        .description("添加至桌面后长按小组件进行编辑")
    }
}

@available(iOSApplicationExtension 16.1, *)
struct MaWidgetsCircular: Widget {
    let kind: String = "MaWidgetsCircular"
    
    var body: some WidgetConfiguration {
        IntentConfiguration(kind: kind, intent: MaCircularConfigurationIntent.self, provider: MaCircularProvider()) { entry in
            MaWidgetsEntryView(entry: entry)
        }
        .supportedFamilies([.accessoryCircular])
        .configurationDisplayName("圆形小组件")
        .description("添加后，请再次点击进入配置列表")
    }
}

@available(iOSApplicationExtension 16.1, *)
struct MaWidgetsRectangular: Widget {
    let kind: String = "MaWidgetsRectangular"
    
    var body: some WidgetConfiguration {
        IntentConfiguration(kind: kind, intent: MaRectangularConfigurationIntent.self, provider: MaRectangularProvider()) { entry in
            MaWidgetsEntryView(entry: entry)
        }
        .supportedFamilies([.accessoryRectangular])
        .configurationDisplayName("矩形小组件")
        .description("添加后，请再次点击进入配置列表")
    }
}
