import Foundation
import UIKit

class SplashPageController: UIViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        initViews()
    }
    
    private func initViews() {
        let testLabel = UILabel(frame: CGRect(origin: .zero, size: CGSize(width: 100, height: 50)))
        testLabel.text = "hello word"
        testLabel.backgroundColor = .red
        testLabel.textColor = .white
        
        self.view.addSubview(testLabel)
    }
}
