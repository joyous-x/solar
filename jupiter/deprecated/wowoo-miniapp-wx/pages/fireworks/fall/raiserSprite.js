import Particle from './particle'

var utils = require('../../../utils/util.js')

class RaiserSprite extends Particle {
    constructor({ x, y, yEnd, size, rgb, velocity = 0.5, opacity = 0.8 } = {}) {
        super({ x, y, size, velocity, opacity });
        this.v = Math.abs(velocity);
        this.angle = 1.5 * Math.PI;
        this.opacity = utils.util.randomIn(opacity, 1);
        this.yEnd = yEnd;
        this.rgb = rgb;
    }

    move() {
        this.x += this.v * Math.cos(this.angle);
        this.y += this.v * Math.sin(this.angle);
        this.y += utils.Gravity;

        if (this.y - this.yEnd <= 50) {
            this.opacity = (this.y - this.yEnd) / 50;
        }

        this.v += this.a;

    }

    render(ctx) {
        this.move();

        if (this.outOfBounds() || this.y <= this.yEnd) return false;

        ctx.save();
        ctx.beginPath();
        ctx.globalCompositeOperation = 'lighter';
        ctx.globalAlpha = this.opacity;
        ctx.translate(this.x, this.y);
        ctx.scale(0.6, 2.0);
        ctx.translate(-this.x, -this.y);
        ctx.fillStyle = `rgba(${this.rgb[0]},${this.rgb[1]},${this.rgb[2]},1)`;
        ctx.arc(this.x + Math.sin(Math.PI * 2 * Math.random()) / 1.2, this.y, this.size, 0, Math.PI * 2, false);
        ctx.fill();
        ctx.stroke();
        ctx.restore();
        return true;
    }
}

export default RaiserSprite