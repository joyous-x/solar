import Foundation
import UIKit

class UserInterfaceStyleProvider {
    public static func getCurrentStyle() -> UIUserInterfaceStyle {
        let val = LocalStorage.shared().getInt32(key: SpKey.SP_KEY_APP_THEME, defValue: Int32(UIUserInterfaceStyle.unspecified.rawValue))
        return UIUserInterfaceStyle(rawValue: Int(val)) ?? .unspecified
    }

    public static func setCurrentStyle(_ style: UIUserInterfaceStyle, apply: Bool = false) {
        LocalStorage.shared().putInt32(key: SpKey.SP_KEY_APP_THEME, value: Int32(style.rawValue))

        if apply {
            _applyStyle(style)
        }
    }

    public static func applyCurrentStyle() {
        let style = Self.getCurrentStyle()

        _applyStyle(style)
    }

    private static func _applyStyle(_ style: UIUserInterfaceStyle) {
        guard let sceneDelegate = UIApplication.shared.connectedScenes.first?.delegate as? SceneDelegate else {
            return
        }
        sceneDelegate.window?.overrideUserInterfaceStyle = style
    }
}
