# coding=utf-8

def testCode():
    data=''' 惠普﻿英特尔 '''
    rst = data.decode('utf-8').encode('gbk','ignore').decode('gbk')
    print(rst)

if __name__ == "__main__":
    testCode()