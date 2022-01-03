class SoundCenter {
    private static _instance: SoundCenter;
    private tap: egret.Sound;
    private win: egret.Sound;
    private fail: egret.Sound;
    private collideGear: egret.Sound;
    private collideArrow: egret.Sound;
    private collideOnce: egret.Sound;
    private collideGreen: egret.Sound;
    private transfer: egret.Sound;
    private collide: egret.Sound;
    private bgm: egret.Sound;
    private bgm_sc: egret.SoundChannel;

    // reference: https://developer.egret.com/en/article/index/id/156

    private newSound(sound: string) {
        return RES.getRes(sound);
    }

    private constructor() {
        this.tap = this.newSound("tap_wav")
        this.bgm = this.newSound("bgm-faraway_mp3")
        this.win = this.newSound("win_mp3");
        this.fail = this.newSound("fail_mp3");
        this.collideGear = this.newSound("collide.gear_mp3");
        this.collideOnce = this.newSound("collide.once_mp3");
        this.collideArrow = this.newSound("collide.arrow_mp3");
        this.collideGreen = this.newSound("collide.green_mp3");
        this.transfer = this.newSound("transfer_mp3");
        this.collide = this.newSound("collide_mp3");
    }

	public static get instance() {
		if (this._instance == null) {
			this._instance = new SoundCenter();
		}
		return this._instance;
    }

    public music(opt: {play: boolean} = undefined) {
        let status = LocalStore.instance.getSwitchMusic()
        if (opt) {
            status = opt.play;
        }
        if (status) {
            this.bgm_sc = this.bgm.play();
        } else {
            if (this.bgm_sc) {
                this.bgm_sc.stop();
                this.bgm_sc = null;
            }
        }
    }

    public voice(target: string) {
        if (!LocalStore.instance.getSwitchVoice()) {
            return;
        }
        let sound: egret.Sound;
        switch(target) {
            case "tap":
                sound = this.tap; break;
            case "win":
                sound = this.win; break;
            case "fail":
                sound = this.fail; break;
            case "transfer":
                sound = this.transfer; break;
            case "collide":
                sound = this.collide; break;
            case "collide.gear":
                sound = this.collideGear; break;
            case "collide.once":
                sound = this.collideOnce; break;
            case "collide.arrow":
                sound = this.collideArrow; break;
            case "collide.green":
                sound = this.collideGreen; break;
        }
        sound.play(0, 1);
    }
}