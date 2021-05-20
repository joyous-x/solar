export const plat = 'wx'

export function getSystemInfo() {
    let res
    if (plat === 'wx') {
        res = wx.getSystemInfoSync()
    } else {
        return
    }
    return {devicePixelRatio: res.pixelRatio, width: res.windowWidth, height: res.windowHeight}
}