import Foundation
import WidgetKit
import SwiftUI
import MaCommonKit
import Intents

@available(iOS 14.0, *)
struct MaWidgetsEntryView : View {
    @Environment(\.colorScheme) var colorScheme
    @Environment(\.widgetFamily) var family: WidgetFamily
    var entry: MaWidgetEntry
    
    var body: some View {
        ZStack {
            // TODO:
        }
    }
}

struct MaWidgetEntry: TimelineEntry {
    let date: Date
    let config: WidgetConfigBase
}
typealias Entry = MaWidgetEntry

class MaProvider {
    static let emptyConfig = WidgetConfigBase()
    
    static func placeholder(in context: TimelineProviderContext) -> MaWidgetEntry {
        MaWidgetEntry(date: Date(), config: emptyConfig)
    }
    
    static func getSnapshot(for configuration: INIntent, in context: TimelineProviderContext, completion: @escaping (MaWidgetEntry) -> ()) {
        completion(MaWidgetEntry(date: Date(), config: emptyConfig))
    }

    static func getTimeline(for configuration: INIntent, in context: TimelineProviderContext, completion: @escaping (Timeline<MaWidgetEntry>) -> ()) {
        // TODO:
    }
}

struct MaSmallProvider: IntentTimelineProvider {
    func placeholder(in context: Context) -> MaWidgetEntry {
        return MaProvider.placeholder(in: context)
    }

    func getSnapshot(for configuration: MaSmallConfigurationIntent, in context: Context, completion: @escaping (MaWidgetEntry) -> ()) {
        MaProvider.getSnapshot(for: configuration, in: context, completion: completion)
    }

    func getTimeline(for configuration: MaSmallConfigurationIntent, in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        MaProvider.getTimeline(for: configuration, in: context, completion: completion)
    }
}

struct MaMediumProvider: IntentTimelineProvider {
    func placeholder(in context: Context) -> MaWidgetEntry {
        return MaProvider.placeholder(in: context)
    }

    func getSnapshot(for configuration: MaMediumConfigurationIntent, in context: Context, completion: @escaping (MaWidgetEntry) -> ()) {
        MaProvider.getSnapshot(for: configuration, in: context, completion: completion)
    }

    func getTimeline(for configuration: MaMediumConfigurationIntent, in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        MaProvider.getTimeline(for: configuration, in: context, completion: completion)
    }
}

struct MaLargeProvider: IntentTimelineProvider {
    func placeholder(in context: Context) -> MaWidgetEntry {
        return MaProvider.placeholder(in: context)
    }

    func getSnapshot(for configuration: MaLargeConfigurationIntent, in context: Context, completion: @escaping (MaWidgetEntry) -> ()) {
        MaProvider.getSnapshot(for: configuration, in: context, completion: completion)
    }

    func getTimeline(for configuration: MaLargeConfigurationIntent, in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        MaProvider.getTimeline(for: configuration, in: context, completion: completion)
    }
}

struct MaCircularProvider: IntentTimelineProvider {
    func placeholder(in context: Context) -> MaWidgetEntry {
        return MaProvider.placeholder(in: context)
    }

    func getSnapshot(for configuration: MaCircularConfigurationIntent, in context: Context, completion: @escaping (MaWidgetEntry) -> ()) {
        MaProvider.getSnapshot(for: configuration, in: context, completion: completion)
    }

    func getTimeline(for configuration: MaCircularConfigurationIntent, in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        MaProvider.getTimeline(for: configuration, in: context, completion: completion)
    }
}

struct MaRectangularProvider: IntentTimelineProvider {
    func placeholder(in context: Context) -> MaWidgetEntry {
        return MaProvider.placeholder(in: context)
    }

    func getSnapshot(for configuration: MaRectangularConfigurationIntent, in context: Context, completion: @escaping (MaWidgetEntry) -> ()) {
        MaProvider.getSnapshot(for: configuration, in: context, completion: completion)
    }

    func getTimeline(for configuration: MaRectangularConfigurationIntent, in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        MaProvider.getTimeline(for: configuration, in: context, completion: completion)
    }
}
