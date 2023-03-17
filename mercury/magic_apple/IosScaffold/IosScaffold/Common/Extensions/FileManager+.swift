import Foundation

public extension FileManager {
    static var fileManager: FileManager {
        return FileManager.default
    }
    
    /// 拷贝文件夹
    /// - Parameters:
    /// - fpath: 原始路径，绝对路径
    /// - tpath: 目标路径，绝对路径,注意：这里tpath路径必须包含目标文件夹名称，否则拷贝不了
    static func copyFolder(fpath: String, tpath: String) -> Bool {
        do {
            if !fileManager.fileExists(atPath: fpath) {
                return false
            }
            //如果已存在，先删除，否则拷贝不了
            if fileManager.fileExists(atPath: tpath){
                try fileManager.removeItem(atPath: tpath)
            }
            try fileManager.copyItem(atPath: fpath, toPath: tpath)
            return true
        } catch {
            return false
        }
    }
    
    static func moveFolder(fpath: String, tpath: String) -> Bool {
        do {
            if !fileManager.fileExists(atPath: fpath) {
                return false
            }
            if fileManager.fileExists(atPath: tpath){
                try fileManager.removeItem(atPath: tpath)
            }
            try fileManager.moveItem(atPath: fpath, toPath: tpath)
            return true
        } catch {
            return false
        }
    }
    
    // 删除目标文件夹下所有的内容
    @discardableResult
    static func removeFolder(_ folderUrl: String) -> Bool{
        // 然后获得所有该目录下的子文件夹
        let files:[AnyObject]? = fileManager.subpaths(atPath: folderUrl)! as [AnyObject]
        // 创建一个循环语句，用来遍历所有子目录
        var result = true
        for file in files! {
            do {
                // 删除指定位置的内容
                try fileManager.removeItem(atPath: folderUrl + "/\(file)")
            } catch{
                result = false
            }
        }
        return result
    }

    @discardableResult
    static func getFolderSize(_ folderUrl: String) -> Double {
        // 文件夹下所有文件
        let files = fileManager.subpaths(atPath: folderUrl)!
        // 遍历计算大小
        var size = 0
        for file in files {
            // 文件名拼接到路径中
            let path = folderUrl + "/\(file)"
            // 取出文件属性
            do {
                let floder = try fileManager.attributesOfItem(atPath: path)
                for (key, fileSize) in floder {
                    // 累加
                    if key == FileAttributeKey.size {
                        size += (fileSize as AnyObject).integerValue
                    }
                }
            } catch {
                print("出错了！")
            }
        }
        return Double(size) / 1024.0 / 1024.0
    }
    
    @discardableResult
    static func creatDir(_ dirPath: String) -> Bool {
        let widgetDirExist = fileManager.fileExists(atPath: dirPath)
        if !widgetDirExist {
            do {
                try fileManager.createDirectory(atPath: dirPath, withIntermediateDirectories: true, attributes: nil)
                return true
            } catch {
                return false
            }
        }
        return true
    }
    
    static func deleteFile(_ atPath: String) -> Bool {
        if judgeFileOrFolderExists(filePath: atPath) {
            do {
                try fileManager.removeItem(atPath: atPath)
                return true
            } catch {}
        }
        return false
    }
    
    // MARK: 2.10、判断 (文件夹/文件) 是否存在
    /** 判断文件或文件夹是否存在*/
    static func judgeFileOrFolderExists(filePath: String) -> Bool {
        let exist = fileManager.fileExists(atPath: filePath)
        // 查看文件夹是否存在，如果存在就直接读取，不存在就直接反空
        guard exist else {
            return false
        }
        return true
    }
    
    // MARK: 2.11、获取 (文件夹/文件) 的前一个路径
    /// 获取 (文件夹/文件) 的前一个路径
    /// - Parameter path: (文件夹/文件) 的路径
    /// - Returns: (文件夹/文件) 的前一个路径
    static func parentPath(path: String) -> String {
        return (path as NSString).deletingLastPathComponent
    }
    
    // MARK: 2.12、判断目录是否可读
    static func judegeIsReadableFile(path: String) -> Bool {
        return fileManager.isReadableFile(atPath: path)
    }
    
    // MARK: 2.13、判断目录是否可写
    static func judegeIsWritableFile(path: String) -> Bool {
        return fileManager.isReadableFile(atPath: path)
    }
    
    // MARK: 2.14、根据文件路径获取文件扩展类型
    /// 根据文件路径获取文件扩展类型
    /// - Parameter path: 文件路径
    /// - Returns: 文件扩展类型
    static func fileSuffixAtPath(path: String) -> String {
        return (path as NSString).pathExtension
    }
    
    // MARK: 根据文件路径获取文件名称，是否需要后缀
    /// 根据文件路径获取文件名称，是否需要后缀
    /// - Parameters:
    ///   - path: 文件路径
    ///   - suffix: 是否需要后缀，默认需要
    /// - Returns: 文件名称
    static func fileName(path: String, suffix: Bool = true) -> String {
        let fileName = (path as NSString).lastPathComponent
        guard suffix else {
            // 删除后缀
            return (fileName as NSString).deletingPathExtension
        }
        return fileName
    }
    
    // MARK: 获取子文件列表（第一层目录）
    static func getSubListFilePath(_ dirPath: String, _ includeDir: Bool) -> [String] {
        var filePaths = [String]()
        do {
            let array = try fileManager.contentsOfDirectory(atPath: dirPath)
            for fileName in array {
                var isDir: ObjCBool = true
                let fullPath = "\(dirPath)/\(fileName)"
                if !fileManager.fileExists(atPath: fullPath, isDirectory: &isDir) {
                    continue
                }
                if isDir.boolValue  {
                    if includeDir {
                        filePaths.append(fullPath)
                    }
                } else {
                    filePaths.append(fullPath)
                }
            }
        } catch let error as NSError {
            print("get file path error: \(error)")
        }
        return filePaths;
    }
    
    // MARK: 2.5、读取文件内容
    /// 读取文件内容
    /// - Parameter filePath: 文件路径
    /// - Returns: 文件内容
    @discardableResult
    static func readfile(filePath: String) -> String? {
        guard judgeFileOrFolderExists(filePath: filePath) else {
            // 不存在的文件路径就不需要要移除
            return nil
        }
        let data = fileManager.contents(atPath: filePath)
        return String(data: data!, encoding: String.Encoding.utf8)
    }
}
