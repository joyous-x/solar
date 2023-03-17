import Foundation
import MMKV

public class LocalStorage {
    
    private var mmKV : MMKV!
    private var section = ""
    
    private func getKeyName(_ key:String) -> String {
        return "\(self.section)_\(key)"
    }
    
    public func putString(key:String,value:String) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getString(key:String,defValue:String) -> String {
        return mmKV?.string(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func putBool(key:String,value:Bool) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getBool(key:String,defValue:Bool) -> Bool {
        return mmKV?.bool(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func putInt32(key:String,value:Int32) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getInt32(key:String,defValue:Int32) -> Int32 {
        return mmKV?.int32(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func putInt64(key:String,value:Int64) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getInt64(key:String,defValue:Int64) -> Int64 {
        return mmKV?.int64(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func putFloat(key:String,value:Float) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getFloat(key:String,defValue:Float) -> Float {
        return mmKV?.float(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func putDouble(key:String,value:Double) {
        mmKV?.set(value, forKey: getKeyName(key))
    }
    
    public func getDouble(key:String,defValue:Double) -> Double {
        return mmKV?.double(forKey: getKeyName(key), defaultValue: defValue) ?? defValue
    }
    
    public func remove(key:String) {
        mmKV?.removeValue(forKey: getKeyName(key))
    }
    
    private static let instance = {
        return LocalStorage(section: "mega_widget_storage")
    }()
    
    init(section: String) {
        // 目前默认是不支持多进程的
        let processName = ProcessInfo.processInfo.processName
        let processId = ProcessInfo.processInfo.processIdentifier
        print("processName: \(processName) -- processId: \(processId)")
        self.section = section
        var root = ""
        if processName == "megawidget" {
            root = "\(NSHomeDirectory())/Documents/mmkv"
            FileManager.creatDir(root)
        } else {
            root = SharedFolderUtil.makeShareFolderExists(groupId: SharedFolderUtil.widgetGroupId, folderName: "mmkv")?.path ?? ""
        }
        self.mmKV = MMKV(mmapID: "\(processName).mega.mmkv", cryptKey: nil, rootPath: root)
    }
    
    @discardableResult
    public class func shared() -> LocalStorage {
        return instance
    }
    
    public static func initFirst() {
        shared()
    }
    
    public func clearAllKey() {
        mmKV.clearMemoryCache()
        mmKV.clearAll()
    }
}
