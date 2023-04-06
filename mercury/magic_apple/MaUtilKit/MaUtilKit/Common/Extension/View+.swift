import Foundation
import SwiftUI

public extension View {
    // 自定义一个函数利用 overlay 产生 mask
    func overlayMask<T: View>(_ overlay: T) -> some View {
        self.overlay(overlay).mask(self)
    }
}

public extension Text {
    /*
     * 最初目的：剪裁小组件电子时钟(00:00:00)的最后两位
     *      Text(date.getDayStart(false), style: .timer).clipTriling(....)
     */
    func clipTrailing(_ widthKeeped: CGFloat, fullWidthEstamited: CGFloat) -> some View {
        return HStack {
            self.tracking(0)
                .lineLimit(1)
                .multilineTextAlignment(.trailing)
                .frame(width: fullWidthEstamited, alignment: .trailing)
        }.frame(width: widthKeeped, alignment: .trailing).clipped()
    }
}
