import * as resData from '../utils'
import { sound,store } from '../modules'
import { monitor } from '../core'

const bgm = sound.load(resData.urlCatapultBgm, {volume: .4, loop: true})

function pause() {
    bgm.paused ? null : bgm.pause()
}

function play() {
    bgm.paused ? bgm.play() : null
}

export default function() {
    store.setting.music && bgm.play()
    monitor
        .on('setting:music', ok => ok ? bgm.play() : bgm.pause())
        .on('ad:close', () => store.setting.music && bgm.play())
        .on('wx:show', () => store.setting.music && bgm.play())
}