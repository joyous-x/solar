import platform from './platform'
import {stage, ticker, monitor, screen} from './core'
import {entry, preload, game, custom, bgmBegin} from './scenes'
import {store} from './modules'
import * as wechat from './wx'

let pointer
monitor
  .on('wx:show', async ({query: {scene}}) => {
      await preload().then(() => { wechat.loginAfter() }).catch(console.log)
      !pointer && monitor.emit('scene:go', 'entry')
  })
  .on('scene:go', (name, opt) => {
    switch (name) {
      case 'game': {
        pointer = game
        game.show(opt)
        break
      }
      case 'entry': {
        pointer = entry
        entry.show(opt)
        break
      }
      case 'custom': {
        pointer = custom
        custom.show(opt)
        break
      }
    }
  })

bgmBegin()
wx.onShow(info => monitor.emit('wx:show', info))
wechat.showShareMenu()
wechat.loginCaller()
wechat.gameGroup()
wechat.update()

/* 等待交互 */
{
  let resolve
  store.interaction = new Promise(a => resolve = a)
  wx.onTouchStart(handle)
  function handle() {
    wx.offTouchStart(handle)
    resolve(wx.getMenuButtonBoundingClientRect())
  }
}


