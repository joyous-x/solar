class CfgLogo extends Object {
    public static data = {
        "logo": {
            start: { x:400, y: 1900 },
            end: { x: 500, y: 600 },
            baffles: [                
                { frame: 'arrow.right.png', x: 100, y: 500 },
                { frame: 'arrow.down.png', x: 900, y: 500 },
                { frame: 'arrow.up.png', x: 100, y: 1300 },
                { frame: 'arrow.left.png', x: 900, y: 1300 },

                { frame: 'square.once.png', x: 500, y: 1300 }, 
                { frame: 'triangle.png', x: 400, y: 600, angle: -45 },
                { frame: 'triangle.png', x: 600, y: 600, angle: 135 },

                { frame: 'pink.png', x: 400, y: 800, angle: 45, shadow: 45 },
                { frame: 'pink.png', x: 400, y: 1000, angle: -45, shadow: 135 },
                { frame: 'ring.png', x: 500, y: 900 },
                { frame: 'pink.png', x: 600, y: 800, angle: -45, shadow: 45 },
                { frame: 'pink.png', x: 600, y: 1000, angle: 45 },
                { frame: 'blue.png', x: 600, y: 1200, angle: -45 },
                { frame: 'gear.png', x: 800, y: 1000, angle: -45 },
                { frame: 'blue.png', x: 400, y: 1200, angle: 45 },
                { frame: 'green.png', x: 200, y: 1000, angle: 45 },
            ]
        },
    }
}