import {stage, screen, device, monitor} from '../core'
import {renderer, store, } from '../modules'

export default {
  last: 0,
  mode: 'design',

  init() {
    this.container = new PIXI.Container()

    this.uploadBtn = PIXI.Sprite.from('icon.upload.png')
    this.uploadBtn.interactive = true
    this.uploadBtn.visible = this.mode === 'design'
    this.uploadBtn.scale.set(.6)

    /* 返回按钮 */
    this.back = PIXI.Sprite.from('icon.back.png')
    this.back.tint = 0x8799a3
    this.back.visible = false
    this.back.interactive = true
    this.back.anchor.set(0, .5)

    /* 重试 */
    this.retry = PIXI.Sprite.from('icon.retry.png')
    this.retry.visible = false
    this.retry.tint = 0x00cbfa
    this.retry.interactive = true
    this.retry.anchor.set(0, .5)

    store.interaction.then(rect => {
      const {back, retry} = this

      back.visible = !retry.visible
      back.y =
      retry.y = (rect.top + rect.bottom) * .5 * device.pixelRatio

      back.x =
      retry.x = (device.width - rect.right) * device.pixelRatio
    })

    this.container.addChild(
      renderer.stage,
      renderer.bar.left,
      renderer.bar.right,
      this.uploadBtn,
      this.back,
      this.retry
    )
  },

  async upload() {
      //TODO
  },

  listen() {
      //TODO
  },

  async load(id) {
    // TODO
  },

  show(id) {
    // TODO
  },
}