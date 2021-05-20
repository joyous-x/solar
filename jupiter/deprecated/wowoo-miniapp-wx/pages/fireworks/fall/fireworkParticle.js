import Particle from './particle'

var utils = require('../../../utils/util.js')

class FireworkParticle extends Particle {
    constructor({ x, y, size, rgb, acceleration=-0.08 }) {
        super({ x, y, size, acceleration });
        this.v = utils.util.randomIn(1, 5);
        this.angle = Math.PI * 2 * Math.random();
        this.size = this.size * utils.util.randomIn(0.8, 1);
        this.radius = this.size / 2;

        this.rgb = rgb;
        // v/a= lifecycle
        // alpha ---(lifecycle)---> 1 ---(0.2 * lifecycle) ---> 0
        this.alpha = 0.2;
        this.decay = this.decayUp;
        this.decayUp = Math.abs((1 - this.alpha) * this.a / (this.v * 0.9));
        this.decayDown = Math.abs(this.a / (this.v * 0.36));

        this.coordinates = [];
        this.coordinateCount = 5;
        while (this.coordinateCount--) {
            this.coordinates.push([this.x, this.y]);
        }
    }

    move() {
        this.timer += 1;
        this.x += this.v * Math.cos(this.angle);
        this.y += this.v * Math.sin(this.angle);
        this.y += utils.Gravity * this.timer / 2;
        this.v += this.a;

        if (this.v <= 0) {
            this.v = 0;
            this.decay = this.decayDown;
            this.alpha -= this.decay;
        } else {
            this.decay = this.decayUp;
            this.alpha = this.alpha + this.decay > 1 ? 1 : this.alpha + this.decay;
        }

        this.coordinates.pop();
        this.coordinates.unshift([this.x, this.y]);
    }

    render(ctx) {
        this.move();

        // alpha足够低时，移除粒子
        if (this.outOfBounds() || (this.alpha <= 0)) return false;

        ctx.beginPath();

        ctx.moveTo(this.coordinates[this.coordinates.length - 1][0], this.coordinates[this.coordinates.length - 1][1]);
        ctx.lineTo(this.x, this.y);

        ctx.strokeStyle = `rgba(${this.rgb[0]}, ${this.rgb[1]}, ${this.rgb[2]}, ${this.alpha})`;
        ctx.stroke();

        ctx.fillStyle = `rgba(${this.rgb[0]}, ${this.rgb[1]}, ${this.rgb[2]}, 1)`;
        ctx.arc(this.x, this.y, this.radius * this.alpha, 0, Math.PI * 2, false);
        ctx.fill();
    }
}


export default FireworkParticle