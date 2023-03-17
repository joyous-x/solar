import Foundation

public final class MaSpaToolEntry {
    private let arguments: [String]

    public init(arguments: [String] = CommandLine.arguments) {
        self.arguments = arguments
    }

    public func run() throws {
        let val = self.arguments.count > 0 ? self.arguments.joined(separator: " ") : "Hello world"
        print("\(val)")
    }
}
