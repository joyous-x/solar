import Foundation
import UIKit

// 小组件的大小
enum DeviceWidgetSize {
    static var small: CGSize {
        switch UIDevice().type {
        case .iPhone12ProMax:
            return CGSize(width: 170, height: 170)
        case .iPhone12Pro, .iPhone12:
            return CGSize(width: 158, height: 158)
        case .iPhone11ProMax, .iPhone11, .iPhoneXSMax, .iPhoneXR:
            return CGSize(width: 169, height: 169)
        case .iPhone12Mini, .iPhone11Pro, .iPhoneXS, .iPhoneX:
            return CGSize(width: 155, height: 155)
        case .iPhone6SPlus, .iPhone7Plus, .iPhone8Plus:
            return CGSize(width: 159, height: 159)
        case .iPhone6S, .iPhone7, .iPhone8, .iPhoneSE2:
            return CGSize(width: 148, height: 148)
        case .iPhoneSE, .iPod7:
            return CGSize(width: 141, height: 144)
        default:
            return CGSize(width: 169, height: 169)
        }
    }

    static var meduim: CGSize {
        switch UIDevice().type {
        case .iPhone12ProMax:
            return CGSize(width: 364, height: 170)
        case .iPhone12Pro, .iPhone12:
            return CGSize(width: 338, height: 158)
        case .iPhone11ProMax, .iPhone11, .iPhoneXSMax, .iPhoneXR:
            return CGSize(width: 360, height: 169)
        case .iPhone12Mini, .iPhone11Pro, .iPhoneXS, .iPhoneX:
            return CGSize(width: 329, height: 155)
        case .iPhone6SPlus, .iPhone7Plus, .iPhone8Plus:
            return CGSize(width: 348, height: 159)
        case .iPhone6S, .iPhone7, .iPhone8, .iPhoneSE2:
            return CGSize(width: 322, height: 148)
        case .iPhoneSE, .iPod7:
            return CGSize(width: 291, height: 144)
        default:
            return CGSize(width: 360, height: 169)
        }
    }
}
