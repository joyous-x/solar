const app = getApp()

Component({
    properties: {
        title: {
            type: String,
            value: 'navigation bar'
        },
        showBack: {
            type: Boolean,
            value: false
        },
        showLocation: {
            type: Boolean,
            value: false
        },
        locationName: {
            type: String,
            value: "location",
        }
    },
    data: {
        statusBarHeight: app.globalData.statusBarHeight + 'px',
        navigationBarHeight: (app.globalData.statusBarHeight + 44) + 'px'
    },
    methods: {
        backHome: function () {
            let pages = getCurrentPages()
            wx.navigateBack({
                delta: pages.length
            })
        },
        back: function () {
            wx.navigateBack({
                delta: 1
            })
        }
    }
})