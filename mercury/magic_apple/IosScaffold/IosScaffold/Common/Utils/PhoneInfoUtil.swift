import Foundation
import UIKit
import MediaPlayer
import CoreTelephony

public class PhoneInfoUtil: NSObject {
    
    public override init() {
        super.init()
    }
    
    // 获取电池电量 0-100
    static func getBatteryLevel() -> Int {
        return Int((getBatteryLevel0_1() * 100 + 0.5))
    }
    
    // 获取电池电量 0-1
    static func getBatteryLevel0_1() -> Float {
        UIDevice.current.isBatteryMonitoringEnabled = true
        return UIDevice.current.batteryLevel
    }
    
    static func getTotalDiskSize() -> String {
        return UIDevice.current.totalDiskSpace()
    }
    
    static func getFreeDiskSize() -> String {
        return UIDevice.current.freeDiskSpace()
    }
    
    static func getFreePercent() -> Int {
        return Int(Float(UIDevice.current.freeDiskSpaceInBytes()) / Float(UIDevice.current.totalDiskSpaceInBytes()) * 100 + 0.5)
    }
    
    // 获取屏幕亮度（0-1）
    static func getScreenBrightness() -> Float {
        return Float(UIScreen.main.brightness)
    }
    
    // 获取系统音量（0-1）
    static func getSysVolum() -> Float {
        try? AVAudioSession.sharedInstance().setActive(true)
        return AVAudioSession.sharedInstance().outputVolume
    }
  
    ///< 暂时未验证过此方法，先注释掉
//    static func isAudioPlaying() -> Bool {
//        return AVAudioSession.sharedInstance().secondaryAudioShouldBeSilencedHint
//    }
    
    static func isMobileEnable() -> Bool {
        let reach = try? Reachability();
        if (reach == nil) {
            return false
        }
        if (!reach!.isReachable) {
            return false
        }
        let state = reach?.connection;
        print("xjp mobile sate = ", state);
        return reach!.isReachableViaWWAN || reach!.isReachableViaWiFi
    }
    
    static func isMobileEnableEx() -> Bool {
        let reach = try? Reachability();
        if (reach == nil) {
            return false
        }
        if (!reach!.isReachable) {
            return false
        }
        let state = reach?.connection;
        print("xjp mobile sate = ", state);
        return reach!.isReachableViaWWAN
    }
    
    static func isWifiEnable() -> Bool {
        let reach = try? Reachability();
        if (reach == nil) {
            return false
        }
        if (!reach!.isReachable) {
            return false
        }
        let state = reach?.connection;
        print("xjp wifi sate = ", state);
        
        return reach!.isReachableViaWiFi
    }
    
    public static func isFlightMode() -> Bool {
        let net = CTTelephonyNetworkInfo()
        let netType = net.currentRadioAccessTechnology;
        return netType == nil
    }
    
    static public func getDeviceName() -> String {
        return UIDevice.current.name
    }
    
    static public func getDeviceSystemName() -> String {
        return UIDevice.current.systemName
    }
    
    static public func getSystemVersion() -> String {
        return UIDevice.current.systemVersion
    }
    
    ///https://www.theiphonewiki.com/wiki/Models
    static public func modelName() -> String {
        var systemInfo = utsname()
        uname(&systemInfo)
        let machineMirror = Mirror(reflecting: systemInfo.machine)
        let identifier = machineMirror.children.reduce("") { identifier, element in
            guard let value = element.value as? Int8, value != 0 else { return identifier }
            return identifier + String(UnicodeScalar(UInt8(value)))
        }
                
        switch identifier {
            case "iPhone3,1", "iPhone3,2", "iPhone3,3":  return "iPhone 4"
            case "iPhone4,1":                            return "iPhone 4s"
            case "iPhone5,1":                            return "iPhone 5"
            case "iPhone5,2":                            return "iPhone 5"
            case "iPhone5,3", "iPhone5,4":               return "iPhone 5c"
            case "iPhone6,1", "iPhone6,2":               return "iPhone 5s"
            case "iPhone7,2":                            return "iPhone 6"
            case "iPhone7,1":                            return "iPhone 6 Plus"
            case "iPhone8,1":                            return "iPhone 6s"
            case "iPhone8,2":                            return "iPhone 6s Plus"
            case "iPhone8,4":                            return "iPhone SE"
            case "iPhone9,1", "iPhone9,3":               return "iPhone 7"
            case "iPhone9,2", "iPhone9,4":               return "iPhone 7 Plus"
            
            case "iPhone10,1", "iPhone10,4":             return "iPhone 8"
            case "iPhone10,2", "iPhone10,5":             return "iPhone 8 Plus"
            case "iPhone10,3", "iPhone10,6":             return "iPhone X"

            case "iPhone11,8":                           return "iPhone XR"
            case "iPhone11,2":                           return "iPhone XS"
            case "iPhone11,6", "iPhone11,4":             return "iPhone XS Max"

            case "iPhone12,1":                           return "iPhone 11"
            case "iPhone12,3":                           return "iPhone 11 Pro"
            case "iPhone12,5":                           return "iPhone 11 Pro Max"
            case "iPhone12,8":                           return "iPhone SE"

            case "iPhone13,1":                           return "iPhone 12 mini"
            case "iPhone13,2":                           return "iPhone 12"
            case "iPhone13,3":                           return "iPhone 12 Pro"
            case "iPhone13,4":                           return "iPhone 12 Pro Max"

            case "iPhone14,4":                           return "iPhone 13 mini"
            case "iPhone14,5":                           return "iPhone 13"
            case "iPhone14,2":                           return "iPhone 13 Pro"
            case "iPhone14,3":                           return "iPhone 13 Pro Max"
            case "iPhone14,6":                           return "iPhone SE"
            
            default:                                     return "iPhone"
      }
    }
}
