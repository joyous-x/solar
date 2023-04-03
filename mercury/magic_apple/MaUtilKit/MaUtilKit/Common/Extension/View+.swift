import Foundation
import SwiftUI

public extension View {
    // 自定义一个函数利用 overlay 产生 mask
    func overlayMask<T: View>(_ overlay: T) -> some View {
        self.overlay(overlay).mask(self)
    }
}
