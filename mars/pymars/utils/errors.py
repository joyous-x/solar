# -*- coding: utf-8 -*-

class ErrCode(object):
    def __init__(self, code, msg):
        self.code = code
        self.msg  = msg

ErrOK = ErrCode(0, "ok")
ErrInvalidArgs = ErrCode(100010001, "invalid args")
ErrRequestHtml = ErrCode(100010002, "request html content error")
ErrParseHtml = ErrCode(100010003, "parse html content error")
ErrHostNotReserved = ErrCode(100010004, "host not reserved")
ErrServerBad = ErrCode(100010005, "server error")
