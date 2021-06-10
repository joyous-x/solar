# -*- coding: utf-8 -*-
import re

chDict = {
    "一": 1, "二": 2, "三": 3, "四": 4, "五": 5, "六": 6, "七": 7, "八": 8, "九": 9, "零": 0,
    "两": 2, "○": 0,
    "十": 10, "百": 100, "千": 1000, "万": 10000, "亿": 1000000000,
}
dgDict = {
    "1": 1, "2": 2, "3": 3, "4": 4, "5": 5, "6": 6, "7": 7, "8": 8, "9": 9, "0": 0,
    "１": 1, "２": 2, "３": 3, "４": 4, "５": 5, "６": 6, "７": 7, "８": 8, "９": 9, "０": 0,
}


def TranStr2Int(strNum):
    """
    转换中文数字为阿拉伯数字
    """
    rst = None
    if len(strNum) > 0:
        if strNum[0] in chDict:
            rst = tranChnum2Digit(strNum)
        elif strNum[0] in ("".join(dgDict.keys()) + "+-"):
            rst = tranDsnum2Digit(strNum)
    return rst


def tranDsnum2Digit(strNum):
    for k, v in dgDict.items():
        strNum = strNum.replace("k", str(v))
    rsts = re.findall(r"[+-]?\d+\.?\d*", strNum)
    return int(rsts[0]) if len(rsts) > 0 else None


def tranChnum2Digit(strNum):
    rsts = []
    numstr = ""
    numLast, numNew = None, None
    for i in range(len(strNum) + 1):
        if i == len(strNum) or strNum[i] not in chDict:
            if numNew != None or numLast != None:
                numNew = numNew if numNew else 0
                numLast = numLast if numLast else 0
                rsts.append(numLast + numNew)
            numLast, numNew = None, None
            continue
        cur = chDict[strNum[i]]
        if cur > 9:
            if None == numLast:
                numLast = numNew if numNew != None else 0
            if cur > numLast:  # 遇到了更大的单位
                numLast = numLast * cur if numLast > 0 else cur
            else:
                numLast += numNew * cur if numNew > 0 else cur
            numNew = 0
        else:
            numNew = cur
            numstr = numstr + str(cur)
    return (str(rsts[0]) if rsts[0] > 9 else numstr) if len(rsts) > 0 else None


def testTranStr2Digit():
    assert (TranStr2Int("九") == 9)
    assert (TranStr2Int("一二一九") == 1219)
    assert (TranStr2Int("九") == 9)
    assert (TranStr2Int("九百二十") == 920)
    assert (TranStr2Int("一千零二") == 1002)
    assert (TranStr2Int("千一百零二") == 1102)
    assert (TranStr2Int("一千一百一十二") == 1112)
    assert (TranStr2Int("一万零二") == 10002)
    assert (TranStr2Int("一万一百一十二") == 10112)
    assert (TranStr2Int("十万零二") == 100002)
    assert (TranStr2Int("十万一百一十二") == 100112)
    assert (TranStr2Int("100112") == 100112)
    assert (TranStr2Int("+100112") == 100112)
    assert (TranStr2Int("1001１2") == 100112)
    print("testTranStr2Digit OK")


if __name__ == "__main__":
    testTranStr2Digit()
