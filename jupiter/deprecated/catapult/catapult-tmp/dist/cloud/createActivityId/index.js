// 云函数入口文件
const cloud = require('wx-server-sdk')

cloud.init({env: 'joyous-catapult'})

// 云函数入口函数
exports.main = () => {
  return cloud.openapi.updatableMessage.createActivityId()
}