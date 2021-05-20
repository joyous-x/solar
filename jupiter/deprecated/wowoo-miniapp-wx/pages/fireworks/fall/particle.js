import config from '../config.js'

var utils = require('../../../utils/util.js')

// note:
//     假定：初速 和 angle 同向或相向
class Particle {
    constructor({ x, y, size, angle=0, velocity=0, acceleration=0, opacity=0 } = {}) {
        this.size = size;
        this.x = x ? x : utils.util.randomIn(0, (config.width - this.size));
        this.y = y ? y : -this.size;
        this.v = velocity;
        this.a = acceleration;
        this.angle = angle;
        this.opacity = opacity;
        this.timer = 0;
    }

    outOfBounds(height = config.height, width = config.width) {
        if (this.x >= -this.size && this.x <= width && this.y <= height && this.y >= -this.size) return false;
        return true;
    }

    move() {
        // TODO: move particle
    }

    render(ctx) {
        this.move()
        // TODO: logic for render
    }
}

export default Particle