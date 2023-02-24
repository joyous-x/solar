import Foundation
import UIKit
import QuartzCore


/**
 https://github.com/yehot/YYFPSLabel
    https://blog.ibireme.com/2015/05/18/runloop/
 */
class FpsView {
    var _count : Int = 0
    var _lastime: TimeInterval = 0.0
    var _llll: TimeInterval = 0.0

    lazy var _link : CADisplayLink = {
        CADisplayLink(target: self, selector: #selector(tick))
    }()
    
    init(count: Int) {
        _count = count
        _link.add(to: .main, forMode: .common)
    }

    func dealloc() {
        _link.invalidate()
    }

    @objc func tick(link: CADisplayLink) {
        if _lastime == 0 {
            _lastime = link.timestamp
            return
        }

        _count += 1

        let delta = link.timestamp - _lastime
        if delta < 1 {
            return
        }
        _lastime = link.timestamp

        let fps = Double(_count) / delta
        _count = 0

        // let progress = fps / 60.0
        // makeProgressColor(progress)
    }

    func makeProgressColor(_ progress: CGFloat) -> UIColor {
        return UIColor.init(hue: 0.27 * (progress - 0.2), saturation: 1, brightness: 0.9, alpha: 1)
    }
}
