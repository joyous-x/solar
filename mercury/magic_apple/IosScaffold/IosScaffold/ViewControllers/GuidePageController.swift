import Foundation
import UIKit

class GuidePageController: UIViewController {
    // 引导完成的回调
    private var completion: () -> Void
    
    init(_ completion: @escaping () -> Void) {
        self.completion = completion
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    deinit {
        print("[GuidePageController] deinit")
    }
    
}
