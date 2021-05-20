// 云函数入口文件
const cloud = require('wx-server-sdk')

cloud.init({env: 'joyous-catapult'})

exports.main = () => {
  return cloud.getWXContext()
}