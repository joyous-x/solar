
import Foundation
import UIKit

extension UIDevice {

    func getTotalSize(totalBytes: Int64) -> Int64 {
        let GB: Int64 = 1024 * 1024 * 1024
        var curValue = 8 * GB
        while (totalBytes > curValue) {
            curValue *= 2
        }
        return curValue
    }
    
    func totalDiskSpaceInBytes() -> Int64 {
        do {
            guard let totalDiskSpaceInBytes = try FileManager.default.attributesOfFileSystem(forPath: NSHomeDirectory())[FileAttributeKey.systemSize] as? Int64 else {
                return 0
            }
            
            return getTotalSize(totalBytes: totalDiskSpaceInBytes)
        } catch {
            return 0
        }
    }

    func freeDiskSpaceInBytes() -> Int64 {
        do {
            guard let totalDiskSpaceInBytes = try FileManager.default.attributesOfFileSystem(forPath: NSHomeDirectory())[FileAttributeKey.systemFreeSize] as? Int64 else {
                return 0
            }
            return totalDiskSpaceInBytes
        } catch {
            return 0
        }
    }

    func usedDiskSpaceInBytes() -> Int64 {
        return totalDiskSpaceInBytes() - freeDiskSpaceInBytes()
    }

    func totalDiskSpace() -> String {
        let diskSpaceInBytes = totalDiskSpaceInBytes()
        if diskSpaceInBytes > 0 {
            return ByteCountFormatter.string(fromByteCount: diskSpaceInBytes, countStyle: ByteCountFormatter.CountStyle.binary)
        }
        return "unknown"
    }

    func freeDiskSpace() -> String {
        let freeSpaceInBytes = freeDiskSpaceInBytes()
        if freeSpaceInBytes > 0 {
            return ByteCountFormatter.string(fromByteCount: freeSpaceInBytes, countStyle: ByteCountFormatter.CountStyle.binary)
        }
        return "unknown"
    }
    
    func freeDiskSpaceInfo() -> Array<String> {
        var arr: [String] = ["0", "GB"]
        let freeSpaceInBytes = freeDiskSpaceInBytes()
        if freeSpaceInBytes > 0 {
            let spaceStr = ByteCountFormatter.string(fromByteCount: freeSpaceInBytes, countStyle: ByteCountFormatter.CountStyle.binary)
            let unitStartIndex = spaceStr.count - 2 - 1
            arr[0] = String(spaceStr[ ...spaceStr.index(spaceStr.startIndex, offsetBy: unitStartIndex - 1)])
            arr[1] = String(spaceStr[spaceStr.index(spaceStr.startIndex, offsetBy: unitStartIndex) ..< spaceStr.endIndex])
        }
        return arr
    }

    func usedDiskSpace() -> String {
        let usedSpaceInBytes = totalDiskSpaceInBytes() - freeDiskSpaceInBytes()
        if usedSpaceInBytes > 0 {
            return ByteCountFormatter.string(fromByteCount: usedSpaceInBytes, countStyle: ByteCountFormatter.CountStyle.binary)
        }
        return "unknown"
    }

}
