class SceneDecorator extends egret.DisplayObjectContainer {
    private btnBack: egret.Bitmap;

    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {
    }

    public show(x: number, y: number, width: number = 0, height: number = 0, opt = {scene: "", level: 0}) {
        height = Math.min(height, 160);
        let btnBack = Utils.instance.createBitmapByName("icon.back.png");
        btnBack.x = x + width * .07;
        btnBack.y = y + height * .5;
        btnBack.touchEnabled = true;
        btnBack.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTabBack.bind(this, btnBack.x, btnBack.y, opt), this);
        this.addChild(btnBack);
        this.btnBack = btnBack;

        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    public onTabBack(x:number, y:number, opt = {scene: "", level: 0}) {
        var tw = egret.Tween.get( this.btnBack );
        tw.to({x:-this.btnBack.width}, 300, egret.Ease.bounceOut).call(() => {
            this.btnBack.x = x;
            SceneManager.instance.navigateBack();
        })
    }
}