import Foundation


class SharedFolderUtil {
    private init() {}
    
    static let widgetGroupId = "WidgetConfigUtil.groupId.____"
    
    /*
     * 创建并返回目录路径URL
     */
    static public func makeShareFolderExists(groupId: String, folderName: String) -> URL? {
        let documentsDirectory = FileManager().containerURL(forSecurityApplicationGroupIdentifier: groupId)
        guard let folderURL = documentsDirectory?.appendingPathComponent(folderName) else { return nil}
        
        var isDir : ObjCBool = false
        var isExists = FileManager.default.fileExists(atPath: folderURL.path, isDirectory: &isDir)
        if isExists && !isDir.boolValue {
            do {
                try FileManager.default.removeItem(at: folderURL)
                isExists = false;
            } catch {
                return nil
            }
        }
        if !isExists {
            do {
                try FileManager.default.createDirectory(atPath: folderURL.path, withIntermediateDirectories: true, attributes: nil)
            } catch {
                return nil
            }
        }
        return folderURL
    }
}
