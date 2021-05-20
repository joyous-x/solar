//index.js

//获取应用实例
const app = getApp()
var global = require('../../utils/anims.js')

Page({
    data: {
        tabbar:{},
        motto: 'Hello World',
        userInfo: {},
        indexOutAnim: undefined,
        indexBgCover: "",
        indexBgTimer: undefined,
    },
    //事件处理函数
    bindViewTap: function() {
        wx.navigateTo({
            url: '../logs/logs'
        })
    },
    onLoad: function () {
        this.setData({
            indexBgCover: global.appResc.indexImgBg,
        })
        let _this = this;
        this.indexBgTimer = setTimeout(function () {
            var animation = wx.createAnimation({
                duration: 1000,
                timingFunction: 'ease',
                delay: 200
            });
            animation.opacity(0).rotate(360).scale(0, 0.4).step();
            _this.setData({
                indexOutAnim: animation.export()
            })
            wx.redirectTo({
                url: '/pages/index/anims'
            })
        }, 1500);
    },
    onUnload: function() {
        this.clearTimers()
    },
    onHide: function() {
        this.clearTimers()
    },
    clearTimers: function() {
        clearTimeout(this.indexBgTimer)
    },
    jumpToAnims: function(e) {
        this.clearTimers()
        wx.redirectTo({
            url: '/pages/index/anims'
        })
        console.debug("index jumpToAnims")
    }
})
