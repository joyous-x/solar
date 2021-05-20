import config from '../config'
import FireworkParticle from '../fall/fireworkParticle'
import FireworkWords from './fireworkWords'
import RaiserSprite from '../fall/raiserSprite'

var utils = require('../../../utils/util.js')

class Firework {
	constructor({raiserSize=2, velocity=3, count=[200,250], dots, prtOption = {}} = {}){
		this.x = utils.util.randomIn(config.width / 7, config.width * 6 / 7);
        this.y = config.height;
        this.yEnd = utils.util.randomIn(config.height / 7, config.height / 3);

        this.hue = 360 * Math.random() | 0;
        this.rgb = utils.hsltorgb(this.hue, utils.util.randomInt(70,90), utils.util.randomInt(50,70));

        this.raiserSprite = new RaiserSprite({x:this.x, y:this.y, yEnd:this.yEnd, size:raiserSize, rgb:this.rgb, velocity});

		if(!dots){
            this.type = 'normal';
            this.particles = [];
            this.count = utils.util.randomIn(...count) | 0;
            for(let i = 0;i < this.count;++i){
                this.particles.push(
                    new FireworkParticle({x:this.x, y:this.yEnd, size:raiserSize * 1.2, rgb:this.rgb })
                );
            }
		}else{
			this.type = 'words';
			const option = {xStart: this.xEnd, yStart: this.yEnd};
			this.particles = dots.map(dot => new FireworkWords(utils.util.extendArr({}, dot, option, prtOption)));
        }

        // 音效
        const audioBomb = new wx.createInnerAudioContext();
        audioBomb.src = config.audioBomb;
        audioBomb.loop = false;
        audioBomb.volume = 0.8;
        this.audioBomb = audioBomb
        const audioChos = new wx.createInnerAudioContext();
        audioChos.src = config.audioChos;;
        audioChos.loop = false;
        audioChos.volume = 0.4;
        this.audioChos = audioChos;
        this.audioStatus = 0;
        this.renderTimer = 0; // 一般按 60帧/s 的速率渲染，以此来判断音效
        
        this.shouldWait = utils.util.randomInt(16, 32);
        this.status = 1;
	}
	render(ctx){
		switch (this.status){
			case 1:
				if (false == this.raiserSprite.render(ctx)) {
                    this.status = 2;
                    this.renderTimer = 0;
                }
				break;
			case 2:
            case 3:
				if(this.renderTimer >= this.shouldWait){
                    this.status = 4;
                    this.renderTimer = 0;
                }
                break;
			case 4:
				ctx.save();
				ctx.globalCompositeOperation = 'lighter';
				for(let i = 0;i < this.particles.length;++i){
                    if (false == this.particles[i].render(ctx)) {
                        this.particles.splice(i, 1);
                    }
                }
                ctx.restore();
                this.status = this.particles.length == 0 ? 0 : this.status;
                break;
        }
        this.renderTimer++;
        return this.status;
	}
    playAudio() {
        if (this.status == 1 && this.renderTimer == 2) {
            this.audioChos.play()
        }
        if (this.status == 4 && this.renderTimer == 1) {
            this.audioBomb.play()
        }
    }
}

export default Firework