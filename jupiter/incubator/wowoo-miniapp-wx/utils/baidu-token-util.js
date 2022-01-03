
const getBdAiAccessToken = function () {
    return  new Promise((resolve, reject) => {
        var time = wx.getStorageSync("time");
        var curTime = new Date().getTime();
        var timeNum = parseInt((curTime - time) / 1000/60/60/24);
        var accessToken = wx.getStorageSync("access_token")
        if (timeNum > 28 || (accessToken == "" || accessToken == null || accessToken == undefined)) {
            //token超过28天或者不存在，则调用云函数重新获取
            //
            // TODO
            //
            // wx.cloud.callFunction({
            //     name: 'baiduAccessToken',
            //     success: res => {
            //         console.log("云函数获取token:" + JSON.stringify(res))
            //         var access_token = res.result.data.access_token
            //         wx.setStorageSync("access_token", access_token);
            //         wx.setStorageSync("time", new Date().getTime());
            //         resolve(
            //             {
            //                 'access_token': access_token
            //             }
            //         );
            //     },
            //     fail: error => {
            //         console.error('[云函数] [sum] 调用失败：', error);
            //         reject('调用云函数失败：' + JSON.stringify(error));
            //     }
            // });
        } else {
            resolve( { 'access_token': accessToken } );
        }
    });
}

module.exports = {
    getBdAiAccessToken: getBdAiAccessToken,
}