class LevelSelectorItem extends egret.DisplayObjectContainer {
    private bg: egret.DisplayObject;
    private lock: egret.Bitmap;
    private desc: egret.TextField; 


    public constructor() {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
    }

    private onAddToStage(event: egret.Event) {
    }

    private drawHexagon(x: number, y: number, width: number, height: number, lineStyle: number) {
        let radius = Math.min(width, height);
        let hexagon = new egret.Shape();
        hexagon.width = width, hexagon.height = width;
        hexagon.graphics.beginFill(0x336699, 0);
        hexagon.graphics.lineStyle(2, lineStyle);
        for (let i = 0; i < 7; i++) {
            let tx = radius * .5 * Math.cos(((i * 60) % 360) * Math.PI / 180);
            let ty = radius * .5 * Math.sin(((i * 60) % 360) * Math.PI / 180);
            if (i === 0) {
                hexagon.graphics.moveTo(tx, ty);
            } else {
                hexagon.graphics.lineTo(tx, ty);
            }
        }
        hexagon.graphics.endFill();
        return hexagon;
    }

    public fillItem(name: string, x: number, y: number, width: number, height: number, locked: boolean = true, lineStyle = 0x00ff00) {
        this.x = x, this.y = y;
        this.width = width, this.height = height;
        let bg = this.drawHexagon(x, y, width, height, lineStyle);
        let desc = this.createTextField(name);
        let lock = this.createBitmapByName("lock.png");
        this.addChild(bg);
        this.addChild(desc);
        this.addChild(lock);
        this.bg = bg;
        this.desc = desc;
        this.lock = lock;
        if (locked) {
            this.lock.visible = locked;
        }
    }

    public setLockStatus(locked: boolean) {
        this.lock.visible = locked;
        this.bg.visible = !locked;
        this.desc.visible = !locked;
    }

    private createBitmapByName(name: string, width: number = 0, height: number = 0) {
        return Utils.instance.createBitmapByName(name, width, height);
    }

    private createTextField(des: string, size: number = 24, color: number = 0xffffff) {
        return Utils.instance.createTextField(des, size, color);
    }
}