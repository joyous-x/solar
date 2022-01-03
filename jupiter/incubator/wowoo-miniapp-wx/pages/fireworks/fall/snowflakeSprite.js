import Particle from './particle'

var utils = require('../../../utils/util.js')

class SnowflakeSprite extends Particle {
    constructor({ x, y, minSize, maxSize, velocity=0.5, opacity=0.8 } = {}) {
        super({ x, y, size: utils.util.randomInt(minSize, maxSize), velocity, opacity });
        this.v = velocity * (1 + Math.random());
        this.angle = 0.5 * Math.PI;
        this.opacity = utils.util.randomIn(opacity, 1);
        this.xshake = Math.random() > 0.5 ? 1 : -1;
        this.rgb = utils.hsltorgb(255, 100, 100);
    }

    move() {
        this.x += Math.random() * this.xshake;
        this.x += this.v * Math.cos(this.angle);
        this.y += this.v * Math.sin(this.angle);
    }

    render(ctx) {
        this.move();

        if (this.outOfBounds()) return false;

        this.g = ctx.createCircularGradient(this.x, this.y, this.size + 1);
        this.g.addColorStop(0, `rgba(${this.rgb[0]},${this.rgb[1]},${this.rgb[2]},${this.opacity})`);
        this.g.addColorStop(0.8, `rgba(${this.rgb[0]},${this.rgb[1]},${this.rgb[2]},0.3)`);
        ctx.beginPath();
        ctx.fillStyle = this.g;
        ctx.arc(this.x, this.y, this.size, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.stroke()
        return true;
    }
}

export default SnowflakeSprite