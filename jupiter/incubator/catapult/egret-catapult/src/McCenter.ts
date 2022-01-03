class McCenter {
    private static _instance: McCenter;
    private mcFactory: egret.MovieClipDataFactory;

    private constructor() {
        var data2mc = RES.getRes("mcs.json");
        var texture2mc = RES.getRes("mcs.png");
        let mcFactory : egret.MovieClipDataFactory = new egret.MovieClipDataFactory(data2mc, texture2mc);
        this.mcFactory = mcFactory;
    }

	public static get instance() {
		if (this._instance == null) {
			this._instance = new McCenter();
		}
		return this._instance;
	}

    public newMovieClip(movieClipName?: string) {
        return new egret.MovieClip(this.mcFactory.generateMovieClipData(movieClipName));
    }

}