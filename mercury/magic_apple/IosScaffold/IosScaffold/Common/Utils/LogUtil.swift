import Foundation
import CocoaLumberjack

open class LogUtil{
    private static let logRootDir : String = NSHomeDirectory() + "/Library/Application Support/log/"
    public static func initLog(enable: Bool, enableLogfile: Bool) {
        if enable {
            DDOSLogger.sharedInstance.logFormatter = LogFormatter.sharedInstance
            DDLog.add(DDOSLogger.sharedInstance, with: .all) // Uses os_log
            if enableLogfile {
                print(logRootDir)
                let logFileManager = DDLogFileManagerDefault(logsDirectory: LogUtil.logRootDir)
                let fileLogger: DDFileLogger = DDFileLogger(logFileManager: logFileManager) // File Logger
                fileLogger.rollingFrequency = 60 * 60 * 24 // 24 hours
                fileLogger.logFileManager.maximumNumberOfLogFiles = 7
                DDLog.add(fileLogger)
            }
        }
    }
    public static func d(_ tag:String, _ msg:String) {
        DDLogDebug(msg,tag: tag)
    }
    
    public static func i(_ tag:String, _ msg:String) {
        DDLogInfo(msg,tag: tag)
    }
    
    public static func w(_ tag:String, _ msg:String) {
        DDLogWarn(msg,tag: tag)
    }
    
    public static func v(_ tag:String, _ msg:String) {
        DDLogVerbose(msg,tag: tag)
    }
    
    public static func e(_ tag:String, _ msg:String) {
        DDLogError(msg,tag: tag)
    }
}

private class LogFormatter: NSObject, DDLogFormatter {
    static let sharedInstance = LogFormatter()
    func format(message logMessage: DDLogMessage) -> String? {
        return "[\(logMessage.tag!)]:\(logMessage.message)"
    }
}
