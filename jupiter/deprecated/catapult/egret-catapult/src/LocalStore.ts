class LocalStore {
    private static _instance: LocalStore;
    private datas = {};
    private levelKey: string = "levels";

    private constructor() {
        this.loadDatas();
    }

	public static get instance() {
		if (this._instance == null) {
			this._instance = new LocalStore();
		}
		return this._instance;
	}

    public setUnlock(cnt: number) {
        cnt = cnt > 75 ? 75 : cnt;
        cnt = cnt < 1 ? 1 : cnt;
        this.datas["unlock"] = cnt;
        this.saveDatas();
    }

    public getUnlock() {
        return this.datas["unlock"] === undefined ? 1 : this.datas["unlock"];
    }

    public setLifes(cnt: number) {
        this.datas["lifes"] = cnt > 0 ? cnt : 0;
        this.datas["lifes_ts"] = new Date();
        this.saveDatas();
    }

    public getLifes() {
        let defaultLifes = 3;
        let now = new Date();
        let lastStr = this.datas["lifes_ts"];
        if (lastStr !== undefined) {
            let last = new Date(lastStr)
            if (last.valueOf() < now.valueOf()) {
                if (now.getDate() !== last.getDate() || now.getMonth() !== last.getMonth() || now.getFullYear() !== last.getFullYear()) {
                    this.setLifes(defaultLifes)
                }
            }
        } else {
            this.setLifes(defaultLifes)
        }
        return this.datas["lifes"];
    }

    public setHints(cnt: number) {
        this.datas["hints"] = cnt > 0 ? cnt : 0;
        this.datas["hints_ts"] = new Date();
        this.saveDatas();
    }

    public getHints() {
        let defaultHints = 3;
        let now = new Date();
        let lastStr = this.datas["hints_ts"];
        if (lastStr !== undefined) {
            let last = new Date(lastStr)
            if (last.valueOf() < now.valueOf()) {
                if (now.getDate() !== last.getDate() || now.getMonth() !== last.getMonth() || now.getFullYear() !== last.getFullYear()) {
                    this.setHints(defaultHints)
                }
            }
        } else {
            this.setHints(defaultHints)
        }
        return this.datas["hints"];
    }

    public setSwitchMusic(on: boolean) {
        this.datas["music"] = on;
        this.saveDatas();
    }

    public getSwitchMusic() {
        return this.datas["music"] === undefined ? true : this.datas["music"];
    }

    public setSwitchVoice(on: boolean) {
        this.datas["voice"] = on ? 1 : 0;
        this.saveDatas();
    }

    public getSwitchVoice() {
        return this.datas["voice"] === undefined ? true : this.datas["voice"];
    }

    private saveDatas() {
        egret.localStorage.setItem(this.levelKey,  JSON.stringify(this.datas));
    }

    private loadDatas() {
        let datas: string = egret.localStorage.getItem(this.levelKey);
        if (datas) {
            this.datas = JSON.parse(datas);
        }
    }

    private deleteDatas(key: string) {
        egret.localStorage.removeItem(key);
    }
}