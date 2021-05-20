var checkPermissionUtil = require('../../../utils/check-permission-util.js');
var baiduTokenUtil = require('../../../utils/baidu-token-util.js');

const app = getApp()

Page({
    data: {
        title: "",
        cityName: "",
        searchText: "",
        speakStatus: ""
    },
    onShow: function() {
    },
    onLoad: function () {
        var that = this;
        try {
            baiduTokenUtil.getBdAiAccessToken().then(
                function (res) {
                    console.log('get baidu_ai token:' + JSON.stringify(res));
                    that.accessToken = res.access_token ;
                }, function (error) {
                    console.error('get baidu_ai token:' + error);
                }
            );
        } catch (error) {
            console.error(error);
        }
        this.setData({
            title: "垃圾分类指南",
            cityName:  "xx",
        })
        app.editTabbar();
    },
    
    /// search
    doTextSearch: function(text) {
        console.debug("ready to search: ", text)
        wx.navigateTo({
            url: '/pages/garbage/search/search?searchText=' + text
        });
    },
    onBindInputInput: function(e) {
        this.setData({ searchText: e.detail.value })
    },
    onBindSearch: function() {
        if (this.data.searchText.length < 1) {
            wx.showToast({ title: '请输入点什么再查询吧~', icon: 'none'});
            return
        }
        this.doTextSearch(this.data.searchText)
    },
    /// animation
    animSpeakingStart: function() {
        var self = this;
        var i = 1;
        self.setData({ speakStatus: "_" + i })
        this.animTimer = setInterval(function () {
            i++; i = i % 5;
            self.setData({ speakStatus: "_" + i })
        }, 80);
    },
    animSpeakingStop: function() {
        clearInterval(this.animTimer);
        this.setData({ speakStatus: "" })
    },
    /// voice
    onBindSpeakStart: function () {
        let self = this
        if (self.data.speakStatus != "") {
            return
        }
        try {
            self.animSpeakingStart();
            checkPermissionUtil.checkPermission('scope.record').then(function (res) {
                    if (self.data.speakStatus != "") {
                        self.recordVoice();
                    }
                }, function (err) {
                    console.error('权限检测失败：' + err);
                    wx.showToast({ title: '授权失败，无法使用该功能~', icon: 'none'});
                }
            );
        } catch (err) {
            console.error(err);
            wx.showToast({ title: '授权失败，无法使用该功能~', icon: 'none'});
        }
    },
    onBindSpeakEnd: function () {
        if (this.data.speakStatus != "") {
            this.animSpeakingStop()
            wx.getRecorderManager().stop()
        }
    },
    recordVoice: function () {
        var self = this;
        const recorderManager = wx.getRecorderManager();
        recorderManager.onStart(() => {
            console.log('recorder start')
        });
        recorderManager.onPause(() => {
            console.log('recorder pause')
        })
        recorderManager.onError((res) => {
            console.error('recorder error:' + JSON.stringify(res));
        })
        recorderManager.onStop((res) => {
            console.log('recorder stop', res);
            if (!res) { 
                return 
            }
            if (res.duration < 1000) {
                wx.showToast({ title: '说话时间太短啦！', icon: 'none' })
                return
            }
            if (res.duration > 8000) {
                wx.showToast({title: '说的有点长，可以精简点呀~', icon: 'none'})
                return
            }
            self.recognizeVoice(self.accessToken, res.tempFilePath, res.fileSize, self.recognizeVoiceCallback);
        })
        recorderManager.start({
            duration: 30000,//指定录音的时长，单位 ms
            sampleRate: 16000,//采样率
            numberOfChannels: 1,//录音通道数
            encodeBitRate: 48000,//编码码率
            format: 'aac',//音频格式，有效值 aac/mp3
        });
    },
    recognizeVoiceCallback: function(voiceData) {
        if (!voiceData || !voiceData.result) {
            wx.showModal({
                title: '提示',
                content: '语音识别出错，再试一次?',
                showCancel: false,
                success (res) {
                    if (res.confirm) {
                        console.log('用户点击确定')
                    } else if (res.cancel) {
                        console.log('用户点击取消')
                    }
                }
            })
            return
        }
        var voiceContent = voiceData.result[0];
        var text = voiceContent.replace(/[\ |\~|\`|\!|\@|\#|\$|\%|\^|\&|\*|\(|\)|\-|\_|\+|\=|\||\\|\[|\]|\{|\}|\;|\:|\"|\'|\,|\<|\.|\。|\，|\！|\；|\>|\/|\?]/g, "");
        this.doTextSearch(text)
    },
    recognizeVoice: function (token, voicePath, voiceSize, recognizeVoiceCallback) {
        wx.showLoading({ title: '语音识别中...', })

        let format = voicePath.substring(voicePath.lastIndexOf('.') + 1);
        if (!format || format.length == 0) {
            format = 'pcm'
        }

        const fileSystemManager = wx.getFileSystemManager()
        fileSystemManager.readFile({
            filePath: voicePath,
            encoding: "base64",
            success(res){
                var base64VoiceData = res.data;
                var reqData = {
                    "format": format, "len": voiceSize, "speech": base64VoiceData,
                    "rate": 16000, "dev_pid": 80001, "channel": 1,
                    "token": token, "cuid": "baidu_workshop",
                }
                wx.request({
                    url: 'https://vop.baidu.com/pro_api',
                    method: 'post',
                    data: reqData,
                    success (resp) {
                        wx.hideLoading();
                        recognizeVoiceCallback(resp ? resp.data : "")
                    },
                    fail(error){
                        wx.hideLoading();
                        wx.showToast({ icon: 'none', title: '请求失败了，请确保网络正常，重新试试~', })
                    }
                })
            },
            fail(res){
                wx.hideLoading();
                wx.showToast({ title: '未知原因造成获取语音失败，再试一次?', icon: "none" })
            }
        })
    },
    /// camera
    onBindCamera: function () {
        let self = this;
        try {
            checkPermissionUtil.checkPermission('scope.camera').then(function (res) {
                wx.chooseImage({
                    count: 1,
                    sizeType: ['original', 'compressed'],
                    sourceType: ['album', 'camera'],
                    success: function (res) {
                        self.recognizeImage(self.accessToken, res.tempFilePaths[0], self.recognizeImageCallback)
                    }
                })
            }, function (err) {
                wx.showToast({ title: '授权失败，无法使用该功能~', icon: 'none' });
            });
        } catch (err) {
            wx.showToast({ title: '授权失败，无法使用该功能~', icon: 'none' });
        }
    },
    recognizeImageCallback: function(imgItems) {
        if (!imgItems) {
            wx.showToast({ icon: 'none', title: '没有认出来，可以再试试~', })
            return
        }
        wx.showActionSheet({
            itemList: imgItems, // imgItems must be an array, eg. [A, B, C]
            success (res) {
              console.log(res.tapIndex)
              self.doTextSearch(imgItems[res.tapIndex])
            },
            fail (res) {
              console.log(res.errMsg)
            }
        })
    },
    recognizeImage: function (token, imagePath, fnCallback) {
        wx.showLoading({ title: '图片识别中...', })
        console.debug("ready to open: ", imagePath)
        wx.getFileSystemManager().readFile({
            filePath: imagePath,
            encoding: "base64",
            success: res => {
                wx.request({
                    url: 'https://aip.baidubce.com/rest/2.0/image-classify/v2/advanced_general?access_token=' + token,
                    method: 'post',
                    data: { "image": res.data },
                    header: { "content-type": "application/x-www-form-urlencoded", },
                    success (resp) {
                        wx.hideLoading();
                        fnCallback(resp && resp.data ? resp.data.result : "")
                    },
                    fail(error){
                        wx.hideLoading();
                        wx.showToast({ icon: 'none', title: '请求失败了，请确保网络正常，重新试试~', })
                    }
                });
            },
            fail: res => {
                wx.hideLoading()
                wx.showToast({ title: '拍照失败,未获取相机权限或其他原因', icon: "none" })
            }
        })
    },
})
