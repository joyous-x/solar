//////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2014-present, Egret Technology.
//  All rights reserved.
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Egret nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY EGRET AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL EGRET AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA,
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////////////

class Main extends SceneBase {
    private entryEasy: BgTextItem;
    private entryMidd: BgTextItem;
    private entryHard: BgTextItem;
    private entrySetting: BgTextItem;

    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {

        egret.lifecycle.addLifecycleListener((context) => {
            // custom lifecycle plugin

            context.onUpdate = () => {

            }
        })

        egret.lifecycle.onPause = () => {
            egret.ticker.pause();
        }

        egret.lifecycle.onResume = () => {
            egret.ticker.resume();
        }

        this.runGame().catch(e => {
            console.log(e);
        })
    }

    private async runGame() {
        SceneManager.instance.setRoot(this);
        this.stage.scaleMode = egret.StageScaleMode.FIXED_NARROW;
        await this.loadResource()
        this.createGameScene();
        await platform.login();
        const userInfo = await platform.getUserInfo();
        console.log(userInfo);
    }

    private async loadResource() {
        try {
            //加载纹理集：https://developer.egret.com/en/article/index/id/135
            await RES.loadConfig("resource/default.res.json", "resource/");
            await RES.loadGroup("loading");
            const loadingView = new LoadingUI();
            this.stage.addChild(loadingView);
            await RES.loadGroup("preload", 0, loadingView);
            this.stage.removeChild(loadingView);
        }
        catch (e) {
            console.error(e);
        }
    }

    /**
     * 创建游戏场景
     * Create a game scene
     */
    private createGameScene() {
        let stageW = this.stage.stageWidth;
        let stageH = this.stage.stageHeight;

        let bg:egret.Bitmap = new egret.Bitmap(RES.getRes("loadingBG_jpg"));
        this.addChild(bg);
        bg.width = stageW;
        bg.height = stageH;

        let backgroud = new egret.Shape();
        this.addChild(backgroud);
        backgroud.graphics.beginFill(0x336699, 0.26);
        backgroud.graphics.drawRect(0, 0, stageW, stageH);
        backgroud.graphics.endFill();

        let entryEasy = new BgTextItem(); this.entryEasy = entryEasy;
        let entryMidd = new BgTextItem(); this.entryMidd = entryMidd;
        let entryHard = new BgTextItem(); this.entryHard = entryHard;
        let entrySetting = new BgTextItem(); this.entrySetting = entrySetting;
        entryEasy.fillItem(LangCenter.instance.easy, "circle.blue.png", stageW * .5, stageH * .2, false);
        entryMidd.fillItem(LangCenter.instance.midd, "circle.pink.png", stageW * .5, stageH * .4, true);
        entryHard.fillItem(LangCenter.instance.hard, "circle.yellow.png", stageW * .5, stageH * .6, true);
        entrySetting.fillItem("", "circle.setting.png", stageW * .5, stageH * .8, false);
        entryEasy.touchEnabled = true;
        entryMidd.touchEnabled = true;
        entryHard.touchEnabled = true;
        entrySetting.touchEnabled = true;
        entryEasy.addEventListener(egret.TouchEvent.TOUCH_TAP, this.clickMenu.bind(this, LangCenter.instance.sceneEasy), this);
        entryMidd.addEventListener(egret.TouchEvent.TOUCH_TAP, this.clickMenu.bind(this, LangCenter.instance.sceneMidd), this);
        entryHard.addEventListener(egret.TouchEvent.TOUCH_TAP, this.clickMenu.bind(this, LangCenter.instance.sceneHard), this);
        entrySetting.addEventListener(egret.TouchEvent.TOUCH_TAP, this.clickMenu.bind(this, LangCenter.instance.sceneSetting), this);
        this.updateShow();

        SoundCenter.instance.music();
    }

    public updateShow(opt = {scene: "", level: 0}) {
        this.entryMidd.setLockVisable(SceneManager.instance.isSceneLocked(LangCenter.instance.sceneMidd));
        this.entryHard.setLockVisable(SceneManager.instance.isSceneLocked(LangCenter.instance.sceneHard));
        this.addMainMenu();
    }

    private addMainMenu() {
        let stageW = this.stage.stageWidth;
        let stageH = this.stage.stageHeight;
        this.entryEasy.x = stageW * .5, this.entryEasy.y = stageH * .2;
        this.entryMidd.x = stageW * .5, this.entryMidd.y = stageH * .4;
        this.entryHard.x = stageW * .5, this.entryHard.y = stageH * .6;
        this.entrySetting.x = stageW * .5, this.entrySetting.y = stageH * .8;
        this.addChild(this.entryEasy);
        this.addChild(this.entryMidd);
        this.addChild(this.entryHard);
        this.addChild(this.entrySetting);
        this.entryEasy.animationIn(0)
        this.entryMidd.animationIn(this.stage.stageWidth)
        this.entryHard.animationIn(0)
        this.entrySetting.animationIn(this.stage.stageWidth)
    }

    private delMainMenu(afterDo: Function) {
        this.entryEasy.animationOut(0, ()=>{this.removeChild(this.entryEasy);});
        this.entryMidd.animationOut(this.stage.stageWidth, ()=>{this.removeChild(this.entryMidd);});
        this.entryHard.animationOut(0, ()=>{this.removeChild(this.entryHard);});
        this.entrySetting.animationOut(this.stage.stageWidth, ()=>{this.removeChild(this.entrySetting); afterDo();});
    }

    private clickMenu(scene: string){
        SoundCenter.instance.voice("tap");
        if (SceneManager.instance.isSceneLocked(scene)) {
            new EasyToast().show(this, LangCenter.instance.sceneLockedPrompt, ()=>{
            }, ()=>{
            });
            return 
        }
        // afterDo 会被添加到 delMainMenu 中的最后一个动画之后执行
        var afterDo = () => {
            SceneManager.instance.navigateTo(scene);
        }
        this.delMainMenu(afterDo)
    }
}