import {install} from '@pixi/unsafe-eval'

install(PIXI)

PIXI.settings.PREFER_ENV = PIXI.ENV.WEBGL
PIXI.settings.PRECISION_FRAGMENT = PIXI.PRECISION.HIGH
PIXI.settings.SORTABLE_CHILDREN = true
