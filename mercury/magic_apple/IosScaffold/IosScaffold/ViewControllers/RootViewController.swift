import UIKit

class RootViewController: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()

        self.view.backgroundColor = UIColor.white
        
        if needShowGuidePage() {
            showGuidePage()
        }
        else {
            showMainPage()
        }
    }

    private func showMainPage() {
        self.addChildController(MainPageController())
    }

    private func showGuidePage() {
        let guideController = GuidePageController { [unowned self] in
            self.showMainPage()
        }
        self.addChildController(guideController)
    }
    
    private func needShowGuidePage() -> Bool {
        return LocalStorage.shared().getBool(key: SpKey.SP_KEY_NEW_GUIDE_SWITCH, defValue: true)
    }
}

extension RootViewController {
    func addChildController(_ child: UIViewController) {
        addChild(child)
        view.addSubview(child.view)
        child.didMove(toParent: self)
    }
    
    func removeFromParentController() {
        willMove(toParent: nil)
        view.removeFromSuperview()
        removeFromParent()
    }
}
