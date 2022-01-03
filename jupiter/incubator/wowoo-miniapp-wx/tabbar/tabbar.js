// tabbar/tabBar.js
const app = getApp();
Component({
  /**
   * 组件的属性列表
   */
  properties: {
    tabbar: {
      type: Object,
      value: {
        "backgroundColor": "#ffffff",
        "color": "#979795",
        "selectedColor": "#1c1c1b",
        "list": [
          {
            "pagePath": "/pages/index/anims",
            "iconPath": "icon/icon_home.png",
            "selectedIconPath": "icon/icon_home_HL.png",
            "text": "首页"
          },
          {
            "pagePath": "/pages/fireworks/fireworks",
            "iconPath": "icon/icon_release.png",
            "isSpecial": true,
            "text": "惊喜"
          },
          {
            "pagePath": "/pages/mine/mine",
            "iconPath": "icon/icon_mine.png",
            "selectedIconPath": "icon/icon_mine_HL.png",
            "text": "我的"
          }
        ]
      }
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    /*isIphoneX: app.globalData.systemInfo.model == "iPhone X" ? true : false,*/
    isIphoneX: false,
  },

  /**
   * 组件的方法列表
   */
  methods: {
    // 在这里面获取数据有两种方法：
    //      一种是获取data里的数据： this.data.属性名；
    //      一种是获取 properties 中的属性值： this.properties.属性名。
    // https://www.jianshu.com/p/3aec33a54d8d
    selectItemAction: function (e) {
        var nowData = this.properties.tabbar;
        var index = e.target.dataset.index;
        var pagePath = nowData[index].pagePath;

        console.debug("------", index, pagePath)

        if (nowData[index].selected) {
            wx.redirectTo({
                url: pagePath,
            }) 
        }
        var nowDate = {
          index: index,
          page: pagePath,
        }
        this.triggerEvent('getCurrData', nowDate);
        console.debug("tabbar selectItemAction: redirectTo =", pagePath)
    }
  }
})
