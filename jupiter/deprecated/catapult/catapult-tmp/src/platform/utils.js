import * as wechat from '../wx'
import { plat } from './global'


function showToast({title, icon = 'success', duration = 1500}) {
    if (plat === 'wx') {
        wx.showToast({title: title, icon: icon, duration: duration})
    }
}

function showModal({title, content, cancelColor, confirmText, confirmColor, success}) {
    if (plat === 'wx') {
        return wx.showModal({
            title: title,
            content: content,
            cancelColor: cancelColor,
            confirmText: confirmText,
            confirmColor: confirmColor,
            success: success
        })
    }
}


function getFont() {
    let font = 'sans-serif'
    if (plat === "wx") {
        font = wx.loadFont('static/fonts/Raleway-SemiBold.ttf')
    }
    return font
}


export {
    showToast, showModal, getFont, 
}