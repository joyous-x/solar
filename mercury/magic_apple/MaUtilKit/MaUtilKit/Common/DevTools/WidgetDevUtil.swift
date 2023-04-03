import Foundation
import SwiftUI

class WidgetDevUtil {

    /**
     * 用于辅助透明背景配置：计算组件位置、大小
     */
    func buildGeometryReaderResultView(_ gr : GeometryProxy) -> some View {
        return VStack {
            // 因为是gr,所以是最外面容器的width和height
            Text("size->width:\(gr.size.width * UIScreen.main.scale)").font(.system(size: 10))
            Text("size->height:\(gr.size.height * UIScreen.main.scale)").font(.system(size: 10))

            // 当容器设置的宽度和高度均未超过屏幕, 有如下内容；若设置超过的情况下, 下面的计算方式就不适用！

            // gr.frame(in: .global).width  代表容器本身的宽度
            Text("global->width:\(Int(gr.frame(in: .global).width * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).height 代表容器本身的高度
            Text("global->height:\(Int(gr.frame(in: .global).height * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).minX   代表容器最左侧距离屏幕左侧的距离
            Text("global->minX:\(Int(gr.frame(in: .global).minX * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).maxX   代表容器最右侧距离屏幕左侧的距离(width + minX)
            Text("global->maxX:\(Int(gr.frame(in: .global).maxX * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).midX   代表容器中间距离屏幕左侧的距离(width/2 + minX)
            Text("global->midX:\(Int(gr.frame(in: .global).midX * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).minY   代表容器顶部距离屏幕顶部的距离
            Text("global->minY:\(Int(gr.frame(in: .global).minY * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).maxY   代表容器底部距离屏幕顶部的距离(height + minY)
            Text("global->maxY:\(Int(gr.frame(in: .global).maxY * UIScreen.main.scale))").font(.system(size: 10))
            // gr.frame(in: .global).midY   代表容器中间距离屏幕顶部的距离(height/2 + minX)
            Text("global->midY:\(Int(gr.frame(in: .global).midY * UIScreen.main.scale))").font(.system(size: 10))

        }.frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .leading)
    }

}
