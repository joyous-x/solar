import Foundation

public extension Date {
    /// 获取当前 秒级 时间戳 - 10位
    var timestamp : Int64 {
        let timeInterval: TimeInterval = self.timeIntervalSince1970
        return Int64(timeInterval)
    }
    
    //获取当天开始的日期
    func getDayStart(_ isDayOf24Hours: Bool)-> Date {
        let calendar:Calendar = Calendar.current
        let year = calendar.component(.year, from: self)
        let month = calendar.component(.month, from: self)
        let day = calendar.component(.day, from: self)
        let hour = calendar.component(.hour, from: self)
        let startHour = isDayOf24Hours || hour <= 12 ? 0 : 12

        let components = DateComponents(year: year, month: month, day: day, hour: startHour, minute: 0, second: 0)
        return Calendar.current.date(from: components)!
    }
}


