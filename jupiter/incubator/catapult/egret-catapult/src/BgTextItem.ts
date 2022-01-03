class BgTextItem extends egret.DisplayObjectContainer {
    private lock: egret.Bitmap;
    private entryBg: egret.Bitmap;
    private entryDes: egret.TextField; 


    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {
    }

    public animationIn(xFrom: number) {
        var tw = egret.Tween.get( this );
        tw.to( {scaleX:0, scaleY:0, x: xFrom}, 1 )
          .to( {scaleX:1, scaleY:1, x: this.x}, 200, egret.Ease.bounceInOut)
    }

    public animationOut(xTo: number, call: Function = ()=>{}, animation: boolean = true) {
        if (!animation) {
            return call()
        }
        var tw = egret.Tween.get( this );  
        tw.to( {scaleX:1, scaleY:1, x: this.x}, 1)
          .to( {scaleX:0, scaleY:0, x: xTo}, 600, egret.Ease.backInOut)
          .call(call)
    }

    public fillItem(name: string, bgRes: string, x: number, y: number, locked: boolean = true, width: number = 0, height: number = 0) {
        let entryBg  = this.createBitmapByName(bgRes, width, height);
        let entryDes = this.createTextField(name);
        if (width != 0 && height != 0) {
            entryBg.width = width;
            entryBg.height = height;
            entryBg.fillMode = egret.BitmapFillMode.SCALE;
        }
        this.width = entryBg.width;
        this.height = entryBg.height;
        this.x = x;
        this.y = y;
        this.addChild(entryBg);
        this.addChild(entryDes);
        this.entryBg = entryBg;
        this.entryDes = entryDes;
        if (locked) {
            let lock  = this.createBitmapByName("lock.png");
            lock.x = entryBg.width * .5 * .7, lock.y = - entryBg.height * .5 * .7;
            this.addChild(lock);
            this.lock = lock;
        }
    }

    public setLockVisable(locked: boolean) {
        if (this.lock) {
            this.lock.visible = locked;
        }
    }

    public setTextVisble(visible: boolean) {
        this.entryDes.$setVisible(visible)
    }

    public setBgVisble(visible: boolean) {
        this.entryBg.$setVisible(visible)
    }

    private createBitmapByName(name: string, width: number = 0, height: number = 0) {
        return Utils.instance.createBitmapByName(name, width, height);
    }

    private createTextField(des: string, size: number = 24, color: number = 0xffffff) {
        return Utils.instance.createTextField(des, size, color);
    }
}