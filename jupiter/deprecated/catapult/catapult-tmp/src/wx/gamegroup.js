import {monitor, screen} from '../core'

/* 游戏圈 */
export const gameGroup = function() {
    const button = wx.createGameClubButton({
      icon: 'white',
      style: {
        left: 10,
        top: screen.height * .1,
        width: 40,
        height: 40
      }
    })
  
    button.hide()
  
    monitor
      .on('scene:show', name => name === 'entry' ? button.show() : button.hide())
      .on('scene:hide', name => name === 'entry' && button.hide())
}