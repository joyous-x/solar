class Utils {
	private static _instance: Utils;

	private constructor() {
	}

	public static get instance() {
		if (this._instance == null) {
			this._instance = new Utils();
		}
		return this._instance;
    }

    public createBitmapByName(name: string, width: number = 0, height: number = 0) {
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
        return result;
    }

    public createTextField(des: string, size: number = 25, color: number = 0xffffff, diameter: number = 0) {
        let textfield = new egret.TextField();
        textfield.alpha = 1;
        textfield.size = size;
        textfield.text = des;
        textfield.textAlign = egret.HorizontalAlign.CENTER;
        textfield.textColor = color;
        textfield.lineSpacing = 10;
        if (diameter) {
            textfield.width = diameter;
            textfield.height = diameter;
        }
        textfield.anchorOffsetX = textfield.width * .5;
        textfield.anchorOffsetY = textfield.height * .5;
        return textfield;
    }

    public distance(p: {x:number, y:number}, q: {x:number, y:number}) {
        return Math.sqrt((p.x - q.x) ** 2 + (p.y - q.y) ** 2)
    }

    public normalAngle(angle: number) {
        return Math.round(angle + 360) % 360;
    }

    public catapultAngle(ballAngle: number, baffleAngle: number) {
        return  (360 + 2 * this.normalAngle(baffleAngle) - this.normalAngle(ballAngle)) % 360;
    }
}