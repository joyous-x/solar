import util from './util'
var utils = require('../../utils/util.js')
var global = require('../../utils/anims.js')

//与时间有关的设置均为毫秒数，本文件底部会自动转化为帧数
const config = (function () {
    var width = 0
    var height = 0
    wx.getSystemInfo({
        success: (res) => {
            if (res.platform == "ios") {
                width = 360;
                height = 600;
            } else {
                width = res.windowWidth;
                height = res.windowHeight;
            }
        }
    });

    return {
        width: width,
        height: height,
        canvases: ['fall', 'bg', 'firework'],
        fallType: 'mix',  // 飘落类型('snow', 'heart', 'mix')
        fallInterval: 60, // 飘落微粒产生间隔

        // 飘落微粒属性
        snow: {
            minSize: 5,
            maxSize: 9,
            velocity: 0.6,
            opacity: 0.8
        },
        heart: {
            minSize: 12,
            maxSize: 18,
            velocity: 0.6,
        },

        // 烟花产生间隔 //---不建议改动
        fireworkInterval: [600, 1200],
        //烟花的属性
        fireworks: {
            raiserSize: 3,
            velocity: 5,  //速率
            count: [90, 120],   //炸裂后粒子数
            wait: undefined,  //消失后 => 炸裂  等待时间
            color: undefined,  //烟花颜色
        },
        // hue:210 lightness 0
        skyColorFn: function (hue, lightness) {
            let rgb = utils.hsltorgb(hue, 60, lightness)
            return `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 0.2)`
        },
        // 烟花音效
        audioBomb: global.appResc.fksAudioBomb,
        audioChos: global.appResc.fksAudioChou,
    }
})();

export default config