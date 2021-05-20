import * as resData from '../utils'
import {store} from '../modules'
import {monitor} from '../core'

export function showShareMenu() {
    /* 开启转发 */
    wx.showShareMenu({withShareTicket: true})
    wx.onShareAppMessage(() => ({
    title: resData.strCatapultShareTitle,
    query: `id=${store.id}`,
    imageUrl: [
        resData.urlCatapultSnapshot
    ][~~(Math.random() * 1)]
    }))

    monitor.on('wx:share', opt => {
    wx.shareAppMessage(opt || {
        title: resData.strCatapultShareTitle,
        query: `id=${store.id}`,
        imageUrl: [
        resData.urlCatapultSnapshot
        ][~~(Math.random() * 1)]
    })
    })
}

