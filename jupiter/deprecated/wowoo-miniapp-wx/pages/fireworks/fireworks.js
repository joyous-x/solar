// 基础配置
import config from './config'
import SnowflakeSprite from './fall/snowflakeSprite'
import HeartSprite from './fall/heartSprite'
import Firework from './fireworks/fireworks'
import ShapeMaker from './other/shape'

var utils = require('../../utils/util.js')
var global = require('../../utils/anims.js')

const app = getApp()

Page({
    data: {
        windowHeight: 0,
        windowWidth: 0,
        canvasWidth: 0,
        canvasHeight: 0,
        canvas: undefined,
        bgImg: "",
        touchCount: 0,
        musicSwitchOnImg: "",
        musicSwitchOffImg: "",
        animationSwitchMain:null,//正面
        animationSwitchBack:null,//背面
    },
    canvasIdErrorCallback: function (e) {
      console.error(e.detail.errMsg)
    },
    onLoad: function(options) {
        wx.getSystemInfo({
            success: (res) => {
              this.setData({
                windowHeight: res.windowHeight,
                windowWidth: res.windowWidth,
                canvasWidth: res.windowWidth,
                canvasHeight: res.windowHeight
              });
            }
        })
        this.setData({
            title: (typeof options.title === 'undefined') ? "fireworks" : options.title,
            bgImg: global.appResc.fksImgBg,
            musicSwitchOnImg: global.appResc.fksImgAudioOn,
            musicSwitchOffImg: global.appResc.fksImgAudioOff,
            canvas: new FCanvas()
        })
        app.editTabbar();
        console.debug("page fireworks onLoad")
    },
    onUnload: function () {
        this.data.canvas.uninit()
        console.debug("page fireworks onUnload")
    },
    onReady: function (e) {
       this.data.canvas.init(this.data.windowHeight, this.data.windowWidth, this.data.bgImg)
        this.onBgMusic()
        console.debug("page fireworks onReady")
    },
    offBgMusic: function(e) {
		this.data.canvas.audioOff()
    },
    onBgMusic: function(e) {
		this.data.canvas.audioOn()
    },
    touchHandler: function(e) {
        if (utils.util.randomIn(0, this.data.canvas.fireworks.length) < 1) {
            this.data.canvas.createFireworks("")
        }
        this.data.touchCount++
    },
    rotateSwitchFn: function(e) {
        var id = e.currentTarget.dataset.id
        this.animation_main = wx.createAnimation({
            duration:400,
            timingFunction:'linear'
        })
        this.animation_back = wx.createAnimation({
            duration:400,
            timingFunction:'linear'
        })
        if (id==1) {
            this.animation_main.rotateY(180).step()
            this.animation_back.rotateY(0).step()
            this.setData({
                animationSwitchMain: this.animation_main.export(),
                animationSwitchBack: this.animation_back.export(),
            })
            this.offBgMusic()
        } else {
            this.animation_main.rotateY(0).step()
            this.animation_back.rotateY(-180).step()
            this.setData({
                animationSwitchMain: this.animation_main.export(),
                animationSwitchBack: this.animation_back.export(),
            })
            this.onBgMusic()
        }
    }
})

class FCanvas {
    init(height, width, bgImg){
        this.initArgs(height, width, bgImg)
        this.initProperty();
        // this.drawBg(this.bgCtx, bgImg, height, width);
        this.loop();
    }
    uninit() {
        this.bgmusic.destroy()
        this.abortAnimationFrame(this.animationID)
    }
    initArgs(height, width, bgImgPath) {
        this.height = height;
        this.width = width;
        this.bgImgPath = bgImgPath;
        config.canvases.forEach(canvasId => {
            this[canvasId + 'Ctx'] = wx.createCanvasContext(canvasId);
        });
        const audio = new wx.createInnerAudioContext();
        audio.src = global.appResc.fksAudioBgm;
        audio.loop = true;
        audio.volume = 0.5;
        this.bgmusic = audio;
        this.audioStatus = 1; // 默认开启音效
        this.animationID = 0;
        this.maxFireworksLen = 5;
        console.debug("Canvas height=" + height + " width=" + width);
    }
    initProperty(){
        // 飘落微粒
        this.fallDots = [];
        this.fallType = config.fallType,
        //动画的时间
        this.time = 0;
        this.lastFrameTime = 0;
        //当前执行的状态
        this.status = 0;
        /********* 烟花 *********/
        //天空颜色
        this.skyColor = {
            hue: 210,
            lightness: 0
        };

        //烟花的数组
        this.fireworks = [];
        this.fireOpt = utils.util.extendArr({
            end: false,
            time: 600,
            showWords: false,
        }, config.fireOpt);
    }
    audioOn() {
        this.audioStatus = 1;
        this.bgmusic.play();
    }
    audioOff() {
        this.audioStatus = 0;
        this.bgmusic.pause();
    }
    drawBg(ctx, imgPath, canvas_height, canvas_width){
        wx.getImageInfo({
            src: imgPath,
            success(res) {
                var img_width = res.width, img_height = res.height;
                var clip_left, clip_top; //左偏移值，上偏移值，
                var clip_width, clip_height; //截取宽度，截取高度
                clip_height = img_width * (canvas_height / canvas_width);
                if (clip_height > img_height) {
                    clip_height = img_height;
                    clip_width = clip_height * (canvas_width / canvas_height);
                    clip_left = (img_width - clip_width) / 2;
                    clip_top = 0;
                } else {
                    clip_left = 0;
                    clip_top = (img_height - clip_height) / 2;
                    clip_width = img_width
                }
                ctx.drawImage(imgPath, clip_left, clip_top, clip_width, clip_height, 0, 0, canvas_width, canvas_height);
                ctx.draw();
                console.debug("drawBg succ: src =", imgPath)
            },
            fail(res) {
                console.debug("drawBg fail: src =", imgPath)
            }
        })
    }
    // 模拟 requestAnimationFrame
    doAnimationFrame(fnCallback) {
        var currTime = new Date().getTime();
        var timeToCall = Math.max(0, 16 - (currTime - this.lastFrameTime));
        var animation_id = setTimeout(function () { fnCallback(currTime + timeToCall); }, timeToCall);
        this.lastFrameTime = currTime + timeToCall
        return animation_id
    }
    // 模拟 cancelAnimationFrame
    abortAnimationFrame(animation_id) {
        clearTimeout(animation_id)
    }
    loop() {
        this.animationID = this.doAnimationFrame(this.loop.bind(this));
        ++this.time >= 60000 ? 0 : this.time;
        this.renderFall(this.fallType)
        if (this.time % utils.util.randomInt(...config.fireworkInterval) == 0) {
            this.createFireworks("")
        }
        this.renderFireworks();
    }
    createFireworks(types) {
        if (this.fireworks.length > this.maxFireworksLen) {
            return
        }
        let p = Math.random()
        if (p > 0.9) {
            // 超级大烟花
            this.fireworks.push(new Firework({
                x: config.width / 2,
                yEnd: config.height / 7,
                count: [100, 150],
                velocity: 5,
                raiserSize: 4
            }));
        } else {
            for(let i = 0; i < 1; i++){
                setTimeout(() => {
                    this.fireworks.push(new Firework(config.fireworks));
                }, i * 100);
            }
        }
    }
    //飘落的装饰
    renderFall(fallType){
        this.fallCtx.clearRect(0,0,this.width,this.height);
        switch(fallType){
            case 'snow':
                this.time % config.fallInterval == 0 && this.fallDots.push(new SnowflakeSprite(config.snow));
                break;
            case 'heart': 
                this.time % config.fallInterval == 0 && this.fallDots.push(new HeartSprite(config.heart));
                break;
            case 'mix': 
                if (this.time % config.fallInterval == 0) {
                    let rint = utils.util.randomInt(1, 60);
                    rint % 3 != 1 && this.fallDots.push(new SnowflakeSprite(config.snow));
                    rint % 3 == 1 && this.fallDots.push(new HeartSprite(config.heart));
                }
                break;
        }
        for(let i = this.fallDots.length - 1; i >= 0; --i){
            if (!this.fallDots[i].render(this.fallCtx)) {
                this.fallDots.splice(i,1);
            }
        }
        this.fallCtx.draw()
    }
    //渲染烟花
    renderFireworks(){
        // 将复合操作设置为destination-out将允许我们以特定的不透明度清除画布，而不是完全擦除画布
        this.fireworkCtx.globalCompositeOperation = 'destination-out';
        // 减少alpha属性以创建更显着的路径
        this.fireworkCtx.fillStyle = 'rgba(0, 0, 0, 0.5)';
        this.fireworkCtx.fillRect(0,0,this.width,this.height);	

        for(let i = this.fireworks.length - 1; i >= 0; --i) {
            if (false == this.fireworks[i].render(this.fireworkCtx)) {
                this.fireworks.splice(i,1);
            }
        }
        this.fireworkCtx.draw();
        if (this.audioStatus == 1) {
            for(let i = this.fireworks.length - 1; i >= 0; --i) {
                this.fireworks[i].playAudio()
            }
        }
    }
}