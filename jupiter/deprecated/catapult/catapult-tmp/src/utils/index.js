function delay(t = 0) {
  return new Promise(resolve => {
    setTimeout(resolve, t * 1e3)
  })
}

const host = "https://www.smilex.xyz/" // https://static.lufei.so

const urlCatapultBgm = 'static/sounds/bgm-faraway.mp3'
const urlCodeFavor = host + "image/code.favor.jpg"
const urlCatapultSnapshot = 'static/images/snapshot_1.png'
const strCatapultShareTitle = '少年, 开启弹射光子的烧脑之旅吧...'

export {
    delay,
    host,
    urlCatapultBgm, urlCatapultSnapshot, urlCodeFavor, strCatapultShareTitle
}