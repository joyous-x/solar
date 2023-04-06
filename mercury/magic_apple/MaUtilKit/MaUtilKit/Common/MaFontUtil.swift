import Foundation
import UIKit

class MaFontUtil {
    func getAllSystemFonts() -> [String] {
        var result: [String] = []
        UIFont.familyNames.map { UIFont.fontNames(forFamilyName: $0) }.forEach { (fonts:[String]) in
            fonts.forEach({ result.append($0) })
        }
        return result
    }
}
