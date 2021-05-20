class LevelSelector extends SceneBase {
    private datas: LevelSelectorItem[] = new Array();
    private scene: string;
    private decorator: SceneDecorator;
    private row: number;
    private col: number;

    public constructor(column: number, row:number, stageW: number, stageH: number) {
        super();
        this.addEventListener(egret.Event.ADDED_TO_STAGE, this.onAddToStage, this);
        this.width = stageW;
        this.height = stageH;
        this.col = column;
        this.row = row;
    }

    private onAddToStage(event: egret.Event) {
    }

    private drawItems(x: number, y:number, stageW: number, stageH: number, lineStyle: number) {
        const itemSize = 80, itemGap = 20;
        let startX = (stageW - (x - 1) * (itemGap + itemSize)) * .5;
        let startY = (stageH - (y - 1) * (itemGap + itemSize)) * .5;

        this.row = x, this.col = y;
        for (let j = 0; j < y; j++) {
            for (let i = 0; i < x; i++)  {
                var mx = startX + i * (itemGap + itemSize);
                var my = startY + j * (itemGap + itemSize);
                var m = new LevelSelectorItem();
                var index = i + x * j + 1;
                m.fillItem(String(index), mx, my, itemSize, itemSize, true, lineStyle);
                m.touchEnabled = true;
                m.addEventListener(egret.TouchEvent.TOUCH_TAP, this.onTabLevel.bind(this, index), this);
                this.addChild(m);
                this.datas.push(m);
            }
        }

        let decorator = new SceneDecorator();
        decorator.show(0, 0, stageW, stageH * .2);
        this.addChild(decorator);        
        this.decorator = decorator;

        this.visible = false;
    }

    private onTabLevel(curScenelevel: number) {
        let topLevel = SceneManager.instance.transLevel(this.scene, curScenelevel)
        if (!SceneManager.instance.isLevelLocked(topLevel)) {
            SceneManager.instance.navigateTo(LangCenter.instance.sceneGame, {level: topLevel, pop:false})
        } else {
            console.debug("---> locked: " + this.scene + "-" + curScenelevel);
        }
    }

    public updateShow(opt = {scene: "", level: 0}) {
        let lineStyle: number;
        switch(opt.scene) {
            case LangCenter.instance.sceneEasy:
                lineStyle = 0x00FF00;
                break;
            case LangCenter.instance.sceneMidd:
                lineStyle = 0xFF6347;
                break;
            case LangCenter.instance.sceneHard:
                lineStyle = 0xFFFF00;
                break;
        }

        this.datas = [];
        this.removeChildren();
        this.drawItems(this.col, this.row, this.width, this.height, lineStyle);

        for (let j = 0; j < this.col; j++) {
            for (let i = 0; i < this.row; i++)  {
                var index = i + this.row * j + 1;
                var level = SceneManager.instance.transLevel(opt.scene, index);
                let item = this.datas[index - 1];
                item.setLockStatus(SceneManager.instance.isLevelLocked(level));
            }
        }
        this.scene = opt.scene;
        this.visible = true;
    }
}
