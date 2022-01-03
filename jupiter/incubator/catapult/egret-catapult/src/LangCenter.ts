class LangCenter {
	private static _instance: LangCenter;
    public easy = "Easy";
    public midd = "Hard";
    public hard = "Nightware";
    public tryAgainPrompt = "Try Again?";
    public useHintPrompt = "Will use a Hint?";
    public noHintPrompt = "3 Hints a Day !\nYou Will Get Them Tomorrow.";
    public getHintPrompt = "No Hints !\nWill to Earn some ?";
    public getLifePrompt = "No Lifes ! (3 Lifes a Day) \nWill to Earn some ?";
    public passAllLevelPrompt = "Congratulations !\nYou Are the Best !!!";
    public sceneLockedPrompt = "Locked !"

    public sceneHome = "home";
    public sceneGame = "game";
    public sceneSetting = "setting";
    public sceneEasy = "easy";
    public sceneMidd = "hard";
    public sceneHard = "nightware";

	private constructor() {
	}

	public static get instance() {
		if (this._instance == null) {
			this._instance = new LangCenter();
		}
		return this._instance;
	}

}