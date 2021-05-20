class SceneManager {
	private static _instance: SceneManager;
	private root: Main;
    private stack: egret.DisplayObjectContainer[];

	private constructor() {
        this.stack = new Array();
	}

	public static get instance() {
		if (this._instance == null) {
			this._instance = new SceneManager();
		}
		return this._instance;
	}

	public setRoot(root:Main){
		this.root=root;
	}

    private pushScene(scene: egret.DisplayObjectContainer) {
        if (this.stack.length > 0) {
            this.stack[this.stack.length - 1].$setVisible(false);
        }
        this.root.addChild(scene);
        this.stack.push(scene);
    }

    private popScene() {
        if (this.stack.length < 1) {
            return null;
        }
        const last = this.stack.pop();
        this.root.removeChild(last);
        if (this.stack.length > 0) {
            this.stack[this.stack.length - 1].visible = true;
        }
        return last;
    }

    public navigateTo(scene: string, opt = {level:1, pop:false}) {
        if (opt.pop) {
            this.popScene();
        }
        let target;
        let lineStyle = 0;
        switch(scene) {
            case LangCenter.instance.sceneHome: {
                this.root.updateShow();
                break;
            }
            case LangCenter.instance.sceneEasy:
                if (lineStyle === 0) lineStyle = 0x00FF00;
            case LangCenter.instance.sceneMidd:
                if (lineStyle === 0) lineStyle = 0xFF6347;
            case LangCenter.instance.sceneHard: {
                if (lineStyle === 0) lineStyle = 0xFFFF00;
                let target = new LevelSelector(5, 5, this.root.stage.stageWidth, this.root.stage.stageHeight)
                this.pushScene(target);
                target.updateShow({scene: scene, level: opt.level});
                break;
            }
            case LangCenter.instance.sceneSetting: {
                let target = new SettingSprite();                
                this.pushScene(target);
                target.updateShow();
                break;
            }
            case LangCenter.instance.sceneGame: {
                let target = new GameSprite();
                this.pushScene(target);
                target.updateShow({scene: scene, level: opt.level});
                break;
            }
        }
    }

    public navigateBack(): boolean {
        let last = this.popScene();
        if (!last) {
            return false
        }
        if (this.stack.length < 1) {
            this.navigateTo(LangCenter.instance.sceneHome);
        } else {
            let opt = (last as SceneBase).sceneDetail();
            let curScene = this.stack[this.stack.length - 1];
            let scene = curScene as SceneBase;
            scene.updateShow(opt);
        }
        return true;
    }

    public refreshTop() {
        if (this.stack.length < 1) {
            this.navigateTo(LangCenter.instance.sceneHome);
        } else {
            let curScene = this.stack[this.stack.length - 1];
            let scene = curScene as SceneBase;
            scene.updateShow(scene.sceneDetail());
        }
        console.debug("---> refreshTop: " + this.stack.length)
    }

    public parseLevel(level: number): {scene: string, level: number} {
        if (level < 26) {
            return {scene: LangCenter.instance.sceneEasy, level: level % 26};
        } else if (level < 51) {
            return {scene: LangCenter.instance.sceneMidd, level: level % 51 - 25};
        } else if (level < 76) {
            return {scene: LangCenter.instance.sceneHard, level: level % 76 - 50};
        }
        return {scene: LangCenter.instance.sceneEasy, level: 1};
    }
    public transLevel(scene: string, level: number): number {
        switch(scene) {
            case LangCenter.instance.sceneEasy:
                return level;
            case LangCenter.instance.sceneMidd:
                return 25 + level;
            case LangCenter.instance.sceneHard:
                return 50 + level;
        }
        return 1;
    }
    public maxLevel(scene: string): number {
        switch(scene) {
            case LangCenter.instance.sceneEasy:
                return Object.getOwnPropertyNames(CfgEasy.data).length;
            case LangCenter.instance.sceneMidd:
                return Object.getOwnPropertyNames(CfgMidd.data).length;
            case LangCenter.instance.sceneHard:
                return Object.getOwnPropertyNames(CfgHard.data).length;
        }
        return 0;
    }
    public isSceneLocked(scene: string): boolean {
        let unlock = LocalStore.instance.getUnlock();
        switch(scene) {
            case LangCenter.instance.sceneEasy:
                return unlock < 1;
            case LangCenter.instance.sceneMidd:
                return unlock < 26;
            case LangCenter.instance.sceneHard:
                return unlock < 51;
        }
        return false;
    }
    public isLevelLocked(level: number): boolean {
        let unlock = LocalStore.instance.getUnlock();
        return level > unlock;
    }
    public getLevelNext(level: string): string {
        switch(level) {
            case LangCenter.instance.easy:
                return LangCenter.instance.midd;
            case LangCenter.instance.midd:
                return LangCenter.instance.hard;
            case LangCenter.instance.hard:
                return "";
        }
        return LangCenter.instance.easy;
    }
}