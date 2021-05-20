/* 检查更新 */
export const update = function() {
    const manager = wx.getUpdateManager()
    manager.onUpdateReady(() => {
      manager.applyUpdate()
    })
}