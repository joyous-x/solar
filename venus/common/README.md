# Common

## Features
- [x] VBA Stomping ：P-CODE 
- [x] 解析 xls 中的 macro(包含处理常见的引用、反混淆)
- [x] VBA、XLM、Embedded OLE 解析
- [x] 清理：stream 维度的清理、删除功能
- [x] 解密算法支持(ecma376 std/agile、xor、rc4、rc4 capi)
  - [ ] ecma agile: verify data integrity
- [x] office 转换而成的 xml、mhtml、html
- [x] wps
  - 可以有 js宏、vba宏 ˙两种，早期 vba 宏仅在WPS+企业付费套餐（商业版/高级商业版）中支持
- 待处理格式
  - [ ] NORMAL.DOT模板
  - [ ] doc macro(word7) ?
  - [ ] DDE links ：msodde.py
  - [ ] linkshell
  - [ ] VBA forms ：oleform.py : parse [VBA forms](https://msdn.microsoft.com/en-us/library/office/cc313125%28v=office.12%29.aspx?f=255&MSPPError=-2147217396) in Microsoft Office files.
  - [ ] Flash objects: pyxswf.py ：extract and analyze Flash objects (SWF) that may be embedded in  MS Office documents
  - [ ] rtf : rtfobj.py
  - [ ] pdf
  - [ ] xlsb、cad、pptm\ppsm、SYLK/SLK
  
## Q & A

## Reference
- [olevba](https://github.com/decalage2/oletools)