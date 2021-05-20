const util = {
    formatTime: date => {
        const year = date.getFullYear()
        const month = date.getMonth() + 1
        const day = date.getDate()
        const hour = date.getHours()
        const minute = date.getMinutes()
        const second = date.getSeconds()
        return [year, month, day].map(formatNumber).join('/') + ' ' + [hour, minute, second].map(formatNumber).join(':')
    },
    formatNumber: function (n) {
        n = n.toString()
        return n[1] ? n : '0' + n
    },
    randomInt: function (min = 0, max){
        return parseInt(min + (max - min) * Math.random());
    },
    randomIn: function (min = 0, max){
        return min + (max - min) * Math.random();
    },
    shuffle: function (arr) {
        arr.sort(function () {
            return Math.random() - 0.5;
        });
    },
    extendArr: function(origin, ...arg){
        arg.forEach(item => {
            for(let key in item){
                origin[key] = item[key];
            }
        });
        return origin;
    },
}

const animationFrame =  {
    requestAnimationFrame: function (callback, lastTime) {
        var lastTime;
        if (typeof lastTime === 'undefined') {
            lastTime = 0
        }
        var currTime = new Date().getTime();
        var timeToCall = Math.max(0, 16.7 - (currTime - lastTime));
        lastTime = currTime + timeToCall;
        var id = setTimeout(function() {
            callback(currTime + timeToCall, lastTime);
        },
        timeToCall);
        return id;
    },     
    cancelAnimationFrame: function (id) {
        clearTimeout(id);
    }
}

//输入的h范围为[0,360],s,l为百分比形式的数值,范围是[0,100] 
//输出r,g,b范围为[0,255],可根据需求做相应调整
// https://syean.cn/2017/03/17/JS%E5%AE%9E%E7%8E%B0RGB-HSL-HSB%E7%9B%B8%E4%BA%92%E8%BD%AC%E6%8D%A2/
function hsltorgb(hue,saturation,lightness) {
    var h=hue/360;
    var s=saturation/100;
    var l=lightness/100;
    var rgb=[];
    if(s==0){
        rgb=[Math.round(l*255),Math.round(l*255),Math.round(l*255)];
    }else{
        var q=l>=0.5?(l+s-l*s):(l*(1+s));
        var p=2*l-q;
        var tr=rgb[0]=h+1/3;
        var tg=rgb[1]=h;
        var tb=rgb[2]=h-1/3;
        for(var i=0; i<rgb.length;i++){
            rgb[i]=Math.round(hue2rgb(p, q, rgb[i]) * 255);
        }
    }
    return rgb;
}

function hue2rgb(p, q, vH) {
    if (vH < 0) vH += 1;
    if (vH > 1) vH -= 1;
    if (6.0 * vH < 1) return p + (q - p) * 6.0 * vH;
    if (2.0 * vH < 1) return q;
    if (3.0 * vH < 2) return p + (q - p) * ((2.0 / 3.0) - vH) * 6.0;
    return p;
}

const Gravity = 0.06;
const GravityAngle = 1.5 * Math.PI;

module.exports = {
    Gravity : Gravity,
    GravityAngle: GravityAngle,
    hsltorgb: hsltorgb,
    util: util,
    animationFrame: animationFrame,
}
