class SettingSprite extends SceneBase {
    private decorator: SceneDecorator;
    private musicOn: egret.Bitmap;
    private musicOff: egret.Bitmap;
    private voiceOn: egret.Bitmap;
    private voiceOff: egret.Bitmap;
    private share: egret.Bitmap;
    private selected: egret.Bitmap;

    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {
        egret.lifecycle.addLifecycleListener((context) => {
            context.onUpdate = () => {
            }
        })
        egret.lifecycle.onPause = () => {
            egret.ticker.pause();
        }
        egret.lifecycle.onResume = () => {
            egret.ticker.resume();
        }
    }

    public updateShow(opt = {scene: "", level: 0}) {
        const stage = {width: this.parent.stage.stageWidth, height: this.parent.stage.stageHeight}
        const itemSize = 120, itemGap = 50;
        const column = 2, row = 2;
        let startX = (stage.width - (column - 1) * (itemGap + itemSize)) * .5;
        let startY = (stage.height - (row - 1) * (itemGap + itemSize)) * .5;
        
        let selected = Utils.instance.createBitmapByName("circle.cue.png");
        selected.visible = false;
        selected.touchEnabled = false;
        this.selected = selected;

        let musicOn = Utils.instance.createBitmapByName("setting.music.png");
        musicOn.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapMusic.bind(this), this);

        let musicOff = Utils.instance.createBitmapByName("setting.music.off.png");
        musicOff.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapMusic.bind(this), this);

        let voiceOn = Utils.instance.createBitmapByName("setting.voice.png");
        voiceOn.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapVoice.bind(this), this);

        let voiceOff = Utils.instance.createBitmapByName("setting.voice.off.png");
        voiceOff.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapVoice.bind(this), this);

        let share = Utils.instance.createBitmapByName("setting.share.png");
        share.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapShare.bind(this), this);

        musicOn.x = startX, musicOn.y = startY;
        musicOff.x = startX, musicOff.y = startY;
        this.musicOn = musicOn, this.musicOff = musicOff;
        this.switchMusicTo(LocalStore.instance.getSwitchMusic(), false);

        voiceOn.x = startX + itemGap + itemSize, voiceOn.y = startY;
        voiceOff.x = startX + itemGap + itemSize, voiceOff.y = startY;
        this.voiceOn = voiceOn, this.voiceOff = voiceOff;
        this.switchVoiceTo(LocalStore.instance.getSwitchVoice(), false)

        share.x = startX, share.y = startY + itemGap + itemSize;
        share.touchEnabled = true;
        this.share = share;

        this.addChild(musicOn), this.addChild(musicOff);
        this.addChild(voiceOn), this.addChild(voiceOff);
        this.addChild(share);     
        this.addChild(selected);   

        let decorator = new SceneDecorator();
        decorator.show(0, 0, stage.width, stage.height * .2);
        this.addChild(decorator);        
        this.decorator = decorator;
    }

    private selectTween(x: number, y: number, callback: Function) {
        this.selected.x = x;
        this.selected.y = y;
        this.selected.alpha = 0;
        this.selected.visible = true;
        const action = egret.Tween.get(this.selected, {
            onChange: (event: egret.Event) => {
            },
        }).to({alpha: 1}, 100)
          .to({alpha: 0}, 100)
          .to({alpha: 1}, 100, egret.Ease.sineInOut).call(() => {
            this.selected.visible = false;
            callback && callback();
        });
    }

    private switchTween(from: egret.Bitmap, to: egret.Bitmap, tween: boolean, callback: Function) {
        let willDo = () => {
            from.visible = false, to.visible = true;
            from.touchEnabled = false, to.touchEnabled = true;
            callback && callback();
        }
        if (tween) {
            const action = egret.Tween.get(from, {
                onChange: (event: egret.Event) => {
                },
            }).to({alpha: 1}, 200).call(() => {
                willDo();
            });
        } else {
            willDo();
        }        
    }

    private switchMusicTo(on: boolean, tween: boolean = true, callback: Function = () => {}) {
        let from = this.musicOff, to = this.musicOn;
        if (!on) {
            from = this.musicOn, to = this.musicOff;
        }
        this.switchTween(from, to, tween, callback);
        if (tween) {
            this.selectTween(from.x, from.y, undefined);
        }        
    }

    private switchVoiceTo(on: boolean, tween: boolean = true, callback: Function = () => {}) {
        let from = this.voiceOff, to = this.voiceOn;
        if (!on) {
            from = this.voiceOn, to = this.voiceOff;
        }
        this.switchTween(from, to, tween, callback);
        if (tween) {
            this.selectTween(from.x, from.y, undefined);
        }        
    }

    private onTapMusic() {
        SoundCenter.instance.voice("tap");
        let on = !LocalStore.instance.getSwitchMusic();
        this.switchMusicTo(on, true, () => {
            LocalStore.instance.setSwitchMusic(on);
            SoundCenter.instance.music({play: on});
        });
    }

    private onTapVoice() {
        SoundCenter.instance.voice("tap");
        let on = !LocalStore.instance.getSwitchVoice();
        this.switchVoiceTo(on, true, () => {
            LocalStore.instance.setSwitchVoice(on);
        });
    }

    private onTapShare() {
        SoundCenter.instance.voice("tap");
        this.selectTween(this.share.x, this.share.y, undefined);
        //> TODO
    }
}