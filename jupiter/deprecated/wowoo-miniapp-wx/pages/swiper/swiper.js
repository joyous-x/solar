// swiper.js

var global = require('../../utils/anims.js')
const app = getApp()
var innerAudioContext = wx.createInnerAudioContext()

Page({
    data: {
        swiperCH: 0,
        swiperHeight: 0,
        imgUrls: [],
        soundUrl: "",
        swiperIndex: 0 //这里不写第一次启动展示的时候会有问题
    },
    fillData: function(key, index) {
        let zooData = global.zooData
        if (!zooData.hasOwnProperty(key)) {
            console.debug("fillData, invalid key=" + key)
            return
        }
        let curItem = zooData[key]
        let images = []
        curItem.imgs.forEach(function(val, _) {
            this.push(global.appConf.host + curItem.imgdir + val);
        }, images)
        this.setData({
            imgUrls: images,
            soundUrl: global.appConf.host + curItem.wowurl
        });
        console.debug("fillData, index=" + index + " , name=" + curItem.name)
    },
    onLoad: function(options) {
        wx.getSystemInfo({
            success: (res) => {
              let wh = res.windowHeight;
              this.setData({
                swiperCH: wh * 0.88,
                swiperHeight: wh * 0.86,
              });
            }
        })

        innerAudioContext.destroy();
        innerAudioContext = wx.createInnerAudioContext()
        innerAudioContext.onPlay(() => {
            console.log("begin play sound: " + innerAudioContext.src)
        })
        innerAudioContext.onError((res) => {
            console.log("play sound error: " + innerAudioContext.src + ", " + res.errCode+ ", " + res.errMsg)
        })

        this.fillData(options.key, options.index);
        app.editTabbar();
    },
    onUnload: function(e) {
        innerAudioContext.destroy();
        console.log("onUnload: swiper")
    },
    swiperChange: function(e) {
        this.setData({
            swiperIndex: e.detail.current
        })
    },
    chuangEvent: function (e) { 
        this.setData({
            swiperIndex: e.currentTarget.id
        })
    },
    tapSwiperEvent: function(e) {
        innerAudioContext.src = this.data.soundUrl
        innerAudioContext.play()
    },
})
