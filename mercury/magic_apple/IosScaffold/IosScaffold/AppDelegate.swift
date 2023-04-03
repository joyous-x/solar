import UIKit
import MaUtilKit

@main
class AppDelegate: UIResponder, UIApplicationDelegate {



    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.
        LocalStorage.initFirst()       // mmkv初始化要在主线程，不然会报异常，需要最早初始化，防止其他异步线程调用到
        LogUtil.initLog(enable: true, enableLogfile: false)
        
        /// - Note: 新用户
        let installTime = LocalStorage.shared().getInt64(key: SpKey.SP_KEY_APP_FIRST_INSTALL_TIME, defValue: -1)
        if installTime < 0 {
            LocalStorage.shared().putInt64(key: SpKey.SP_KEY_APP_FIRST_INSTALL_TIME, value: Date().timestamp)
        }
        return true
    }

    // MARK: UISceneSession Lifecycle

    func application(_ application: UIApplication, configurationForConnecting connectingSceneSession: UISceneSession, options: UIScene.ConnectionOptions) -> UISceneConfiguration {
        // Called when a new scene session is being created.
        // Use this method to select a configuration to create the new scene with.
        return UISceneConfiguration(name: "Default Configuration", sessionRole: connectingSceneSession.role)
    }

    func application(_ application: UIApplication, didDiscardSceneSessions sceneSessions: Set<UISceneSession>) {
        // Called when the user discards a scene session.
        // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
        // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
    }

    /// 程序退出
    func applicationWillTerminate(_ application: UIApplication) {
        
    }
}

