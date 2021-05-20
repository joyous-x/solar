import { device } from '../core'
import dayjs from 'dayjs'
import {store} from '../modules'
import * as wechat from './index'

function login() {
    return new Promise((resolve, reject) => {
      wx.getUserInfo({
        success({userInfo, cloudID}) {
          resolve({
            cloudID,
            user: {
              name: userInfo.nickName,
              avatar: userInfo.avatarUrl,
              city: userInfo.city,
              gender: userInfo.gender
            }
          })
        },
  
        fail() {
          const btn = wx.createUserInfoButton({
            type: 'text',
            text: '',
            style: {
              left: 0,
              top: 0,
              width: device.width,
              height: device.height,
              backgroundColor: 'transparent'
            }
          })
  
          btn.onTap(({userInfo, cloudID}) => {
            btn.destroy()
            userInfo ? resolve({
              cloudID,
              user: {
                name: userInfo.nickName,
                avatar: userInfo.avatarUrl,
                city: userInfo.city,
                gender: userInfo.gender
              }
            }) : reject({msg: '拒绝授权'})
          })
        }
      })
    })
}

/* 记录登录 */
function loginCaller() {
    login().then(async info => {
      store.user = info.user
      let stuff = await wechat.cloud.transfer().catch(console.log)
      if (!stuff) return
      store.id = stuff
    }).catch(async () => {
      /* 拒绝授权 */
      let stuff = await wechat.cloud.transfer().catch(console.log)
      if (!stuff) return
      store.id = stuff
    }).then(async () => {
      if (!store.id) return
      let stuff = await wechat.cloud.find({_openid: store.id}).catch(console.log)
      if (stuff) {
        store.diamond = stuff.diamond || 0
        store.unlocked = Math.max(stuff.unlocked || 1, store.unlocked)
        wechat.cloud.update({
          ...store.user,
          stamp: Date.now(),
          date: dayjs().format('YYYY/MM/DD HH:mm:ss')
        })
      } else {
        wechat.cloud.set({
          ...store.user,
          diamond: 3,
          invitees: [],
          stamp: Date.now(),
          date: dayjs().format('YYYY/MM/DD HH:mm:ss')
        })
      }
    })
}

function loginAfter() {
    const {query: {id}} = wx.getLaunchOptionsSync()
    id && wechat.cloud.verify(id).catch(console.log)
}

export {
    loginCaller,
    loginAfter
}