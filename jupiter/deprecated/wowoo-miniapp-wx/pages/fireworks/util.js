const util = {
    isPhone() {
        var platform = ""
        wx.getSystemInfo({
            success:function(res){
                platform = res.platform
            }
        })
        return platform
    },
	//ms => 帧
	transTime(time, defult){
		return +time / 1000 * 60 | 0 || defult;
    },
    requestAnimationFrame(callback, lastTime) {
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
    cancelAnimationFrame(id) {
        clearTimeout(id);
    }
}

export default util