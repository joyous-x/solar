import Foundation

public extension Date {
    /// 获取当前 秒级 时间戳 - 10位
    var timestamp : Int64 {
        let timeInterval: TimeInterval = self.timeIntervalSince1970
        return Int64(timeInterval)
    }
}


