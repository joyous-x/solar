import Particle from './particle'

var utils = require('../../../utils/util.js')

class HeartSprite extends Particle {
    constructor({ x, y, minSize, maxSize, velocity=1, opacity=0 } = {}) {
        super({ x, y, size: utils.util.randomInt(minSize, maxSize), velocity, opacity });
        this.v = velocity * (1 + Math.random());
        this.angle = 0.5 * Math.PI;
        this.xshake = Math.random() > 0.5 ? 1 : -1;
        let rgb = utils.hsltorgb(Math.random() * 360, 90, 65)
        this.rgb = `rgba(${rgb[0]}, ${rgb[1]}, ${rgb[2]}, 1)`;
    }

    move() {
        this.x += Math.random() * this.xshake;
        this.x += this.v * Math.cos(this.angle);
        this.y += this.v * Math.sin(this.angle);
    }

    render(ctx) {
        this.move();

        if (this.outOfBounds()) return false;

        ctx.save();
        ctx.beginPath();
        ctx.fillStyle = this.rgb;
        ctx.moveTo(this.x + 0.5 * this.size, this.y + 0.3 * this.size);
        ctx.bezierCurveTo(this.x + 0.1 * this.size, this.y, this.x,
            this.y + 0.6 * this.size, this.x + 0.5 *
            this.size, this.y + 0.9 * this.size);
        ctx.bezierCurveTo(this.x + 1 * this.size, this.y + 0.6 *
            this.size, this.x + 0.9 * this.size, this.y,
            this.x + 0.5 * this.size,
            this.y + 0.3 * this.size);
        ctx.closePath();
        ctx.fill();
        ctx.restore();

        return true;
    }
}

export default HeartSprite