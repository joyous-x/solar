import MaSpmToolCore

@main
public struct MaSpmTool {
    public private(set) var text = "Hello, World!"

    public static func main() {
        // print(MaSpmTool().text)
        do {
            try MaSpaToolEntry().run()
        } catch {
            print("oops! An error occurred: \(error)")
        }
        
    }
}

