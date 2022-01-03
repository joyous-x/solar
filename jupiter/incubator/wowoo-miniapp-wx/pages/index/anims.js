//anims.js
const app = getApp()

var global = require('../../utils/anims.js')

let colaH = 0;
let colbH = 0;

Page({
    data: {
        items: [], 
        scrollH: 0,
        cola: [],
        colb: [],
        imgWidth: 0,
        loadingCount: -1,
        loadingImages: [],
        page: 0,
        pageSize: 15,
        animsImgBg: "",
    },
    onLoad: function(options) {
        wx.getSystemInfo({
            success: (res) => {
              let ww = res.windowWidth;
              let wh = res.windowHeight;
              let imgWidth = ww * 0.48;
              let scrollH = wh;
              this.setData({
                scrollH: scrollH,
                imgWidth: imgWidth
              });
              this.initItems();
              this.loadImages();
            }
        })
        this.setData({
            animsImgBg: global.appResc.animsImgBg,
            title: (typeof options.title === 'undefined') ? "animals" : options.title,
        });
        app.editTabbar();
    },
    onUnload: function () {
        wx.hideToast();
        console.log("onUnload page " + "anims")
    },
    onImageLoad: function(e) {
        let factor = 0.64
        let imageId = e.currentTarget.id;
        let oImgW = e.detail.width;         //图片原始宽度
        let oImgH = e.detail.height;        //图片原始高度
        let scale = this.data.imgWidth / oImgW;      //比例计算=图片设置的宽度/图片原始宽度
        let imgWidth = this.data.imgWidth * factor;  //宽度
        let imgHeight = oImgH * scale * factor;      //高度自适应

        let images = this.data.loadingImages;
        let imageObj = null;

        for (let i = 0; i < images.length; i++) {
            let img = images[i];
            if (img.id == imageId) {
                imageObj = img;
                break;
            }
        }

        imageObj.width = imgWidth;
        imageObj.height = imgHeight;
        let loadingCount = this.data.loadingCount - 1;
        let cola = this.data.cola;
        let colb = this.data.colb;

        if (colaH <= colbH) {
            colaH += imgHeight;
            cola.push(imageObj);
        } else {
            colbH += imgHeight;
            colb.push(imageObj);
        }

        let data = {
            loadingCount: loadingCount,
            cola: cola,
            colb: colb
        };

        if (!loadingCount) {
            data.images = [];
            wx.hideToast();
        }

        this.setData(data)
        console.debug("loadingCount is " + imageId + ", " + imageObj.name_cn)
    },
    initItems: function() {
        let zooData = global.zooData
        let images = []
        let index = 0
        for (var key in zooData) {
            let tmp = zooData[key]
            let imgItem = {
                id: index++,
                key: key,
                height: 0,
                name_cn: tmp.name,
                name_en: tmp.name_en,
                imageurl: global.appConf.host + tmp.imgdir + tmp.thumbnail,
                soundurl: global.appConf.host + tmp.wowurl
            }
            images.push(imgItem)
        }
        this.setData({
            items: images
        })
    },
    loadImages: function () {
        let images = [];
        let curPage = this.data.page, pageSize = this.data.pageSize;

        for (let i = 0; i < this.data.items.length; i++) {
            if (i < curPage * pageSize || i >= (curPage + 1) * pageSize ) {
                continue;
            }
            images.push(this.data.items[i]);
            console.log(this.data.items[i]);
        }

        if (images.length > 0) {
            wx.showToast({
                title: '拼命加载中...',
                icon: 'loading',
                duration: 5000
            })
        }

        this.setData({
            loadingCount: images.length,
            loadingImages: images,
            page: curPage + 1
        });
    }, 
    clickOnContItem: function(e) {
        if (false === ("number" === typeof e.currentTarget.dataset.id)) {
            console.debug("e.currentTarget.dataset.id type error: " + (typeof e.currentTarget.dataset.id))
            return 
        }
        let id = e.currentTarget.dataset.id;
        let key = this.data.items[id].key
        if (app.globalData.favorTable.has(key)) {
            app.globalData.favorTable[key]++
        } else {
            app.globalData.favorTable.set(key, 1)
        }
        console.debug("click on item:", this.data.items.length, key, this.data.items[id])
        wx.navigateTo({
            url: '/pages/swiper/swiper?key=' + key + '&index=' + 0,
            success:res => {},
            fail:err => {},
            complete:_ => {}
        })
    }
})
