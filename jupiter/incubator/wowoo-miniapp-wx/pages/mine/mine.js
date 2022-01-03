// mine.js

const app = getApp()
var global = require('../../utils/anims.js')
var utils = require('../../utils/util.js')

Page({
    data: {
        txtDescribeA: "",
        txtDescribeB: "",
        imgSelect: "",
        userInfo: {},
        hasUserInfo: false,
        canIUse: wx.canIUse('button.open-type.getUserInfo'),
        refreshDoID: 0,
        refreshDoTimes: 0,
        refreshDoInterval: 20,
        refreshBtnDisable: false,
        imgBg: "",
    },
    onLoad: function(options) {
        app.editTabbar();

        if (app.globalData.userInfo) {
            this.setData({
                userInfo: app.globalData.userInfo,
                hasUserInfo: true
            })
        } else if (this.data.canIUse){
            // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
            // 所以此处加入 callback 以防止这种情况
            app.userInfoReadyCallback = res => {
                this.setData({
                    userInfo: res.userInfo,
                    hasUserInfo: true
                })
            }
        } else {
            // 在没有 open-type=getUserInfo 版本的兼容处理
            wx.getUserInfo({
                success: res => {
                    app.globalData.userInfo = res.userInfo
                    this.setData({
                        userInfo: res.userInfo,
                        hasUserInfo: true
                    })
                }
            })
        }

        this.setData({
            txtDescribeA: "你最喜欢的是: ",
            txtDescribeB: "",
            imgSelect: "",
            imgRefresh: global.appResc.minImgRefresh,
            imgBg: global.appResc.minImgBg,
        })

        this.updateSelectItem(Object.keys(global.zooData), utils.util.randomInt(0, 100))
    },
    onUnload: function(e) {
        this.stopRefreshDo()
    },
    getUserInfo: function(e) {
        app.globalData.userInfo = e.detail.userInfo
        this.setData({
            userInfo: e.detail.userInfo,
            hasUserInfo: true
        })
        console.debug("mine getUserInfo: ", e.detail)
    },
    updateSelectItem: function(zooKeys, times) {
        let index = times % zooKeys.length
        let zooData = global.zooData
        let select = zooData[zooKeys[index]]
        console.debug("updateSelectItem: index=", index, " select=", select, times)
        this.setData({
            txtDescribeB: select.name,
            imgSelect: global.appConf.host + select.imgdir + select.thumbnail,
        })
        console.debug("updateSelectItem: index=", index, " select=", select)
    },
    refreshDo: function(fnCallback) {
        return setTimeout(fnCallback, this.data.refreshDoTimes * this.data.refreshDoInterval / 6)
    },
    getSelectedItem: function() {
        let zooData = global.zooData
        let zooKeys = Object.keys(zooData)
        for (var key in app.globalData.favorTable) { 
            for (var i = 0; i < Object.keys(app.globalData.favorTable).length; i++) {
                zooKeys.push(key)
            }
        }
        utils.shuffle(zooKeys)

        this.updateSelectItem(zooKeys, ++this.data.refreshDoTimes)
        if (this.data.refreshDoTimes * this.data.refreshDoInterval >= 1200) {
            this.stopRefreshDo()
            return 
        } else {
            console.debug("getSelectedItem doTimes:", this.data.refreshDoTimes)
        }

        clearTimeout(this.data.refreshDoID)
        this.data.refreshDoID = this.refreshDo(this.getSelectedItem.bind(this))
    },
    tapRefresh: function(e) {
        this.setData({
            refreshBtnDisable: true,
        })
        this.getSelectedItem()
    },
    stopRefreshDo: function() {
        clearTimeout(this.data.refreshDoID)
        this.setData({
            refreshDoTimes: 0,
            refreshBtnDisable: false,
        })
    }
})