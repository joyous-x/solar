//app.js

var global = require('./utils/anims.js')
const Promise = require('./utils/bluebird.core.min.js');

App({
    onLoad: function () {
        const self = this
        wx.checkSession({
            fail() {
                wx.login({
                    succes(res) {
                        wx.setStorageSync("code", res.code)
                    }
                })
            }
        })
        self.doLoginApp()
    },
    onLaunch: function () {
        const self = this;

        wx.getSystemInfo({
            success: function (res) {
                self.globalData.statusBarHeight = res.statusBarHeight
                self.globalData.windowHeight = res.windowHeight
            }
        })

        const updateManager = wx.getUpdateManager()
        updateManager.onCheckForUpdate(function (res) {
            console.log(res.hasUpdate)
        })
        updateManager.onUpdateReady(function () {
            wx.showModal({
                title: '更新提示',
                content: '新版本已经准备好，是否重启应用？',
                success: function (res) {
                    if (res.confirm) {
                        // 新的版本已经下载好，调用 applyUpdate 应用新版本并重启
                        updateManager.applyUpdate()
                    }
                }
            })
        })

        wx.checkSession({
            success(res) {
                console.log(res)
            },
            fail() {
                wx.login({
                    success(res) {
                        wx.setStorageSync("code", res.code)
                    }
                })
            }
        })

        wx.login({
            success: res => {
                wx.setStorageSync("code", res.code)
            }
        })

        console.debug("app login ok, ready to get userInfo")

        // 获取用户信息
        wx.getSetting({
            success: res => {
                if (res.authSetting['scope.userInfo']) {
                    // 已经授权，可以直接调用 getUserInfo 获取头像昵称，不会弹框
                    wx.getUserInfo({
                        success: res => {
                            self.globalData.userInfo = res.userInfo
                            let data = {
                                appname: global.appConf.appname,
                                data: {
                                    encryptedData: res.userInfo.encryptedData,
                                    iv: res.userInfo.iv,
                                }
                            }
                            self.doPostRequest(global.appConf.host + global.apiUrls.userinfo, data, null, null)
                        }
                    })
                }
            },
            fail: res => {
                // 引导用户触发，以获取用户信息
            }
        })
    },
    doLoginApp: function () {
        const self = this;
        const code = wx.getStorageSync("code")
        let data = {
            appname: global.appConf.appname,
            data: {
                jscode: code,
                inviter: "",
            }
        }
        fnSucc = function (res) {
            if (0 == res.data.status) {
                self.globalData.userApInfo = {
                    isNewUser: res.data.data.is_new_user,
                    uuid: res.data.data.uuid,
                    token: res.data.data.token,
                }
            }
        }
        fnFail = (res => {
            wx.showLoading({ title: '系统内部错误', mask: true, duration: 1000 })
        })
        self.doPostRequest(global.appConf.host + global.apiUrls.login, data, fnSucc, fnFail)
    },
    doPostRequest: function (url, data, fnSucc, fnFail) {
        let fnSuccWrapper = function (res) {
            if (fnSucc) fnSucc(res)
            console.debug("doPostRequest request success: data =", res.data)
        }
        let fnFailWrapper = (res => {
            if (fnFail) fnFail(res)
            console.debug("doPostRequest request failed: res =", res)
        })
        wx.request({
            method: "post",
            url: url,
            data: data,
            header: {
                'content-type': 'application/json'
            },
            success: fnSuccWrapper,
            fail: fnFailWrapper,
        })
    },
    isLoginSucc: function () {
        if (!this.globalData.userApInfo || this.globalData.userInfo.token.length < 1) {
            return false
        }
        return true
    },
    editTabbar: function () {
        let tabbar = this.globalData.tabBar;
        let currentPages = getCurrentPages();
        let _this = currentPages[currentPages.length - 1];
        let pagePath = _this.route;
        (pagePath.indexOf('/') != 0) && (pagePath = '/' + pagePath);
        for (let i in tabbar.list) {
            tabbar.list[i].selected = false;
            (tabbar.list[i].pagePath == pagePath) && (tabbar.list[i].selected = true);
        }
        _this.setData({
            tabbar: tabbar
        });
    },
    globalData: {
        userInfo: null,
        userApInfo: null,
        favorTable: new Map(),
        statusBarHeight: 0,
        windowHeight: 0,
        tabBar: {
            "backgroundColor": "#ffffff",
            "color": "#979795",
            "selectedColor": "#1c1c1b",
            "list": [
                {
                    "pagePath": "/pages/index/anims",
                    "iconPath": "icon/icon_home.png",
                    "selectedIconPath": "icon/icon_home_HL.png",
                    "text": "首页"
                },
                {
                    "pagePath": "/pages/fireworks/fireworks",
                    "iconPath": "icon/icon_release.png",
                    "isSpecial": true,
                    "text": "惊喜"
                },
                {
                    "pagePath": "/pages/garbage/index/index",
                    "iconPath": "icon/icon_mine.png",
                    "selectedIconPath": "icon/icon_mine_HL.png",
                    "text": "我的"
                }
            ]
        }
    }
})