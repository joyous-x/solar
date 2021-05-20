import {loader} from '../core'
import {store} from '../modules'
import platform from '../platform'

/* 加载字体 */
store.font = platform.getFont()

export default function () {
  return new Promise(resolve => {
    loader.baseUrl = 'static'
    loader
      .add('textures/ui.json')
      .add('textures/item.json')
      .load(resolve)
  })
}
