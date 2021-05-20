// anims.js
export const zooData = zooDataFun();
export const appConf = appConfigFun();
export const apiUrls = appApiUrlFun();
export const appResc = rscConfigFun();

function appConfigFun() {
    var conf = {
        host: "https://smilex.xyz",
        appname: "wowoo",
    }
    return conf
}

function rscConfigFun() {
    var conf = {
        indexImgBg: "/resources/images/bg-cover.png",
        fksAudioBgm: appConf.host + "/resources/sounds/city-of-sky.mp3",
        fksImgBg: "/resources/images/bg-night.jpg",
        fksImgAudioOn: "/resources/images/musicon.png",
        fksImgAudioOff: "/resources/images/musicoff.png",
        fksAudioBomb: "/resources/sounds/firework-bomb.mp3",
        fksAudioChou: "/resources/sounds/firework-chos.wav",
        animsImgBg: appConf.host + "/resources/images/bg/tuoyuan-b.jpg",
        minImgBg: appConf.host + "/resources/images/bg/tuoyuan-a.jpg",
        minImgRefresh: "/resources/images/refresh.png",
        animsRootDir: '/resources/images/anims/',
    }
    return conf
}

function zooDataFun() {
    var data = {
        "lion": {
            name:'狮子', alias:'', name_en:'lion', thumbnail:'a.png',
            imgdir: '/resources/images/anims/lion/', wowurl: '/resources/sounds/anims/lion.mp3',
            imgs: ['a.png', 'b.jpg', 'c.jpg']
        },
        "zebra": {
            name:'斑马', alias:'', name_en:'zebra', thumbnail:'b@1x.png',
            imgdir: '/resources/images/anims/zebra/', wowurl: '/resources/sounds/anims/zebra.mp3',
            imgs: ['a@1x.png', 'b@1x.png', 'c@1x.png']
        },
        "hippopotamus": {
            name:'河马', alias:'', name_en:'hippopotamus', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/hippopotamus/', wowurl: '/resources/sounds/anims/hippopotamus.mp3',
            imgs: ['a@1x.png']
        },
        "crocodile": {
            name:'鳄鱼', alias:'', name_en:'crocodile', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/crocodile/', wowurl: '/resources/sounds/anims/crocodile.mp3',
            imgs: ['a@1x.png']
        },
        "rhinoceros": {
            name:'犀牛', alias:'', name_en:'rhinoceros', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/rhinoceros/', wowurl: '/resources/sounds/anims/rhinoceros.mp3',
            imgs: ['a@1x.png']
        },
        "elephant": {
            name:'大象', alias:'', name_en:'elephant', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/elephant/', wowurl: '/resources/sounds/anims/elephant.mp3',
            imgs: ['a@1x.png', 'b@1x.png']
        },
        "leopard": {
            name:'豹子', alias:'', name_en:'leopard', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/leopard/', wowurl: '/resources/sounds/anims/leopard.mp3',
            imgs: ['a@1x.png']
        },
        "giraffe": {
            name:'长颈鹿', alias:'', name_en:'giraffe', thumbnail:'a@1x.png',
            imgdir: '/resources/images/anims/giraffe/', wowurl: '/resources/sounds/anims/giraffe.mp3',
            imgs: ['a@1x.png', 'b@1x.png']
        },
        "cat": {
            name:'猫', alias:'', name_en:'cat', thumbnail:'a.jpg',
            imgdir: '/resources/images/anims/cat/', wowurl: '/resources/sounds/anims/cat.mp3',
            imgs: ['a.jpg', 'b.jpg', 'c.jpg', 'd.jpg', 'e.jpg', 'f.jpg', 'g.jpg', 'h.jpg', 'i.png', 'j.jpg']
        },
        "dog": {
            name:'狗', alias:'', name_en:'dog', thumbnail:'a.png',
            imgdir: '/resources/images/anims/dog/', wowurl: '/resources/sounds/anims/dog.mp3',
            imgs: ['a.png', 'b.png', 'c.png', 'd.jpg', 'e.png', 'f.jpg']
        },
        "dolphin": {
            name:'海豚', alias:'', name_en:'dolphin', thumbnail:'a.png',
            imgdir: '/resources/images/anims/dolphin/', wowurl: '/resources/sounds/anims/dolphin.mp3',
            imgs: ['a.png', 'b.jpg', 'c.jpg', 'd.jpg']
        },
        "penguin": {
            name:'企鹅', alias:'', name_en:'penguin', thumbnail:'a.jpg',
            imgdir: '/resources/images/anims/penguin/', wowurl: '/resources/sounds/anims/penguin.mp3',
            imgs: ['a.jpg', 'b.jpg', 'c.jpg', 'd.jpg', 'e.jpg', 'f.jpg', 'g.jpg', 'h.jpg', 'i.jpg', 'j.jpg', 'k.jpg', 'l.jpg']
        },
        "polar-bear": {
            name:'北极熊', alias:'', name_en:'polar-bear', thumbnail:'a.png',
            imgdir: '/resources/images/anims/polar-bear/', wowurl: '/resources/sounds/anims/polar-bear.mp3',
            imgs: ['a.png', 'b.png', 'c.jpg', 'd.jpg', 'e.jpg', 'f.jpg', 'g.jpg', 'h.jpg', 'i.jpg', 'j.jpg']
        },
        "dinosaur": {
            name:'恐龙', alias:'', name_en:'dinosaur', thumbnail:'a.png',
            imgdir: '/resources/images/anims/dinosaur/', wowurl: '/resources/sounds/anims/dinosaur.mp3',
            imgs: ['a.png', 'b.png']
        },
        "wolf": {
            name:'狼', alias:'', name_en:'wolf', thumbnail:'a.png',
            imgdir: '/resources/images/anims/wolf/', wowurl: '/resources/sounds/anims/wolf.mp3',
            imgs: ['a.png']
        },
        "panda": {
            name:'大熊猫', alias:'', name_en:'panda', thumbnail:'a.png',
            imgdir: '/resources/images/anims/panda/', wowurl: '/resources/sounds/anims/panda.mp3',
            imgs: ['a.png','b.jpg','c.png']
        },
        "camel": {
            name:'骆驼', alias:'', name_en:'camel', thumbnail:'a.jpg',
            imgdir: '/resources/images/anims/camel/', wowurl: '/resources/sounds/anims/camel.mp3',
            imgs: ['a.jpg']
        },
        "chicken": {
            name:'小鸡', alias:'', name_en:'chicken', thumbnail:'a.png',
            imgdir: '/resources/images/anims/chicken/', wowurl: '/resources/sounds/anims/chicken.mp3',
            imgs: ['a.png', 'b.jpg', 'c.jpg', 'd.jpg', 'e.jpg', 'f.jpg', 'g.jpg', 'h.jpg']
        },
        "horse": {
            name:'马', alias:'', name_en:'horse', thumbnail:'a.png',
            imgdir: '/resources/images/anims/horse/', wowurl: '/resources/sounds/anims/horse.mp3',
            imgs: ['a.png']
        },
        "rabbit": {
            name:'兔子', alias:'', name_en:'rabbit', thumbnail:'a.png',
            imgdir: '/resources/images/anims/rabbit/', wowurl: '/resources/sounds/anims/rabbit.mp3',
            imgs: ['a.png']
        },
        "sheep": {
            name:'羊', alias:'', name_en:'sheep', thumbnail:'a.png',
            imgdir: '/resources/images/anims/sheep/', wowurl: '/resources/sounds/anims/sheep.mp3',
            imgs: ['a.png']
        },
        "monkey": { // no sound
            name:'猴子', alias:'', name_en:'monkey', thumbnail:'a.png',
            imgdir: '/resources/images/anims/monkey/', wowurl: '/resources/sounds/anims/monkey.mp3',
            imgs: ['a.png']
        },
        "kangaroo": { // no sound
            name:'袋鼠', alias:'', name_en:'kangaroo', thumbnail:'a.png',
            imgdir: '/resources/images/anims/kangaroo/', wowurl: '/resources/sounds/anims/kangaroo.mp3',
            imgs: ['a.png']
        },
    };
    return data
}

function appApiUrlFun() {
    var conf = {
        login: "/v1/wx/user/login",
        userinfo: "/v1/wx/user/update"
    }
    return conf
}