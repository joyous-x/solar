class GameSprite extends SceneBase {
    private level: number;
    private ball: egret.Bitmap;
    private goal: egret.Bitmap;
    private bulb: egret.Sprite;
    private startArrow: egret.Bitmap;
    private startMClip: egret.MovieClip;
    private baffles: egret.DisplayObject[];
    private decorator: SceneDecorator;
    private gameStage: egret.Sprite;
    private ballSpeed: number;
    private clickable = ['blue.png', 'green.png', 'pink.png'];
    private single = ['gear.png', 'ring.png', 'arrow.down.png', 'arrow.up.png', 'arrow.left.png', 'arrow.right.png'];
    private infoText: egret.TextField;
    private lifeText: egret.TextField;

    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
        this.addEventListener(egret.Event.REMOVED_FROM_STAGE, this.onRemoveFromStage, this);
    }

    private onAddToStage(event: egret.Event) {
        this.fillGameScene();
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

    private onRemoveFromStage(event: egret.Event) {
        this.onTapStop();
    }

    private createBitmapByName(name: string, width: number = 0, height: number = 0) {
        let result = new egret.Bitmap();
        let texture: egret.Texture = RES.getRes(name);
        result.texture = texture;
        if (width != 0 && height != 0) {
            result.width = width;
            result.height = height;
            result.fillMode = egret.BitmapFillMode.SCALE;
            result.anchorOffsetX = result.width * .5;
            result.anchorOffsetY = result.height * .5;
        } else {
            result.anchorOffsetX = texture.textureWidth * .5;
            result.anchorOffsetY = texture.textureHeight * .5;
        }
        result.touchEnabled = true;
        return result;
    }

    private fillGameScene() {
        const stage = {width: this.parent.stage.stageWidth, height: this.parent.stage.stageHeight}
        const design = {width: 1440, height: 2560};
        const expect = {width: 1080, height: 1920};
        const scale = Math.min(1, Math.min(stage.width, expect.width) / design.width, Math.min(stage.height, expect.height) / design.height);

        let gameStage = new egret.Sprite();
        gameStage.width = design.width * .76;
        gameStage.height = design.height * .76;
        gameStage.x = (stage.width - gameStage.width * scale) * .5;
        gameStage.y = (stage.height - gameStage.height * scale) * .6;
        gameStage.scaleX = gameStage.scaleY = scale;
        this.gameStage = gameStage;

        gameStage.graphics.lineStyle( 4, 0x00ff00, 0.1 );
        gameStage.graphics.beginFill( 0xff0000, 0);
        gameStage.graphics.drawRect(0, 0, gameStage.width, gameStage.height);
        gameStage.graphics.endFill();

        let statHeight = Math.min(160, stage.height - gameStage.y - gameStage.height * scale);
        this.showStatistics(stage.width, statHeight);
    }

    private fillGameStage(levelCfg) {
        this.bulb = this.createBitmapWithNum("icon.bulb.png", LocalStore.instance.getHints(), true)
        this.addChild(this.bulb);
        this.bulb.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapBulb.bind(this), this);

        this.ball = this.createBitmapByName("ball.png");
        this.gameStage.addChild(this.ball);

        this.goal = this.createBitmapByName("end.png");
        this.gameStage.addChild(this.goal);

        let startMClip = McCenter.instance.newMovieClip("start.mc");
        startMClip.frameRate = startMClip.frameRate * 0.9;
        startMClip.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapStart.bind(this), this);
        this.startMClip = startMClip;
        this.gameStage.addChild(startMClip);

        this.startArrow = this.createBitmapByName("start.core.png");
        this.gameStage.addChild(this.startArrow);

        let baffles: egret.DisplayObject[] = new Array();
        for ( let i = 0; i < levelCfg.baffles.length; i++) {
            let frame = levelCfg.baffles[i].frame,
                baffleX = levelCfg.baffles[i].x,
                baffleY = levelCfg.baffles[i].y,
                baffleAngle = levelCfg.baffles[i].angle,
                baffleShadow = levelCfg.baffles[i].shadow; // 能够通过关卡的正确方向
            let tmp = this.createBitmapByName(frame);
            if (this.single.indexOf(frame) === -1) {
                let mcBaffle = McCenter.instance.newMovieClip(frame.replace(".png", ".mc"));
                mcBaffle.anchorOffsetX = mcBaffle.width * .5;
                mcBaffle.anchorOffsetY = mcBaffle.height * .5;
                mcBaffle.scaleX = tmp.scaleX * 2, mcBaffle.scaleY = tmp.scaleY * 2; // 图片被缩小了，所以此处放大还原
                mcBaffle.frameRate = mcBaffle.frameRate * 4;
                mcBaffle.touchEnabled = false;
                (tmp as any).mclip = mcBaffle;
            }

            if (frame === 'ring.png') {
                const action = egret.Tween.get(tmp, {
                    loop: true,
                    onChange: (event) => {
                    },
                }).to({rotation:Utils.instance.normalAngle(tmp.rotation + 0.1 * 360 / Math.PI)}, 300).call(() => {
                });
            }

            tmp.x = baffleX, tmp.y = baffleY;
            tmp.name = frame;
            tmp.touchEnabled = this.clickable.indexOf(frame) !== -1;
            if (baffleAngle) {
                tmp.rotation = baffleAngle;
            }
            if (baffleShadow) {
                (tmp as any).shadow = baffleShadow;
            }
            (tmp as any).type = 'baffle', (tmp as any).collidable = true;
            baffles.push(tmp);
            tmp.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTapBaffle.bind(this, frame, i), this);
        }
        baffles.forEach(item => {
            this.gameStage.addChild(item);
            if ((item as any).mclip) {
                let mclip = (item as any).mclip as egret.MovieClip;
                this.gameStage.addChild(mclip);
            }
        });
        this.baffles = baffles;
    }

    private resetGameStage(levelCfg) {
        this.bulb.tint = 0x00cbfa
        this.bulb.touchEnabled = true
        this.bulb.anchorOffsetX = this.bulb.width * .5;
        this.bulb.anchorOffsetY = this.bulb.height * .5;
        if ((this.stage.height / this.stage.width) > (this.gameStage.height / this.gameStage.width)) {
            this.bulb.y = this.decorator.y + this.decorator.height * .5;
            this.bulb.x = this.decorator.width * .67;
        } else {
            this.bulb.y = this.gameStage.y * 1.2;
            this.bulb.x = this.gameStage.x * .5;
        }
        if ((this.bulb as any).tween) {
            (this.bulb as any).tween.play();
        }

        this.ball.x = levelCfg.start.x;
        this.ball.y = levelCfg.start.y;
        this.ball.rotation = 0;

        this.goal.x = levelCfg.end.x;
        this.goal.y = levelCfg.end.y;

        let startMClip = this.startMClip;
        startMClip.x = levelCfg.start.x;
        startMClip.y = levelCfg.start.y;
        startMClip.anchorOffsetX = startMClip.width * .5;
        startMClip.anchorOffsetY = startMClip.height * .5;
        startMClip.stop(), startMClip.gotoAndPlay(0,-1);
        startMClip.touchEnabled = true;

        this.startArrow.x = levelCfg.start.x;
        this.startArrow.y = levelCfg.start.y;
        this.startArrow.touchEnabled = false;

        for (let i = 0; i < this.baffles.length; i++) {
            let baffle = this.baffles[i];
            baffle.x = levelCfg.baffles[i].x;
            baffle.y = levelCfg.baffles[i].y;
            if (levelCfg.baffles[i].angle) {
                baffle.rotation = levelCfg.baffles[i].angle;
            }
            baffle.visible = true;
            (baffle as any).collidable = true;
            if ((baffle as any).mclip) {
                let mclip = (baffle as any).mclip as egret.MovieClip;
                mclip.rotation = baffle.rotation;
                mclip.x = baffle.x;
                mclip.y = baffle.y;
            }
        }

        this.ballTrailPool = [];
        this.ballLastPosition = null;

        this.updateStatistics();
    }

    private fillDecorator() {
        let levelDetail = SceneManager.instance.parseLevel(this.level)
        let decorator = new SceneDecorator();
        decorator.show(0, 0, this.parent.stage.stageWidth, this.gameStage.y, {scene: levelDetail.scene, level: this.level});
        this.addChild(decorator);        
        this.decorator = decorator;
    }

    private getLevelCfg(level: number) {
        let levelDetail = SceneManager.instance.parseLevel(level)
        let levelCfg;
        switch(levelDetail.scene) {
            case LangCenter.instance.sceneEasy:
                levelCfg = CfgEasy.data[levelDetail.level]; break;
            case LangCenter.instance.sceneMidd:
                levelCfg = CfgMidd.data[levelDetail.level]; break;
            case LangCenter.instance.sceneHard:
                levelCfg = CfgHard.data[levelDetail.level]; break;
        }
        return levelCfg;
    }

    public sceneDetail() {
        let levelDetail = SceneManager.instance.parseLevel(this.level);
        return {scene: levelDetail.scene, level: this.level}
    }

    public updateShow(opt = {scene: "", level: 0}) {
        this.level = opt.level;
        let levelCfg = this.getLevelCfg(opt.level);
        this.removeChildren();
        this.addChild(this.gameStage);
        this.fillDecorator();
        this.fillGameStage(levelCfg);
        this.resetGameStage(levelCfg);
    }

    private onTapStart() {
        if (LocalStore.instance.getLifes() < 1) {
            new EasyToast().show(this, LangCenter.instance.getLifePrompt, ()=>{
                SceneManager.instance.navigateBack();
            }, ()=>{
                this.earnLife();
                this.updateStatistics();
            });
            return ;
        }
        this.ballSpeed = 18;
        this.startMClip.touchEnabled = false;
        this.baffles.forEach( item => { item.touchEnabled = false; })
        egret.startTick(this.ticker, this);
    }

    private onTapStop() {
        egret.stopTick(this.ticker, this);
        if ((this.bulb as any).tween) {
            (this.bulb as any).tween.pause();
        }        
        this.startMClip.touchEnabled = true;
        this.baffles.forEach( item => { item.touchEnabled = this.clickable.indexOf(item.name) !== -1; })
    }

    private onTapBulb() {
        this.bulb.touchEnabled = false;
        const hints = LocalStore.instance.getHints();
        if (hints < 1) {
            new EasyToast().showEx(this, {
                    headImg: "tips.png", 
                    desc: LangCenter.instance.noHintPrompt, 
                    headStr: "", lHandler: undefined, rHandler: undefined, done: ()=>{}});
            // this.earnHint();
        } else {
            new EasyToast().show(this, LangCenter.instance.useHintPrompt, ()=>{
            }, ()=>{
                LocalStore.instance.setHints(hints - 1);
                this.showHint();
                this.updateStatistics();
            });
        }
        this.bulb.touchEnabled = true;
    }

    private onTapBaffle(frame: string, index: number) {
        let baffle = this.baffles[index];
        if (baffle) {
            baffle.rotation = Utils.instance.normalAngle(baffle.rotation + 90);
            if ((baffle as any).mclip) {
                let mclip = (baffle as any).mclip as egret.MovieClip;
                mclip.rotation = baffle.rotation;
            }
            SoundCenter.instance.voice("tap");
        }
        if (this.bulb.touchEnabled === false) {
            this.bulb.touchEnabled = true;
            if ((this.bulb as any).tween) {
                (this.bulb as any).tween.play();
            }           
        } 
    }

    private lastTimeStamp: number;
    private ticker(timeStamp: number) : boolean {
        if (this.lastTimeStamp === 0) {
            this.lastTimeStamp = timeStamp;
        } else {
            if ((timeStamp - this.lastTimeStamp) < 16) { 
                return;
            } else {
                this.lastTimeStamp += 16;
            }
        }

        this.ballTrail();
        this.hitDetect();
        return false;
    }

    private hitDetect() {
        if (Utils.instance.distance(this.ball, this.goal) < this.ballSpeed) {
            return this.gameWin();
        }

        let stage = this.ball.parent;
        if (0 > this.ball.x || stage.width < this.ball.x || 0 > this.ball.y || stage.height < this.ball.y) {
            return this.gameLose();
        }

        for (let i = 0; i < this.baffles.length; i++) {
            let curBaffle = this.baffles[i];
            if (!(curBaffle as any).collidable) {
                continue;
            }
            const distance = Utils.instance.distance(curBaffle, this.ball);
            if (distance <= this.ballSpeed && !(curBaffle as any).hold) {
                (curBaffle as any).hold = true;
                this.hitHandler(curBaffle);
                if (curBaffle.name != "ring.png") {
                    this.ball.x = curBaffle.x;
                    this.ball.y = curBaffle.y;
                }                
            } else if (distance > 52 && (curBaffle as any).hold) {
                (curBaffle as any).hold = false;
            }
        }
    }

    // 球 and 拖尾
    private ballLastPosition: {x:number, y:number};
    private ballTrailPool: egret.Bitmap[] = new Array();
    private ballTrail() {
        this.ball.x += Math.sin(this.ball.rotation * Math.PI / 180) * this.ballSpeed;
        this.ball.y -= Math.cos(this.ball.rotation * Math.PI / 180) * this.ballSpeed;

        if (!this.ballLastPosition) {
            this.ballLastPosition = {x: this.ball.x, y: this.ball.y};
            return
        }        
        if (Utils.instance.distance(this.ball, this.ballLastPosition) < 34) return ;

        let trail = this.ballTrailPool.pop() || Utils.instance.createBitmapByName("trail.png");
        trail.alpha = 1;
        trail.x = this.ballLastPosition.x, trail.y = this.ballLastPosition.y;
        this.ball.parent.addChild(trail);
        this.ballLastPosition = {x: this.ball.x, y: this.ball.y};

        var trailAnimation = egret.Tween.get(trail, {
            loop: false,
            onChange: (event) => {
            },
        }).to({alpha:0}, 500).call(() => {
            this.ball.parent.removeChild(trail);
            this.ballTrailPool.push(trail);
        });
    }

    private hitHandler(baffle: egret.DisplayObject) {
        if ((baffle as any).mclip) {
            let mclip = (baffle as any).mclip as egret.MovieClip;
            mclip.visible = true;
            mclip.gotoAndPlay(0, 1);
        }

        switch(baffle.name) {
            case 'gear.png':
            case 'pink.png':
            case 'blue.png': {
                const newAngle = Utils.instance.catapultAngle(Utils.instance.normalAngle(this.ball.rotation), Utils.instance.normalAngle(baffle.rotation))
                this.ball.rotation = newAngle;
                break;
            }
            case 'green.png': {
                const m = Utils.instance.normalAngle(baffle.rotation);
                const n = Utils.instance.normalAngle(this.ball.rotation);
                let delta = (m - n + 360) % 360;
                if (delta === 45 || delta === 135) {
                    delta === 45 ? this.ball.rotation += 90 : null;
                    delta === 135 ? this.ball.rotation -= 90 : null;
                }
                break;
            }
            case 'triangle.png': {
                const m = Utils.instance.normalAngle(baffle.rotation);
                const n = Utils.instance.normalAngle(this.ball.rotation);
                let delta = m - n;
                if (delta == 0 || delta == 360) {
                    this.ball.rotation += 90;
                } else if (delta == -270 || delta == 90) {
                    this.ball.rotation -= 90;
                } else {
                    this.ball.rotation += 180
                }
                break;
            }
            case 'ring.png': {
                let nexts = this.baffles.filter(item => item.name === 'ring.png' && item !== baffle)
                if (nexts.length > 0) {
                    let next = nexts[0];
                    (next as any).hold = true;
                    this.ball.x = next.x, this.ball.y = next.y;
                }
                break;
            }
            case 'square.png':
            case 'square.once.png':
            case 'arrow.up.png':
            case 'arrow.left.png':
            case 'arrow.down.png':
            case 'arrow.right.png': {
                this.ball.rotation += 180;
                break;
            }
        }

        switch(baffle.name) {
            case 'gear.png': {
                const action = egret.Tween.get(baffle, {
                    loop: false,
                    onChange: (event) => {
                    },
                }).to({rotation:baffle.rotation + 90}, 300).call(() => {
                });
                SoundCenter.instance.voice("collide.gear");
                break;
            }
            case 'arrow.up.png':
            case 'arrow.down.png':
            case 'arrow.left.png':
            case 'arrow.right.png': {
                const tx = baffle.name === 'arrow.left.png' ? -200 : baffle.name === 'arrow.right.png' ? 200 : 0;
                const ty = baffle.name === 'arrow.up.png' ? -200 : baffle.name === 'arrow.down.png' ? 200 : 0;
                const action = egret.Tween.get(baffle, {
                    loop: false,
                    onChange: (event) => {
                    },
                }).to({x: baffle.x + tx, y: baffle.y + ty}, 300).call(() => {
                });
                SoundCenter.instance.voice("collide.arrow");
                break;
            }
            case 'square.once.png':
            case 'pink.png': {
                (baffle as any).collidable = false;
                const action = egret.Tween.get(baffle, {
                    loop: false,
                    onChange: (event) => {
                    },
                }).to({}, 200).call(() => {
                    baffle.visible = false;
                    if ((baffle as any).mclip) {
                        let mclip = (baffle as any).mclip as egret.MovieClip;
                        mclip.visible = false;
                    }
                });
                SoundCenter.instance.voice("collide.once");
                break;
            }
            case 'ring.png': {
                SoundCenter.instance.voice("transfer");
                break;
            }
            case 'green.png': {
                SoundCenter.instance.voice("collide.green");
                break;
            }
            default: {
                SoundCenter.instance.voice("collide");
                break;
            }
        }
    }

    private gameRestart() {
        LocalStore.instance.setLifes(LocalStore.instance.getLifes() - 1);
        this.resetGameStage(this.getLevelCfg(this.level));
    }

    private gameWin() {
        SoundCenter.instance.voice("win");
        this.onTapStop();

        let finalDo = ()=>{
            if (this.level < 75) {
                egret.Tween.get(this).to({x:this.width}, 300, egret.Ease.quadOut).call(() => {
                    SceneManager.instance.navigateTo(LangCenter.instance.sceneGame, {level: nextLevel, pop: true});
                });
            } else {
                SceneManager.instance.navigateBack();
            }
        }

        let nextLevel = this.level + 1;
        if (LocalStore.instance.getUnlock() === this.level) {
            LocalStore.instance.setUnlock(nextLevel);
            let curLevelDetail = SceneManager.instance.parseLevel(this.level);
            let newLevelDetail = SceneManager.instance.parseLevel(nextLevel);
            if (curLevelDetail.scene !== newLevelDetail.scene) {
                new EasyToast().showEx(this, {headImg: "cup.png", 
                    desc: LangCenter.instance.passAllLevelPrompt, 
                    headStr: "", 
                    done: ()=>{
                        finalDo();
                    },
                    lHandler: undefined, rHandler: undefined});
                return ;
            }
        }
        finalDo();
    }

    private gameLose() {
        SoundCenter.instance.voice("fail");
        this.onTapStop();
        new EasyToast().show(this, LangCenter.instance.tryAgainPrompt, ()=>{
            SceneManager.instance.navigateBack();
        }, ()=>{
            this.gameRestart();
        });
    }

    private showHint() {
        if ((this.bulb as any).tween) {
            (this.bulb as any).tween.pause();
        }         

        this.bulb.touchEnabled = false;
        this.startMClip.frameRate = this.startMClip.frameRate * 0.5;
        for (let i = 0; i < this.baffles.length; i++) {
            let baffle = this.baffles[i];
            let shadow = (baffle as any).shadow;
            if (!shadow) {
                continue;
            }
            let mclip: egret.MovieClip;
            if ((baffle as any).mclip) {
                mclip = (baffle as any).mclip as egret.MovieClip;
                mclip.visible = false;
            }
            baffle.touchEnabled = false;
            const action = egret.Tween.get(baffle, {
                loop: false,
                onChange: (event) => {
                },
            }).to({rotation: shadow}, 1000, egret.Ease.elasticInOut).call(() => {
                baffle.touchEnabled = true;
                if (mclip) {
                    mclip.visible = true;
                    mclip.rotation = shadow;
                }
            });
        }
    }

    private earnHint() {
        showNativeAd("hint");
    }

    private earnLife() {
        showNativeAd("life");
        console.debug("---> earnLife: complete")
    }

    private createBitmapWithNum(img: string, num: number, tween: boolean = false, width: number = 0, height: number = 0) {
        let stage = new egret.Sprite();
        let bitmap = Utils.instance.createBitmapByName(img);
        let txt = Utils.instance.createTextField(num.toString(), 28, 0xFFFFFF, Math.max(bitmap.width, bitmap.height) * .46);

        if (width === 0 && height === 0) {
            stage.width  = bitmap.width * 1.5;
            stage.height = bitmap.height * 1.5;
        } else {
            stage.width = width;
            stage.height = height;
        }
        
        bitmap.x = stage.width * .5, bitmap.y = stage.height * .5;
        txt.x = stage.width * .86, txt.y = stage.height * .2;

        stage.graphics.beginFill(0x0099ff, 0.68);
        stage.graphics.drawCircle(txt.x, txt.y, Math.max(txt.width, txt.height) * .68);
        stage.graphics.endFill();

        bitmap.touchEnabled = false;
        txt.touchEnabled = false;
        (stage as any).text = txt;

        if (tween) {
            let obj = {r: 0, g: 203, b: 250, p: bitmap};
            let imgTween = egret.Tween.get(obj, {
                loop:true,
                onChangeObj: obj,
                onChange: function (v: egret.Event) {
                    this.p.tint = (this.r << 16) + (this.g << 8) + this.b;
                },
            }).to({r: 237, g: 70, b: 48}, 1000).call(function () {
            });
            (stage as any).tween = imgTween;
        }
        
        stage.addChild(bitmap);
        stage.addChild(txt);
        return stage;
    }

    private showStatistics(width: number, height: number) {
        let x = 0, y = this.parent.stage.stageHeight - height;
        if (this.infoText === undefined) {
            let infoText = Utils.instance.createTextField("Easy - 1", 28);
            infoText.x = x + width * .33, infoText.y = y + height * .5;
            this.infoText = infoText;        
            this.addChild(infoText);    
        }
        if (this.lifeText === undefined) {
            let lifeText = Utils.instance.createTextField("3", 28);
            lifeText.x = x + width * .67, lifeText.y = y + height * .5;
            this.lifeText = lifeText;
            this.addChild(lifeText);
        }
    }

    private updateStatistics() {
        if (!this.contains(this.infoText)) {
            this.addChild(this.infoText);
        }
        if (!this.contains(this.lifeText)) {
            this.addChild(this.lifeText);
        }
        let sceneDetail = SceneManager.instance.parseLevel(this.level);
        let infoStr = "Level: " + sceneDetail.scene + " - " + sceneDetail.level.toString();
        let lifeStr = "Life: " + LocalStore.instance.getLifes();
        this.infoText.text = infoStr;
        this.lifeText.text = lifeStr;
        if ((this.bulb as any).text) {
            let hintNum = LocalStore.instance.getHints();
            (this.bulb as any).text.text = hintNum.toString();
        }
    }
}