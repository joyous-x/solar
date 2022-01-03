var global = require('../../utils/anims.js')
var innerAudioContext = wx.createInnerAudioContext()

Page({
    data: {
        showModal: false,
        item_name_cen: "",
        item_soundurl: "",
        item_imageurl: "",
        item_images: [],
        item_key  : "",
        item_index: 0,
        move_flag:0,
        item_img_index: "",
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
            this.push(curItem.imgdir + val);
        }, images)
        let realIndex = (+this.data.item_index+1) + "-" + images.length
        this.setData({
            showModal: true,
            item_key : key,
            item_index: index,
            item_images: images,
            item_name_cen: curItem.name + " " + curItem.name_en,
            item_soundurl: global.appConf.host + curItem.wowurl,
            item_imageurl: global.appConf.host + images[index],
            item_img_index: realIndex,
        });
        console.debug("fillData, index=" + index + " , name=" + curItem.name)
    },

    onLoad: function(options) {
        wx.getSystemInfo({
            success: (res) => {
              let ww = res.windowWidth;
              let wh = res.windowHeight;
            //   let imgWidth = ww * 0.48;
            //   let scrollH = wh;
            //   this.setData({
            //     scrollH: scrollH,
            //     imgWidth: imgWidth
            //   });
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

        this.fillData(options.key, options.index)
    },
 
    preventTouchMove: function() {
    },
 
    closeMask: function() { 
        this.setData({
            showModal: false
        })
        wx.navigateBack({
            delta: 1,
        })
    },

    touchHandler: function(e) {
        innerAudioContext.src = this.data.item_soundurl
        innerAudioContext.play()
    },
    handleMoveEvent: function(flag) {
        switch(flag) {
            case 1:
                var index = +this.data.item_index - 1
                if (index < 0) {
                    index += this.data.item_images.length
                }
                this.fillData(this.data.item_key, index)
                console.debug("move left");
                break;
            case 2:
                var index = +this.data.item_index + 1
                if (index >= this.data.item_images.length) {
                    index %= this.data.item_images.length
                }
                this.fillData(this.data.item_key, index)
                console.debug("move right");
                break;
            case 3:
                this.closeMask();
                console.debug("move up");
                break
            case 4:
                console.debug("move down");
                break;
            default:
                console.debug("move undefined");
        }
    },
    touchMove: function(event) {
        if (this.data.move_flag !== 0){
            return
        }
        let currx = event.touches[0].pageX;
        let curry = event.touches[0].pageY;
        let stepx = currx - this.data.lastX;
        let stepy = curry - this.data.lastY;

        if (Math.abs(stepx) > Math.abs(stepy)) {
            if (stepx < 0) {
                this.data.move_flag= 1  // 向左滑动
            } else {
                this.data.move_flag= 2  // 向右滑动
            }
        } else {
            if (stepy < 0){
                this.data.move_flag= 3 // 向上滑动
            } else {
                this.data.move_flag= 4 // 向下滑动
            }
        }

        this.data.lastX = currx;
        this.data.lastY = curry;
        this.handleMoveEvent(this.data.move_flag)
    },
    touchStart:function(event) {
        this.data.lastX = event.touches[0].pageX;
        this.data.lastY = event.touches[0].pageY;
    },
    touchEnd:function(event) {
        this.data.move_flag = 0
    },
})
