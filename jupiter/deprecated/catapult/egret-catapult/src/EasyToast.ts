class EasyToast extends egret.DisplayObjectContainer {
    private container: egret.DisplayObjectContainer;

    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {
    }

    public showEx(parent: egret.DisplayObjectContainer, opt: {
                        headStr: string, 
                        headImg: string, 
                        desc: string,
                        lHandler: Function, 
                        rHandler: Function,
                        done: Function }) {
        let stageW = parent.stage.stageWidth;
        let stageH = parent.stage.stageHeight;

        let backgroud = new egret.Shape();
        this.addChild(backgroud);
        backgroud.graphics.beginFill(0xcccccc, 0.5);
        backgroud.graphics.drawRect(0, 0, stageW, stageH);
        backgroud.graphics.endFill();
        backgroud.touchEnabled = true;
        backgroud.addEventListener(egret.TouchEvent.TOUCH_TAP, (e: egret.Event) => {
                this.onTabBtn();
                opt.done && opt.done();
        }, this);

        this.container = new egret.DisplayObjectContainer();
        this.container.width = stageW * .78;
        this.container.height = this.container.width * .46;
        this.addChild(this.container);

        if (opt.headImg && opt.desc) {
            let h = Utils.instance.createBitmapByName(opt.headImg);
            let d = Utils.instance.createTextField(opt.desc, 28);
            this.container.addChild(d);
            this.container.addChild(h);

            if (h.height > h.width * 1.2) {
                h.parent.height = (h.height + d.height) * 2;
                h.parent.width = Math.max(h.parent.height * .68, d.width * 1.1);
            } else {
                h.parent.height = (h.height + d.height) * 2;
            }
            h.x = h.parent.width * .5;
            h.y = h.parent.height * .23 + h.height * .5;
            h.touchEnabled = false;
            
            d.stroke = 1;
            d.x = d.parent.width * .5;
            d.y = h.parent.height * .3 + h.height + d.height * .5;
            d.touchEnabled = false;
        }

        if (opt.headStr) {
            let head = Utils.instance.createTextField(opt.headStr, 32);
            this.container.addChild(head);
            head.x = head.parent.width * .5;
            head.y = head.parent.height * .23;
            head.stroke = 2;
            head.touchEnabled = false;
            //head.strokeColor = 0xccffff;
        }

        if (opt.lHandler && opt.rHandler) {
            let l = Utils.instance.createBitmapByName("false.png");
            let r = Utils.instance.createBitmapByName("true.png");
            l.x = this.container.width * .3, l.y = this.container.height * .67;
            r.x = this.container.width * .72, r.y = this.container.height * .67;
            l.addEventListener(egret.TouchEvent.TOUCH_TAP, (e: egret.Event) => {
                opt.lHandler(e);
                this.onTabBtn();
            }, this);
            r.addEventListener(egret.TouchEvent.TOUCH_TAP, (e: egret.Event) => {
                opt.rHandler(e);
                this.onTabBtn();
            }, this);
            this.container.addChild(l);
            this.container.addChild(r);
        }        
        
        this.container.x = (stageW - this.container.width) * .5;
        this.container.y = (stageH - this.container.height) * .5 - 120;

        let containerBg = new egret.Shape();
        this.container.addChildAt(containerBg, 0);
        containerBg.graphics.beginFill(0x333333, 0.9);
        containerBg.graphics.drawRoundRect(0, 0, this.container.width, this.container.height, 15);

        var tw = egret.Tween.get( this );
        tw.to( {scaleX:0, scaleY:0, x: stageW * .5, y: stageH}, 1 )
          .to( {scaleX:1, scaleY:1, x: 0, y: 0}, 200, egret.Ease.quadInOut)
          .call(() => {
          });
        
        parent.addChild(this);
    }

    public show(parent: egret.DisplayObjectContainer, title: string, lHandler: Function, rHandler: Function) {
        let stageW = parent.stage.stageWidth;
        let stageH = parent.stage.stageHeight;

        let backgroud = new egret.Shape();
        this.addChild(backgroud);
        backgroud.graphics.beginFill(0xcccccc, 0.5);
        backgroud.graphics.drawRect(0, 0, stageW, stageH);
        backgroud.graphics.endFill();
        backgroud.touchEnabled = true;

        this.container = new egret.DisplayObjectContainer();
        this.container.width = stageW * .78;
        this.container.height = this.container.width * .46;
        this.container.x = (stageW - this.container.width) * .5;
        this.container.y = (stageH - this.container.height) * .5 - 120;
        this.addChild(this.container);

        let containerBg = new egret.Shape();
        this.container.addChild(containerBg);
        containerBg.graphics.beginFill(0x333333, 0.9);
        containerBg.graphics.drawRoundRect(0, 0, this.container.width, this.container.height, 15);

        let head = Utils.instance.createTextField(title, 32);
        this.container.addChild(head);
        head.x = head.parent.width * .5;
        head.y = head.parent.height * .23;
        head.stroke = 2;
        //head.strokeColor = 0xccffff;

        let lDes = "", rDes = "";
        let l = new BgTextItem(); 
        let r = new BgTextItem();
        this.container.addChild(l);
        this.container.addChild(r);
        l.fillItem(lDes, "false.png", this.container.width * .3, this.container.height * .67, false);
        r.fillItem(rDes, "true.png", this.container.width * .72, this.container.height * .67, false);
        l.addEventListener(egret.TouchEvent.TOUCH_TAP, (e: egret.Event) => {
            lHandler(e);
            this.onTabBtn();
        }, this);
        r.addEventListener(egret.TouchEvent.TOUCH_TAP, (e: egret.Event) => {
            rHandler(e);
            this.onTabBtn();
        }, this);
        parent.addChild(this);

        var tw = egret.Tween.get( this );
        tw.to( {scaleX:0, scaleY:0, x: stageW * .5, y: stageH}, 1 )
          .to( {scaleX:1, scaleY:1, x: 0, y: 0}, 200, egret.Ease.quadInOut)
          .call(() => {
                l.touchEnabled = true;
                r.touchEnabled = true;
          });
    }

    private onTabBtn() {
        this.parent ? this.parent.removeChild(this) : 0;
    }
}