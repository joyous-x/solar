const {
    devicePixelRatio,
    windowWidth,
    windowHeight,
} = wx.getSystemInfoSync();

// lib/weapp-adapter.js 做了针对wx的适配

// 使用: https://pixijs.download/v4.8.8/pixi.js
// pixi:5.0+ 会对 new PIXI.Application 报错：TypeError
//    通过 console.log("------>", typeof PIXI, Object.getOwnPropertyNames(PIXI)) 可以发现没有导出信息

// 处理 async / await
// https://www.holidaypenguin.com/blob/2019-07-03-let-wechat-applet-support-async-await/


// const load = function() {
//     console.debug("----> app: resource loader ready")
//     loader.baseUrl = 'static'
//     loader.add('textures/ui.json')
//           .add('textures/item.json')
//           .on("progress", (l, resource)=> {
//             console.debug("----> app: resource loader loading: " + resource.url + " progress: " + loader.progress + "%"); 
//             console.debug("----> app: resource loader.loading", Object.keys(loader.resources))
//           })
//           .load(() => {
//             uiRes = PIXI.loader.resources["textures/ui.json"].textures
//             itemRes = PIXI.loader.resources["textures/item.json"].textures
//             !pointer && monitor.emit('scene:go', 'entry')
//           });
//     const {query: {id}} = wx.getLaunchOptionsSync()
//     /* 来自分享 */
//     id && wechat.cloud.verify(id).catch(console.log)
// }
