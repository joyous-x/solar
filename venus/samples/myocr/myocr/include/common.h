//
// Created by jiao on 2020/9/23.
//

#ifndef AIMBOOSTER_COMMON_H
#define AIMBOOSTER_COMMON_H

#define TAG "aim_ndk"

static const char *aJniPkg = "com/bpool/aimbooster/so/";
static const char *aJniClass = "com/bpool/aimbooster/so/Aiming";
#define LOGD(fmt, ...) printf("%s"fmt,"d:--:",##__VA_ARGS__);printf("\n")
#define LOGI(fmt, ...) printf("%s"fmt,"i:--:",##__VA_ARGS__);printf("\n")
#define LOGW(fmt, ...) printf("%s"fmt,"w:--:",##__VA_ARGS__);printf("\n")
#define LOGE(fmt, ...) printf("%s"fmt,"e:--:",##__VA_ARGS__);printf("\n")
#endif //AIMBOOSTER_COMMON_H
