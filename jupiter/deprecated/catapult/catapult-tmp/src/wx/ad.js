let ad, resolve, reject

function create() {
  ad = wx.createRewardedVideoAd({
    adUnitId: 'adunit-test-0358'
  })

  ad.onError(err => {
    wx.showToast({title: err.errMsg, icon: 'none'})
  })

  ad.onClose(({isEnded}) => {
    resolve(isEnded)
  })

  return ad
}

export function show() {
  const promise = new Promise((a, b) => {
    resolve = a
    reject = b
  })

  ad = ad || create()

  ad.show().catch(err => {
    ad.load()
      .then(() => ad.show())
      .catch(reject)
  })

  return promise
}